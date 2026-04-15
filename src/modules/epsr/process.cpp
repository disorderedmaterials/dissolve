// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "math/mathFunc.h"

#include "base/sysFunc.h"
#include "classes/neutronWeights.h"
#include "classes/partialSet.h"
#include "classes/scatteringMatrix.h"
#include "classes/xRayWeights.h"
#include "io/export/data1D.h"
#include "keywords/module.h"
#include "main/dissolve.h"
#include "math/error.h"
#include "math/filters.h"
#include "math/ft.h"
#include "math/gaussFit.h"
#include "math/poissonFit.h"
#include "module/group.h"
#include "modules/energy/energy.h"
#include "modules/epsr/epsr.h"
#include "modules/gr/gr.h"
#include "modules/neutronSQ/neutronSQ.h"
#include "modules/sq/sq.h"
#include "modules/xRaySQ/xRaySQ.h"
#include "templates/algorithms.h"
#include "templates/array3D.h"
#include <functional>

// Run set-up stage
bool EPSRModule::setUp(Dissolve &dissolve, Flags<KeywordBase::KeywordSignal> actionSignals)
{
    // Default to applying generated potentials - an associated EPSRManager may turn this off in its own setup stage
    applyPotentials_ = true;

    // Check for exactly one Configuration referenced through target modules
    targetConfiguration_ = nullptr;
    std::optional<double> rho;
    for (auto *module : targets_)
    {
        // Retrieve source SQ module, and then the related RDF module
        auto optSQModule = module->keywords().get<const SQModule *, ModuleKeyword<const SQModule>>("SourceSQs");
        const SQModule *sqModule = nullptr;
        if (optSQModule)
            sqModule = optSQModule.value();
        if (!sqModule)
            return Messenger::error(
                "[SETUP {}] Target '{}' doesn't source any S(Q) data, so it can't be used as a target for the EPSR module.",
                name_, module->name());

        auto *grModule = sqModule->sourceGR();
        if (!grModule)
            return Messenger::error(
                "[SETUP {}] Target '{}'s S(Q) module doesn't reference a GRModule, it can't be used as a target "
                "for the EPSR module.",
                name_, module->name());
        // Check for number of targets, or different target if there's only 1
        auto rdfConfigs = grModule->keywords().getVectorConfiguration("Configurations");
        if (rdfConfigs.size() != 1)
            return Messenger::error(
                "[SETUP {}] GR module '{}' targets multiple configurations, which is not permitted when using "
                "its data in the EPSR module.",
                name_, grModule->name());

        if ((targetConfiguration_ != nullptr) && (targetConfiguration_ != rdfConfigs.front()))
            return Messenger::error("[SETUP {}] GR module '{}' targets a configuration which is different from another target "
                                    "module, and which is not permitted when using its data in the EPSR module.",
                                    name_, grModule->name());

        else
            targetConfiguration_ = rdfConfigs.front();

        rho = targetConfiguration_->atomicDensity();
    }

    // If a pcof file was provided, read in the parameters from it here
    if (!pCofFilename_.empty())
    {
        Messenger::print("[SETUP {}] Reading potential coefficients from '{}'...\n", name_, pCofFilename_);

        // Read in the coefficients / setup from the supplied file
        if (!readPCof(dissolve, pCofFilename_))
            return Messenger::error("[SETUP {}] Failed to read in potential coefficients from EPSR pcof file.\n", name_);

        // Set up the additional potentials - reconstruct them from the current coefficients
        auto rmaxpt = rMaxPT_ ? rMaxPT_.value() : PairPotential::range();
        auto rminpt = rMinPT_ ? rMinPT_.value() : rmaxpt - 2.0;
        if (expansionFunction_ == EPSRModule::GaussianExpansionFunction)
        {
            if (!generateEmpiricalPotentials(dissolve, targetConfiguration_->atomTypeVector(), rho.value_or(0.1), nCoeffP_,
                                             rminpt, rmaxpt, gSigma1_, gSigma2_))
            {
                return false;
            }
        }
        else
        {
            if (!generateEmpiricalPotentials(dissolve, targetConfiguration_->atomTypeVector(), rho.value_or(0.1), nCoeffP_,
                                             rminpt, rmaxpt, pSigma1_, pSigma2_))
            {
                return false;
            }
        }
    }

    // If an inpa file was provided, read in the parameters from it here
    if (!inpaFilename_.empty())
    {
        Messenger::print("[SETUP {}] Reading fit coefficients from '{}'...\n", name_, inpaFilename_);

        // Read in the coefficients / setup from the supplied file
        if (!readFitCoefficients(dissolve, inpaFilename_))
            return Messenger::error("[SETUP {}] Failed to read in fit coefficients from EPSR inpa file.\n", name_);
    }

    // Try to calculate the deltaSQ array
    updateDeltaSQ(dissolve.processingModuleData());

    // Clear any existing scattering matrix
    scatteringMatrix_ = std::nullopt;

    return true;
}

// Run main processing
Module::ExecutionResult EPSRModule::process(Dissolve &dissolve)
{
    std::string testDataName;

    // EPSR constants
    const auto mcoeff = 200;

    // Calculate some values if they were not provided
    auto rmaxpt = rMaxPT_ ? rMaxPT_.value() : PairPotential::range();
    auto rminpt = rMinPT_ ? rMinPT_.value() : rmaxpt - 2.0;
    auto ncoeffp = nCoeffP_ ? nCoeffP_.value() : std::min(int(10.0 * rmaxpt + 0.0001), mcoeff);

    // Print option summary
    Messenger::print("EPSR: Feedback factor is {}.\n", feedback_);
    for (auto &&[targetModule, weight] : targetWeights_)
        Messenger::print("EPSR: Weight for module data '{}' set to {}.\n", targetModule->name(), weight);
    Messenger::print("EPSR: {} functions will be used to approximate difference data.\n",
                     expansionFunctionTypes().keyword(expansionFunction_));
    Messenger::print("EPSR: Number of functions used in approximation is {}, sigma(Q) = {}.\n", ncoeffp, pSigma2_);
    if (modifyPotential_)
    {
        Messenger::print("EPSR: Perturbations to interatomic potentials will be generated with a frequency of {}.\n",
                         *modifyPotential_);
        if (applyPotentials_)
            Messenger::print("EPSR: Global pair potentials will be modified.\n");
        else
            Messenger::print("EPSR: Current potentials will not be modified.\n");
    }
    else
        Messenger::warn("EPSR: Fluctuation coefficients and resulting potentials will not be modified.\n");

    Messenger::print("EPSR: Range for potential generation is {} < Q < {} Angstroms**-1.\n", qMin_, qMax_);
    Messenger::print("EPSR: Weighting factor used when applying fluctuation coefficients is {}\n", weighting_);
    if (fluctuationSmoothing_)
        Messenger::print("EPSR: Coefficients will be smoothed (average length = 2N+1, N = {})", *fluctuationSmoothing_);
    if (saveDifferenceFunctions_)
        Messenger::print("EPSR: Difference functions will be saved.\n");
    if (saveEmpiricalPotentials_)
        Messenger::print("EPSR: Empirical potentials will be saved.\n");
    if (saveEstimatedPartials_)
        Messenger::print("EPSR: Estimated partials will be saved.\n");
    if (savePotentialCoefficients_)
        Messenger::print("EPSR: Potential coefficients will be saved.\n");
    if (saveSimulatedFR_)
        Messenger::print("EPSR: Simulated F(r) (from FT of F(Q)) will be saved.\n");
    Messenger::print("\n");

    /*
     * Do we have targets to refine against?
     */
    if (targets_.empty())
    {
        Messenger::error("At least one Module target containing suitable data must be provided.\n");
        return ExecutionResult::Failed;
    }

    if (!targetConfiguration_)
    {
        Messenger::error("No target configuration is set.\n");
        return ExecutionResult::Failed;
    }

    if (!targetConfiguration_->atomicDensity())
    {
        Messenger::error("No density available for target configuration '{}'\n", targetConfiguration_->name());
        return ExecutionResult::Failed;
    }
    auto rho = *targetConfiguration_->atomicDensity();

    /*
     * Realise and increase run counter
     */
    auto &moduleData = dissolve.processingModuleData();
    auto [runCount, runCountStatus] = moduleData.realiseIf<int>("RunCount", name(), GenericItem::InRestartFileFlag);
    if (runCountStatus == GenericItem::ItemStatus::Created)
        runCount = 0;
    ++runCount;

    /*
     * EPSR Main
     */

    // Set up storage for the changes to coefficients used to generate the empirical potentials
    const auto atomTypes = targetConfiguration_->atomTypeVector();
    const auto nAtomTypes = atomTypes.size();

    Array3D<double> fluctuationCoefficients(nAtomTypes, nAtomTypes, ncoeffp);
    fluctuationCoefficients = 0.0;

    // Create storage for our summed UnweightedSQ
    auto &calculatedUnweightedSQ = moduleData.realise<DoubleKeyedMap<Data1D>>("UnweightedSQ", name_);
    calculatedUnweightedSQ.clear(true);
    dissolve::for_each_pair(
        ParallelPolicies::par, atomTypes, [&](int indexI, auto atI, int indexJ, auto atJ)
        { calculatedUnweightedSQ[{atI->name(), atJ->name()}].setTag(std::format("{}-{}", atI->name(), atJ->name())); });

    // Get summed weights over all datasets
    const auto totalDataSetWeight =
        std::accumulate(targetWeights_.begin(), targetWeights_.end(), 0.0,
                        [](const auto acc, const auto &targetWeight) { return acc + targetWeight.second; }) +
        (targets_.size() - targetWeights_.size());

    // Initialise a scattering matrix if we haven't already
    if (!scatteringMatrix_)
        scatteringMatrix_.emplace(atomTypes);

    // Loop over target data
    auto rFacTot = 0.0;
    std::vector<double> rangedRFacTots(ranges_.size());
    for (auto *module : targets_)
    {
        /*
         * Retrieve data for this module
         */

        // Retrieve the weighted S(Q)/F(Q)
        if (!moduleData.contains("WeightedSQ", module->name()))
        {
            Messenger::error("Weighted partials data not found for target '{}'.\n", module->name());
            return ExecutionResult::Failed;
        }
        const auto &weightedSQ = moduleData.value<PartialSet>("WeightedSQ", module->name());

        // Get source SQModule in order to have access to the unweighted S(Q)
        auto optSQModule = module->keywords().get<const SQModule *, ModuleKeyword<const SQModule>>("SourceSQs");
        const SQModule *sqModule = nullptr;
        if (optSQModule)
            sqModule = optSQModule.value();
        if (!sqModule)
        {
            Messenger::error("Module '{}' doesn't source any S(Q) data, so it can't be used to augment the scattering matrix.",
                             module->name());
            return ExecutionResult::Failed;
        }

        // Retrieve the unweighted S(Q)/F(Q)
        if (!moduleData.contains("UnweightedSQ", sqModule->name()))
        {
            Messenger::error("Unweighted partials data not found for target '{}'.\n", sqModule->name());
            return ExecutionResult::Failed;
        }
        const auto &unweightedSQ = moduleData.value<PartialSet>("UnweightedSQ", sqModule->name());

        // Retrieve the ReferenceData
        if (!moduleData.contains("ReferenceData", module->name()))
        {
            Messenger::error("Reference data not found for target '{}'.\n", module->name());
            return ExecutionResult::Failed;
        }
        const auto &originalReferenceData = moduleData.value<Data1D>("ReferenceData", module->name());

        // Realise the r-factor array and make sure its object name is set
        auto &errors =
            moduleData.realise<Data1D>(std::format("RFactor//{}", module->name()), name_, GenericItem::InRestartFileFlag);

        /*
         * Calculate difference functions and current percentage errors in calculated vs reference target data.
         * Do this over the widest Q-range allowed by both datasets
         */

        // Get difference data container and form the difference between the reference and calculated data
        auto &differenceData =
            moduleData.realise<Data1D>(std::format("Difference//{}", module->name()), name(), GenericItem::InRestartFileFlag);
        differenceData = weightedSQ.total();
        differenceData *= -1.0;
        Interpolator::addInterpolated(originalReferenceData, differenceData);

        // Zero the difference function for points outside of the reference data range
        auto originalXMin = originalReferenceData.xAxis().front();
        auto originalXMax = originalReferenceData.xAxis().back();
        for (auto &&[x, y] : zip(differenceData.xAxis(), differenceData.values()))
            if (x < originalXMin || x > originalXMax)
                y = 0.0;

        // Calculate r-factor over fit range and store
        auto trimmedReferenceData = originalReferenceData;
        Filters::trim(trimmedReferenceData, qMin_, qMax_);
        const auto rFactorReport = Error::rFactor(trimmedReferenceData, weightedSQ.total());
        rFacTot += rFactorReport.error;
        errors.addPoint(dissolve.iteration(), rFactorReport.error);
        Messenger::print("Current R-Factor for reference data '{}' is {:.5f}.\n", module->name(), rFactorReport.error);

        // Calculate r-factor over specified ranges_
        for (auto &&[range, rangeTot] : zip(ranges_, rangedRFacTots))
        {
            if (range.minimum() < rFactorReport.firstX || range.maximum() > rFactorReport.lastX)
            {
                Messenger::warn("The specified range '{:.5f} to {:.5f}' is outside of the range of data '{}', which exists "
                                "between {:.5f} and {:.5f}",
                                range.minimum(), range.maximum(), module->name(), rFactorReport.firstX, rFactorReport.lastX);
            }
            const auto rangedRFactorError = Error::rFactor(trimmedReferenceData, weightedSQ.total(), range).error;
            rangeTot += rangedRFactorError;
            Messenger::print("Current R-Factor for reference data '{}' over range {:.5f} to {:.5f} is {:.5f}.\n",
                             module->name(), range.minimum(), range.maximum(), rangedRFactorError);
        }

        /*
         * Generate difference function for fitting, spanning (maximally) only the range requested
         */

        // Get difference and fit function objects
        auto &deltaFQ =
            moduleData.realise<Data1D>(std::format("DeltaFQ//{}", module->name()), name_, GenericItem::InRestartFileFlag);
        auto &deltaFQFit =
            moduleData.realise<Data1D>(std::format("DeltaFQFit//{}", module->name()), name_, GenericItem::InRestartFileFlag);

        // Copy the original difference data and "invert" it
        deltaFQ = differenceData;
        deltaFQ *= -1.0;
        for (auto &&[x, y] : zip(deltaFQ.xAxis(), deltaFQ.values()))
            if (x < qMin_ || x > qMax_)
                y = 0.0;

        // Fit a function expansion to the deltaFQ - if the coefficient arrays already exist then re-fit starting from
        // those.
        auto [fitCoefficients, status] = moduleData.realiseIf<std::vector<double>>(
            std::format("FitCoefficients_{}", module->name()), name_, GenericItem::InRestartFileFlag);

        auto fitError = 0.0;
        auto nIterations = nPItSs_.value_or(0);
        if (expansionFunction_ == EPSRModule::GaussianExpansionFunction)
        {
            // Construct our fitting object
            GaussFit coeffMinimiser(deltaFQ);

            if (status == GenericItem::ItemStatus::Created)
                fitError = coeffMinimiser.constructReciprocal(0.0, rmaxpt, ncoeffp, gSigma1_, nIterations, 0.01,
                                                              fluctuationSmoothing_);
            else
            {
                if (fitCoefficients.size() != ncoeffp)
                {
                    Messenger::warn("Number of terms ({}) in existing FitCoefficients array for target '{}' does "
                                    "not match the current number ({}), so will fit from scratch.\n",
                                    fitCoefficients.size(), module->name(), ncoeffp);
                    fitError = coeffMinimiser.constructReciprocal(0.0, rmaxpt, ncoeffp, gSigma1_, nIterations, 0.01,
                                                                  fluctuationSmoothing_);
                }
                else
                    fitError = coeffMinimiser.constructReciprocal(0.0, rmaxpt, fitCoefficients, gSigma1_, nIterations, 0.01,
                                                                  fluctuationSmoothing_);
            }

            // Store the new fit coefficients
            fitCoefficients = coeffMinimiser.A();

            deltaFQFit = coeffMinimiser.approximation();
        }
        else if (expansionFunction_ == EPSRModule::PoissonExpansionFunction)
        {
            // Construct our fitting object
            PoissonFit coeffMinimiser(deltaFQ);

            if (status == GenericItem::ItemStatus::Created)
                fitError = coeffMinimiser.constructReciprocal(0.0, rmaxpt, ncoeffp, pSigma1_, pSigma2_, nIterations, 0.1,
                                                              fluctuationSmoothing_);
            else
            {
                if (fitCoefficients.size() != ncoeffp)
                {
                    Messenger::warn("Number of terms ({}) in existing FitCoefficients array for target '{}' does "
                                    "not match the current number ({}), so will fit from scratch.\n",
                                    fitCoefficients.size(), module->name(), ncoeffp);
                    fitError = coeffMinimiser.constructReciprocal(0.0, rmaxpt, ncoeffp, pSigma1_, pSigma2_, nIterations, 0.01,
                                                                  fluctuationSmoothing_);
                }
                else
                    fitError = coeffMinimiser.constructReciprocal(0.0, rmaxpt, fitCoefficients, pSigma1_, pSigma2_, nIterations,
                                                                  0.01, fluctuationSmoothing_);
            }

            // Store the new fit coefficients
            fitCoefficients = coeffMinimiser.C();

            deltaFQFit = coeffMinimiser.approximation();
        }
        Messenger::print("Error between delta F(Q) and fit function is {:.2f}%.\n", fitError);

        /*
         * Calculate F(r)
         */

        // Retrieve the storage object
        auto &simulatedFR =
            moduleData.realise<Data1D>(std::format("SimulatedFR//{}", module->name()), name_, GenericItem::InRestartFileFlag);

        // Copy the total calculated F(Q) and trim to the same range as the experimental data before FT
        simulatedFR = weightedSQ.total();
        Filters::trim(simulatedFR, originalReferenceData);
        Fourier::sineFT(simulatedFR, 1.0 / (2 * M_PI * M_PI * rho), 0.0, 0.03, 30.0,
                        WindowFunction(WindowFunction::Form::Lorch0));

        /*
         * Add the Data to the Scattering Matrix
         */

        // Set overall weighting factor for the dataset
        auto weightIt = std::find_if(targetWeights_.begin(), targetWeights_.end(),
                                     [module](const auto &targetWeight) { return targetWeight.first == module; });
        auto dataSetWeight =
            feedback_ * (weightIt == targetWeights_.end() ? 1.0 : weightIt->second) * (targets_.size() / totalDataSetWeight);

        if (module->type() == ModuleTypes::NeutronSQ)
        {
            const auto weights = dynamic_cast<NeutronSQModule *>(module)->weights();

            // Subtract intramolecular total from the reference data - this will enter into the ScatteringMatrix
            auto refMinusIntra = trimmedReferenceData;
            Interpolator::addInterpolated(weightedSQ.boundTotal(), refMinusIntra, -1.0);

            // Always add absolute data to the scattering matrix - if the calculated data has been normalised, remove this
            // normalisation from the reference data (we assume that the two are consistent)
            auto normType = module->keywords().getEnumeration<StructureFactors::NormalisationType>("NormaliseTo");
            if (normType == StructureFactors::AverageOfSquaresNormalisation)
                refMinusIntra *= weights.boundCoherentAverageOfSquares();
            else if (normType == StructureFactors::SquareOfAverageNormalisation)
                refMinusIntra *= weights.boundCoherentSquareOfAverage();

            // Set the zero limit on the data (equivalent to EPSR's szeros == 0.0)
            Data1D zeroed;
            zeroed.addPoint(0.0, refMinusIntra.values().front());
            for (auto &&[x, y] : zip(refMinusIntra.xAxis(), refMinusIntra.values()))
                zeroed.addPoint(x, y);

            scatteringMatrix_->setRow(std::format("Neutron//{}", module->name()), zeroed, weights, dataSetWeight);
        }
        else if (module->type() == ModuleTypes::XRaySQ)
        {
            const auto weights = dynamic_cast<XRaySQModule *>(module)->weights();

            // For X-ray data we always add the reference data normalised to AverageOfSquares in order to give consistency
            // in terms of magnitude with any neutron data. If the calculated data have not been normalised, or were
            // normalised to something else, we correct it before adding.
            auto refMinusIntra = trimmedReferenceData;
            Interpolator::addInterpolated(weightedSQ.boundTotal(), refMinusIntra, -1.0);

            auto normType = module->keywords().getEnumeration<StructureFactors::NormalisationType>("NormaliseTo");
            if (normType == StructureFactors::SquareOfAverageNormalisation)
            {
                // Remove square of average normalisation, and apply average of squares
                auto bSqOfAv = weights.boundCoherentSquareOfAverage(refMinusIntra.xAxis());
                auto bAvOfSq = weights.boundCoherentAverageOfSquares(refMinusIntra.xAxis());
                for (auto &&[val, bOld, bNew] : zip(refMinusIntra.values(), bSqOfAv, bAvOfSq))
                    val *= bOld / bNew;
            }
            else if (normType == StructureFactors::NoNormalisation)
            {
                auto bbar = weights.boundCoherentAverageOfSquares(refMinusIntra.xAxis());
                std::transform(refMinusIntra.values().begin(), refMinusIntra.values().end(), bbar.begin(),
                               refMinusIntra.values().begin(), std::divides<>());
            }

            // Set the zero limit on the data (equivalent to EPSR's szeros == 0.0)
            Data1D zeroed;
            zeroed.addPoint(0.0, refMinusIntra.values().front());
            for (auto &&[x, y] : zip(refMinusIntra.xAxis(), refMinusIntra.values()))
                zeroed.addPoint(x, y);

            scatteringMatrix_->setRow(std::format("XRay//{}", module->name()), zeroed, weights, dataSetWeight);
        }
        else
        {
            Messenger::error("Don't know how to add data from a module of type '{}' to the scattering matrix.",
                             ModuleTypes::moduleType(module->type()));
            return ExecutionResult::Failed;
        }

        /*
         * Sum Unweighted S(Q)
         */

        // Add the unweighted from this target to our combined, unweighted S(Q) data
        dissolve::for_each_pair(ParallelPolicies::seq, atomTypes,
                                [&](int i, const auto &at1, int j, const auto &at2)
                                {
                                    const auto &partialIJ = unweightedSQ.unboundPartials().get(at1->name(), at2->name());
                                    Interpolator::addInterpolated(partialIJ, calculatedUnweightedSQ[{at1->name(), at2->name()}],
                                                                  1.0 / targets_.size());
                                });

        /*
         * Save Data
         */

        if (saveDifferenceFunctions_)
        {
            Data1DExportFileFormat exportDiffFormat(std::format("{}-Diff.q", module->name()));
            if (!exportDiffFormat.exportData(differenceData))
                return ExecutionResult::Failed;

            Data1DExportFileFormat exportDeltaSQFormat(std::format("{}-DiffFit.q", module->name()));
            if (!exportDeltaSQFormat.exportData(deltaFQFit))
                return ExecutionResult::Failed;
        }
        if (saveSimulatedFR_)
        {
            Data1DExportFileFormat exportFormat(std::format("{}-SimulatedFR.r", module->name()));
            if (!exportFormat.exportData(simulatedFR))
                return ExecutionResult::Failed;
        }
    }

    // Finalise and store the total r-factor
    rFacTot /= targets_.size();
    auto &totalRFactor = moduleData.realise<Data1D>("RFactor", name_, GenericItem::InRestartFileFlag);
    totalRFactor.addPoint(dissolve.iteration(), rFacTot);
    Messenger::print("Current total R-Factor is {:.5f}.\n", rFacTot);
    for (auto &&[range, rangeTot] : zip(ranges_, rangedRFacTots))
    {
        Messenger::print("Current total R-Factor over range {:.5f} to {:.5f} is {:.5f}.\n", range.minimum(), range.maximum(),
                         rangeTot /= targets_.size());
    }

    Messenger::print("\n");

    /*
     * Augment the Scattering Matrix
     */

    // Add a contribution from each interatomic partial S(Q), weighted according to the feedback factor
    dissolve::for_each_pair(ParallelPolicies::seq, atomTypes,
                            [&](int i, auto &atI, int j, auto &atJ)
                            {
                                // Copy and rename the data for clarity
                                auto data = calculatedUnweightedSQ[{atI->name(), atJ->name()}];
                                data.setTag(std::format("Simulated {}-{}", atI->name(), atJ->name()));

                                scatteringMatrix_->setRow(std::format("{}-{}", atI->name(), atJ->name()), data, atI, atJ,
                                                          1.0 - feedback_);
                            });

    // Make sure inverse matrices are up-to-date, taking representative Q values from the calculated unweighted S(Q)
    scatteringMatrix_->generateMatrices(calculatedUnweightedSQ.begin()->second.xAxis());
    scatteringMatrix_->print();

    if (Messenger::isVerbose())
    {
        Messenger::print("\nScattering Matrix Inverse (Q = 0.0):\n");
        scatteringMatrix_->printInverse();

        Messenger::print("\nIdentity (Ainv * A):\n");
        scatteringMatrix_->matrixProduct().print();
    }

    /*
     * Generate S(Q) from completed scattering matrix
     */

    auto &estimatedSQ = dissolve.processingModuleData().realise<DoubleKeyedMap<Data1D>>("EstimatedSQ", name_);
    estimatedSQ = scatteringMatrix_->generateEstimatedPartials();
    updateDeltaSQ(moduleData, calculatedUnweightedSQ, estimatedSQ);

    // Save data?
    if (saveEstimatedPartials_)
    {
        for (auto &[key, data] : estimatedSQ)
        {
            Data1DExportFileFormat exportFormat(std::format("{}-EstSQ-{}.txt", name_, data.tag()));
            if (!exportFormat.exportData(data))
                return ExecutionResult::Failed;
        }
    }

    /*
     * Calculate estimated g(r) from estimated S(Q)
     */

    DoubleKeyedMap<Data1D> estimatedGR;
    dissolve::for_each_pair(ParallelPolicies::seq, atomTypes,
                            [&](int indexI, auto atI, int indexJ, auto atJ)
                            {
                                DoubleKeyedMapKey key(atI->name(), atJ->name());

                                // Set tag
                                estimatedGR.get(key).setTag(std::format("{}-{}", atI->name(), atJ->name()));

                                // Copy experimental S(Q) and FT it
                                estimatedGR[key] = estimatedSQ.get(key);
                                Fourier::sineFT(estimatedGR[key], 1.0 / (2 * M_PI * M_PI * rho), 0.0, 0.05, 30.0,
                                                WindowFunction(WindowFunction::Form::Lorch0));
                                estimatedGR[key] += 1.0;
                            });

    moduleData.realise<DoubleKeyedMap<Data1D>>("EstimatedGR", name_) = estimatedGR;

    /*
     * Calculate contribution to potential coefficients.
     * Multiply each coefficient by the associated weight in the inverse scattering matrix.
     * Note: the data were added to the scattering matrix in the order they appear in the targets iterator.
     */
    auto dataIndex = 0;
    for (auto *module : targets_)
    {
        // For this Module, retrieve the coefficients of the fit performed above.
        const auto &fitCoefficients =
            moduleData.value<std::vector<double>>(std::format("FitCoefficients_{}", module->name()), name_);

        // Loop over pair potentials and retrieve the inverse weight from the scattering matrix
        auto columnIndex = 0;
        dissolve::for_each_pair(ParallelPolicies::seq, atomTypes,
                                [&](int i, auto typeI, int j, auto typeJ)
                                {
                                    auto weight = scatteringMatrix_->qZeroMatrixInverse()[{columnIndex, dataIndex}];

                                    /*
                                     * EPSR assembles the potential coefficients from the deltaFQ fit coefficients as a linear
                                     * combination with the following weighting factors (see circa line 3378 in
                                     * epsr_standalone_rev1.f):
                                     *
                                     * 1. The overall potential factor (potfac) which is typically set to 1.0 in EPSR (or 0.0 to
                                     * disable potential generation)
                                     * 2. A flag controlling whether specific potentials are refined (efacp)
                                     * 3. The value of the inverse scattering matrix for this dataset / potential (cwtpot),
                                     * multiplied by the feedback factor.
                                     */

                                    // In the original EPSR the off-diagonal elements in the inverse matrix have also been
                                    // halved so as not to double-count the i != j terms
                                    if (i != j)
                                        weight *= 0.5;

                                    // Store fluctuation coefficients ready for addition to potential coefficients later on.
                                    auto [begin, end] = fluctuationCoefficients[std::tuple{i, j}];
                                    std::transform(fitCoefficients.begin(), fitCoefficients.end(), begin, begin,
                                                   [weight, this](auto coeff, auto result)
                                                   { return result + weight * feedback_ * coeff; });

                                    ++columnIndex;
                                });

        // Increase dataIndex
        ++dataIndex;
    }

    // Generate new empirical potentials
    auto energabs = 0.0;
    if (modifyPotential_ && (runCount % *modifyPotential_ == 0))
    {
        // Sum fluctuation coefficients in to the potential coefficients
        auto &coefficients = potentialCoefficients(moduleData, ncoeffp);
        dissolve::for_each_pair(ParallelPolicies::seq, atomTypes,
                                [&](int i, auto at1, int j, auto at2)
                                {
                                    auto &potCoeff = coefficients[{i, j}];

                                    // Zero potential before adding in fluctuation coefficients?
                                    if (overwritePotentials_)
                                        std::fill(potCoeff.begin(), potCoeff.end(), 0.0);

                                    // Add in fluctuation coefficients
                                    auto [begin, end] = fluctuationCoefficients[std::tuple{i, j}];
                                    std::transform(potCoeff.begin(), potCoeff.end(), begin, potCoeff.begin(),
                                                   [this](auto pot, auto fluct) { return pot + weighting_ * fluct; });

                                    // Set first term to zero (following EPSR)
                                    potCoeff[0] = 0.0;
                                });

        // Determine absolute energy of empirical potentials
        energabs = absEnergyEP(moduleData, atomTypes);

        /*
         * Determine the scaling we will apply to the coefficients (if any)
         * Notes:
         * 	- ereq is actually read in as a variable called 'pressreq', then assigned to another variable
         * 'absolute_energy' in the EPSR25 source.
         * 	- erequnit appears to be set to the value of ereqstep read in from the 'ereqstep' command (clamped to
         * 0.0-1.0).
         */
        auto pressfac = 1.0;
        auto erequnit = 0.0, ereqstep = 0.0;

        if (fabs(eReq_) == 0.0)
        {
            pressfac = 0.0;
            energabs = 0.0;
        }
        else if (fabs(energabs) > 0.0)
        {
            if (fabs(energabs) < fabs(erequnit))
                pressfac = fabs(erequnit) / fabs(energabs);
            else
            {
                pressfac = fabs(eReq_) / fabs(energabs);
                if ((pressfac > 1.0) && (ereqstep == 0.0))
                    pressfac = 1.0;
            }
        }
        Messenger::print("  generate_ep>  {}  {}  {}\n", eReq_, energabs, pressfac);

        // Scale coefficients
        for (auto &n : coefficients)
            std::transform(n.begin(), n.end(), n.begin(), [pressfac](auto value) { return value * pressfac; });
        energabs *= pressfac;

        // Generate additional potentials from the coefficients
        auto sigma1 = expansionFunction_ == EPSRModule::PoissonExpansionFunction ? pSigma1_ : gSigma1_;
        auto sigma2 = expansionFunction_ == EPSRModule::PoissonExpansionFunction ? pSigma2_ : gSigma2_;

        if (!generateEmpiricalPotentials(dissolve, atomTypes, rho, ncoeffp, rminpt, rmaxpt, sigma1, sigma2))
            return ExecutionResult::Failed;
    }
    else
        energabs = absEnergyEP(moduleData, atomTypes);

    // Save data?
    if (saveEmpiricalPotentials_)
    {
        if (!for_each_pair_early(atomTypes,
                                 [&](int i, auto at1, int j, auto at2) -> EarlyReturn<bool>
                                 {
                                     // Grab pointer to the relevant pair potential
                                     PairPotential *pp = dissolve.pairPotential(at1, at2);

                                     Data1DExportFileFormat exportFormat(
                                         std::format("{}-EP-{}-{}.txt", name_, at1->name(), at2->name()));
                                     if (!exportFormat.exportData(pp->additionalPotential()))
                                         return false;
                                     return EarlyReturn<bool>::Continue;
                                 })
                 .value_or(true))
            return ExecutionResult::Failed;
    }
    if (savePotentialCoefficients_)
    {
        auto &coefficients = potentialCoefficients(moduleData, ncoeffp);

        if (!for_each_pair_early(atomTypes,
                                 [&](int i, auto at1, int j, auto at2) -> EarlyReturn<bool>
                                 {
                                     // Grab reference to coefficients
                                     auto &potCoeff = coefficients[{i, j}];

                                     LineParser fileParser;
                                     if (!fileParser.openOutput(
                                             std::format("{}-PCof-{}-{}.txt", name_, at1->name(), at2->name())))
                                         return false;
                                     for (auto n : potCoeff)
                                         if (!fileParser.writeLineF("{}\n", n))
                                             return false;
                                     fileParser.closeFiles();
                                     return EarlyReturn<bool>::Continue;
                                 })
                 .value_or(true))
            return ExecutionResult::Failed;
    }

    // Realise the phiMag array and make sure its object name is set
    auto &phiArray = moduleData.realise<Data1D>("EPMag", name_, GenericItem::InRestartFileFlag);
    phiArray.addPoint(dissolve.iteration(), energabs);

    return ExecutionResult::Success;
}
