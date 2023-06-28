// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/neutronweights.h"
#include "classes/partialset.h"
#include "classes/scatteringmatrix.h"
#include "classes/xrayweights.h"
#include "data/isotopes.h"
#include "io/export/data1d.h"
#include "keywords/module.h"
#include "main/dissolve.h"
#include "math/error.h"
#include "math/filters.h"
#include "math/ft.h"
#include "math/gaussfit.h"
#include "math/poissonfit.h"
#include "module/group.h"
#include "modules/energy/energy.h"
#include "modules/epsr/epsr.h"
#include "modules/gr/gr.h"
#include "modules/neutronsq/neutronsq.h"
#include "modules/sq/sq.h"
#include "modules/xraysq/xraysq.h"
#include "templates/algorithms.h"
#include "templates/array3d.h"
#include <functional>

// Run set-up stage
bool EPSRModule::setUp(Dissolve &dissolve, const ProcessPool &procPool, Flags<KeywordBase::KeywordSignal> actionSignals)
{
    // Realise storage for generated S(Q), and initialise a scattering matrix
    auto &estimatedSQ =
        dissolve.processingModuleData().realise<Array2D<Data1D>>("EstimatedSQ", name_, GenericItem::InRestartFileFlag);
    scatteringMatrix_.initialise(dissolve.atomTypes(), estimatedSQ);

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
        if (!readPCof(dissolve, procPool, pCofFilename_))
            return Messenger::error("[SETUP {}] Failed to read in potential coefficients from EPSR pcof file.\n", name_);

        // Set up the additional potentials - reconstruct them from the current coefficients
        auto rmaxpt = rMaxPT_ ? rMaxPT_.value() : dissolve.pairPotentialRange();
        auto rminpt = rMinPT_ ? rMinPT_.value() : rmaxpt - 2.0;
        if (expansionFunction_ == EPSRModule::GaussianExpansionFunction)
        {
            if (!generateEmpiricalPotentials(dissolve, expansionFunction_, rho.value_or(0.1), nCoeffP_, rminpt, rmaxpt,
                                             gSigma1_, gSigma2_))
                return false;
        }
        else
        {
            if (!generateEmpiricalPotentials(dissolve, expansionFunction_, rho.value_or(0.1), nCoeffP_, rminpt, rmaxpt,
                                             pSigma1_, pSigma2_))
                return false;
        }
    }

    // If an inpa file was provided, read in the parameters from it here
    if (!inpaFilename_.empty())
    {
        Messenger::print("[SETUP {}] Reading fit coefficients from '{}'...\n", name_, inpaFilename_);

        // Read in the coefficients / setup from the supplied file
        if (!readFitCoefficients(dissolve, procPool, inpaFilename_))
            return Messenger::error("[SETUP {}] Failed to read in fit coefficients from EPSR inpa file.\n", name_);
    }

    // Try to calculate the deltaSQ array
    updateDeltaSQ(dissolve.processingModuleData());

    return true;
}

// Run main processing
bool EPSRModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    std::string testDataName;

    // EPSR constants
    const auto mcoeff = 200;

    // Calculate some values if they were not provided
    auto rmaxpt = rMaxPT_ ? rMaxPT_.value() : dissolve.pairPotentialRange();
    auto rminpt = rMinPT_ ? rMinPT_.value() : rmaxpt - 2.0;
    auto ncoeffp = nCoeffP_ ? nCoeffP_.value() : std::min(int(10.0 * rmaxpt + 0.0001), mcoeff);

    // Print option summary
    Messenger::print("EPSR: Feedback factor is {}.\n", feedback_);
    Messenger::print("EPSR: {} functions will be used to approximate difference data.\n",
                     expansionFunctionTypes().keyword(expansionFunction_));
    Messenger::print("EPSR: Number of functions used in approximation is {}, sigma(Q) = {}.\n", ncoeffp, pSigma2_);
    if (modifyPotential_)
        Messenger::print(
            "EPSR: Perturbations to interatomic potentials will be generated and applied with a frequency of {}.\n",
            *modifyPotential_);
    else
        Messenger::print("EPSR: Perturbations to interatomic potentials will be generated only (current potentials "
                         "will not be modified).\n");
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
    if (test_)
        Messenger::print("EPSR: Test mode is enabled (threshold = {}%).", testThreshold_);
    Messenger::print("\n");

    /*
     * Do we have targets to refine against?
     */
    if (targets_.empty())
        return Messenger::error("At least one Module target containing suitable data must be provided.\n");

    if (!targetConfiguration_)
        return Messenger::error("No target configuration is set.\n");

    if (!targetConfiguration_->atomicDensity())
        return Messenger::error("No density available for target configuration '{}'\n", targetConfiguration_->name());
    auto rho = *targetConfiguration_->atomicDensity();

    /*
     * Realise and increase run counter
     */
    auto [runCount, runCountStatus] =
        dissolve.processingModuleData().realiseIf<int>("RunCount", name(), GenericItem::InRestartFileFlag);
    if (runCountStatus == GenericItem::ItemStatus::Created)
        runCount = 0;
    ++runCount;

    /*
     * EPSR Main
     */

    // Set up storage for the changes to coefficients used to generate the empirical potentials
    const auto nAtomTypes = dissolve.nAtomTypes();
    Array3D<double> fluctuationCoefficients(nAtomTypes, nAtomTypes, ncoeffp);
    fluctuationCoefficients = 0.0;

    // Create storage for our summed UnweightedSQ
    auto &calculatedUnweightedSQ =
        dissolve.processingModuleData().realise<Array2D<Data1D>>("UnweightedSQ", name_, GenericItem::InRestartFileFlag);
    calculatedUnweightedSQ.initialise(nAtomTypes, nAtomTypes, true);
    dissolve::for_each_pair(ParallelPolicies::par, dissolve.atomTypes().begin(), dissolve.atomTypes().end(),
                            [&](int i, auto at1, int j, auto at2) {
                                calculatedUnweightedSQ[{i, j}].setTag(fmt::format("{}-{}", at1->name(), at2->name()));
                            });

    // Is our scattering matrix fully set-up and just requiring updated data?
    auto scatteringMatrixSetUp = scatteringMatrix_.nReferenceData() != 0;

    // Loop over target data
    auto rFacTot = 0.0;
    for (auto *module : targets_)
    {
        /*
         * Retrieve data for this module
         */

        // Retrieve the weighted S(Q)/F(Q)
        if (!dissolve.processingModuleData().contains("WeightedSQ", module->name()))
            return Messenger::error("Weighted partials data not found for target '{}'.\n", module->name());
        const auto &weightedSQ = dissolve.processingModuleData().value<PartialSet>("WeightedSQ", module->name());

        // Get source SQModule in order to have access to the unweighted S(Q)
        auto optSQModule = module->keywords().get<const SQModule *, ModuleKeyword<const SQModule>>("SourceSQs");
        const SQModule *sqModule = nullptr;
        if (optSQModule)
            sqModule = optSQModule.value();
        if (!sqModule)
            return Messenger::error(
                "Module '{}' doesn't source any S(Q) data, so it can't be used to augment the scattering matrix.",
                module->name());

        // Retrieve the unweighted S(Q)/F(Q)
        if (!dissolve.processingModuleData().contains("UnweightedSQ", sqModule->name()))
            return Messenger::error("Unweighted partials data not found for target '{}'.\n", sqModule->name());
        const auto &unweightedSQ = dissolve.processingModuleData().value<PartialSet>("UnweightedSQ", sqModule->name());

        // Retrieve the ReferenceData
        if (!dissolve.processingModuleData().contains("ReferenceData", module->name()))
            return Messenger::error("Reference data not found for target '{}'.\n", module->name());
        const auto &originalReferenceData = dissolve.processingModuleData().value<Data1D>("ReferenceData", module->name());

        // Realise the r-factor array and make sure its object name is set
        auto &errors = dissolve.processingModuleData().realise<Data1D>(fmt::format("RFactor//{}", module->name()), name_,
                                                                       GenericItem::InRestartFileFlag);

        /*
         * Calculate difference functions and current percentage errors in calculated vs reference target data.
         * Do this over the widest Q-range allowed by both datasets
         */

        // Get difference data container and form the difference between the reference and calculated data
        auto &differenceData = dissolve.processingModuleData().realise<Data1D>(fmt::format("Difference//{}", module->name()),
                                                                               name(), GenericItem::InRestartFileFlag);
        differenceData = weightedSQ.total();
        differenceData *= -1.0;
        Interpolator::addInterpolated(originalReferenceData, differenceData);

        // Calculate r-factor over fit range and store
        auto tempRefData = originalReferenceData;
        Filters::trim(tempRefData, qMin_, qMax_);
        auto rFactor = Error::rFactor(tempRefData, weightedSQ.total(), true);
        rFacTot += rFactor;
        errors.addPoint(dissolve.iteration(), rFactor);
        Messenger::print("Current R-Factor for reference data '{}' is {:.5f}.\n", module->name(), rFactor);

        /*
         * Generate difference function for fitting, spanning (maximally) only the range requested
         */

        // Get difference and fit function objects
        auto &deltaFQ = dissolve.processingModuleData().realise<Data1D>(fmt::format("DeltaFQ//{}", module->name()), name_,
                                                                        GenericItem::InRestartFileFlag);
        auto &deltaFQFit = dissolve.processingModuleData().realise<Data1D>(fmt::format("DeltaFQFit//{}", module->name()), name_,
                                                                           GenericItem::InRestartFileFlag);

        // Copy the original difference data and "invert" it
        deltaFQ = differenceData;
        deltaFQ *= -1.0;
        for (auto &&[x, y] : zip(deltaFQ.xAxis(), deltaFQ.values()))
            if (x < qMin_ || x > qMax_)
                y = 0.0;

        // Fit a function expansion to the deltaFQ - if the coefficient arrays already exist then re-fit starting from those.
        auto [fitCoefficients, status] = dissolve.processingModuleData().realiseIf<std::vector<double>>(
            fmt::format("FitCoefficients_{}", module->name()), name_, GenericItem::InRestartFileFlag);

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
        auto &simulatedFR = dissolve.processingModuleData().realise<Data1D>(fmt::format("SimulatedFR//{}", module->name()),
                                                                            name_, GenericItem::InRestartFileFlag);

        // Copy the total calculated F(Q) and trim to the same range as the experimental data before FT
        simulatedFR = weightedSQ.total();
        Filters::trim(simulatedFR, originalReferenceData);
        Fourier::sineFT(simulatedFR, 1.0 / (2 * PI * PI * rho), 0.0, 0.03, 30.0, WindowFunction(WindowFunction::Form::Lorch0));

        /*
         * Add the Data to the Scattering Matrix
         */

        if (module->type() == ModuleTypes::NeutronSQ)
        {
            const auto &weights = dissolve.processingModuleData().value<NeutronWeights>("FullWeights", module->name());

            // Subtract intramolecular total from the reference data - this will enter into the ScatteringMatrix
            auto refMinusIntra = originalReferenceData;
            Interpolator::addInterpolated(weightedSQ.boundTotal(), refMinusIntra, -1.0);

            // Always add absolute data to the scattering matrix - if the calculated data has been normalised, remove this
            // normalisation from the reference data (we assume that the two are consistent)
            auto normType = module->keywords().getEnumeration<StructureFactors::NormalisationType>("NormaliseTo");
            if (normType == StructureFactors::AverageOfSquaresNormalisation)
                refMinusIntra *= weights.boundCoherentAverageOfSquares();
            else if (normType == StructureFactors::SquareOfAverageNormalisation)
                refMinusIntra *= weights.boundCoherentSquareOfAverage();

            if (scatteringMatrixSetUp ? !scatteringMatrix_.updateReferenceData(refMinusIntra, feedback_)
                                      : !scatteringMatrix_.addReferenceData(refMinusIntra, weights, feedback_))
                return Messenger::error("Failed to add target data '{}' to weights matrix.\n", module->name());
        }
        else if (module->type() == ModuleTypes::XRaySQ)
        {
            auto &weights = dissolve.processingModuleData().retrieve<XRayWeights>("FullWeights", module->name());

            // For X-ray data we always add the reference data normalised to AverageOfSquares in order to give consistency in
            // terms of magnitude with any neutron data. If the calculated data have not been normalised, or were normalised to
            // something else, we correct it before adding.
            auto refMinusIntra = originalReferenceData;
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

            if (scatteringMatrixSetUp ? !scatteringMatrix_.updateReferenceData(refMinusIntra, feedback_)
                                      : !scatteringMatrix_.addReferenceData(refMinusIntra, weights, feedback_))
                return Messenger::error("Failed to add target data '{}' to weights matrix.\n", module->name());
        }
        else
            return Messenger::error("Don't know how to add data from a module of type '{}' to the scattering matrix.",
                                    ModuleTypes::moduleType(module->type()));

        /*
         * Sum Unweighted S(Q)
         */

        // Add the unweighted from this target to our combined, unweighted S(Q) data
        auto &types = unweightedSQ.atomTypeMix();
        dissolve::for_each_pair(
            ParallelPolicies::seq, types.begin(), types.end(),
            [&](int i, const AtomTypeData &atd1, int j, const AtomTypeData &atd2)
            {
                auto globalI = atd1.atomType()->index();
                auto globalJ = atd2.atomType()->index();

                const auto &partialIJ = unweightedSQ.unboundPartial(i, j);
                Interpolator::addInterpolated(partialIJ, calculatedUnweightedSQ[{globalI, globalJ}], 1.0 / targets_.size());
            });

        /*
         * Save Data
         */

        if (saveDifferenceFunctions_)
        {
            if (procPool.isMaster())
            {
                Data1DExportFileFormat exportFormat(fmt::format("{}-Diff.q", module->name()));
                if (exportFormat.exportData(differenceData))
                    procPool.decideTrue();
                else
                    return procPool.decideFalse();
            }
            else if (!procPool.decision())
                return true;

            if (procPool.isMaster())
            {
                Data1DExportFileFormat exportFormat(fmt::format("{}-DiffFit.q", module->name()));
                if (exportFormat.exportData(deltaFQFit))
                    procPool.decideTrue();
                else
                    return procPool.decideFalse();
            }
            else if (!procPool.decision())
                return true;
        }
        if (saveSimulatedFR_)
        {
            if (procPool.isMaster())
            {
                Data1DExportFileFormat exportFormat(fmt::format("{}-SimulatedFR.r", module->name()));
                if (exportFormat.exportData(simulatedFR))
                    procPool.decideTrue();
                else
                    return procPool.decideFalse();
            }
            else if (!procPool.decision())
                return true;
        }

        /*
         * Test Mode
         */

        if (test_)
        {
            testDataName = fmt::format("WeightedFR-{}-total", module->name());
            if (testReferenceData_.containsData(testDataName))
            {
                auto optRefData = testReferenceData_.data(testDataName);
                if (!optRefData)
                    return Messenger::error("Reference data '{}' not found.\n", testDataName);
                auto error = Error::percent(simulatedFR, *optRefData);
                Messenger::print("Simulated F(r) reference data '{}' has error of {:7.3f}% with calculated data "
                                 "and is {} (threshold is {:6.3f}%)\n\n",
                                 testDataName, error, error <= testThreshold_ ? "OK" : "NOT OK", testThreshold_);
                if (error > testThreshold_)
                    return false;
            }
        }
    }

    // Finalise and store the total r-factor
    rFacTot /= targets_.size();
    auto &totalRFactor = dissolve.processingModuleData().realise<Data1D>("RFactor", name_, GenericItem::InRestartFileFlag);
    totalRFactor.addPoint(dissolve.iteration(), rFacTot);
    Messenger::print("Current total R-Factor is {:.5f}.\n", rFacTot);

    /*
     * Augment the Scattering Matrix
     */

    // Add a contribution from each interatomic partial S(Q), weighted according to the feedback factor
    for_each_pair_early(dissolve.atomTypes().begin(), dissolve.atomTypes().end(),
                        [&](int i, auto at1, int j, auto at2) -> EarlyReturn<bool>
                        {
                            // Copy and rename the data for clarity
                            auto data = calculatedUnweightedSQ[{i, j}];
                            data.setTag(fmt::format("Simulated {}-{}", at1->name(), at2->name()));

                            // Add this partial data to the scattering matrix - its factored weight will be (1.0 - feedback)
                            if (scatteringMatrixSetUp
                                    ? !scatteringMatrix_.updateReferenceData(data, 1.0 - feedback_)
                                    : !scatteringMatrix_.addPartialReferenceData(data, at1, at2, 1.0, (1.0 - feedback_)))
                                return Messenger::error("EPSR: Failed to augment scattering matrix with partial {}-{}.\n",
                                                        at1->name(), at2->name());
                            return EarlyReturn<bool>::Continue;
                        });

    // If the scattering matrix was not set-up, need to generate the necessary inverse matrix or matrices here
    if (!scatteringMatrixSetUp)
        scatteringMatrix_.generateMatrices();

    scatteringMatrix_.print();

    if (Messenger::isVerbose())
    {
        Messenger::print("\nScattering Matrix Inverse (Q = 0.0):\n");
        scatteringMatrix_.printInverse();

        Messenger::print("\nIdentity (Ainv * A):\n");
        scatteringMatrix_.matrixProduct().print();
    }

    /*
     * Generate S(Q) from completed scattering matrix
     */

    auto &estimatedSQ =
        dissolve.processingModuleData().realise<Array2D<Data1D>>("EstimatedSQ", name_, GenericItem::InRestartFileFlag);
    scatteringMatrix_.generatePartials(estimatedSQ);
    updateDeltaSQ(dissolve.processingModuleData(), calculatedUnweightedSQ, estimatedSQ);

    // Save data?
    if (saveEstimatedPartials_)
    {
        if (procPool.isMaster())
        {
            for (auto &sq : estimatedSQ)
            {
                Data1DExportFileFormat exportFormat(fmt::format("{}-EstSQ-{}.txt", name_, sq.tag()));
                if (!exportFormat.exportData(sq))
                    return procPool.decideFalse();
            }
            procPool.decideTrue();
        }
        else if (!procPool.decision())
            return true;
    }

    // Test Mode
    if (test_)
    {
        for_each_pair_early(dissolve.atomTypes().begin(), dissolve.atomTypes().end(),
                            [&](int i, auto at1, int j, auto at2) -> EarlyReturn<bool>
                            {
                                testDataName = fmt::format("EstimatedSQ-{}-{}", at1->name(), at2->name());
                                auto optRefData = testReferenceData_.data(testDataName);
                                if (optRefData)
                                {
                                    auto error = Error::percent(estimatedSQ[{i, j}], *optRefData);
                                    Messenger::print("Generated S(Q) reference data '{}' has error of {:7.3f}% with "
                                                     "calculated data and is {} (threshold is {:6.3f}%)\n\n",
                                                     testDataName, error, error <= testThreshold_ ? "OK" : "NOT OK",
                                                     testThreshold_);
                                    if (error > testThreshold_)
                                        return false;
                                }
                                return EarlyReturn<bool>::Continue;
                            });
    }

    /*
     * Calculate g(r) from estimatedSQ
     */

    auto &estimatedGR =
        dissolve.processingModuleData().realise<Array2D<Data1D>>("EstimatedGR", name_, GenericItem::InRestartFileFlag);
    estimatedGR.initialise(dissolve.nAtomTypes(), dissolve.nAtomTypes(), true);
    dissolve::for_each_pair(ParallelPolicies::seq, dissolve.atomTypes().begin(), dissolve.atomTypes().end(),
                            [&](int i, auto at1, int j, auto at2)
                            {
                                auto &expGR = estimatedGR[{i, j}];
                                expGR.setTag(fmt::format("{}-{}", at1->name(), at2->name()));

                                // Copy experimental S(Q) and FT it
                                expGR = estimatedSQ[{i, j}];
                                Fourier::sineFT(expGR, 1.0 / (2 * PI * PI * rho), 0.0, 0.05, 30.0,
                                                WindowFunction(WindowFunction::Form::Lorch0));
                                expGR += 1.0;
                            });

    /*
     * Calculate contribution to potential coefficients.
     * Multiply each coefficient by the associated weight in the inverse scattering matrix.
     * Note: the data were added to the scattering matrix in the order they appear in the targets iterator.
     */
    auto dataIndex = 0;
    for (auto *module : targets_)
    {
        // For this Module, retrieve the coefficients of the fit performed above.
        const auto &fitCoefficients = dissolve.processingModuleData().value<std::vector<double>>(
            fmt::format("FitCoefficients_{}", module->name()), name_);

        // Loop over pair potentials and retrieve the inverse weight from the scattering matrix
        dissolve::for_each_pair(
            ParallelPolicies::seq, dissolve.atomTypes().begin(), dissolve.atomTypes().end(),
            [&](int i, auto at1, int j, auto at2)
            {
                auto weight = scatteringMatrix_.qZeroMatrixInverse()[{scatteringMatrix_.pairIndex(at1, at2), dataIndex}];

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
                               [weight, this](auto coeff, auto result) { return result + weight * feedback_ * coeff; });
            });

        // Increase dataIndex
        ++dataIndex;
    }

    // Generate new empirical potentials
    auto energabs = 0.0;
    if (modifyPotential_ && (runCount % *modifyPotential_ == 0))
    {
        // Sum fluctuation coefficients in to the potential coefficients
        auto &coefficients = potentialCoefficients(dissolve, nAtomTypes, ncoeffp);
        dissolve::for_each_pair(ParallelPolicies::seq, dissolve.atomTypes().begin(), dissolve.atomTypes().end(),
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
        energabs = absEnergyEP(dissolve);

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

        if (!generateEmpiricalPotentials(dissolve, expansionFunction_, rho, ncoeffp, rminpt, rmaxpt, sigma1, sigma2))
            return false;
    }
    else
        energabs = absEnergyEP(dissolve);

    // Test absolute EP energy?
    if (!testAbsEnergyEP_.empty() && !testAbsEnergyEP(dissolve))
        return false;

    // Save data?
    if (saveEmpiricalPotentials_)
    {
        if (procPool.isMaster())
        {
            dissolve::for_each_pair(ParallelPolicies::seq, dissolve.atomTypes().begin(), dissolve.atomTypes().end(),
                                    [&](int i, auto at1, int j, auto at2) -> std::optional<bool>
                                    {
                                        // Grab pointer to the relevant pair potential
                                        PairPotential *pp = dissolve.pairPotential(at1, at2);

                                        Data1DExportFileFormat exportFormat(
                                            fmt::format("{}-EP-{}-{}.txt", name_, at1->name(), at2->name()));
                                        if (!exportFormat.exportData(pp->uAdditional()))
                                            return procPool.decideFalse();
                                        return std::nullopt;
                                    });
            procPool.decideTrue();
        }
        else if (!procPool.decision())
            return false;
    }
    if (savePotentialCoefficients_)
    {
        if (procPool.isMaster())
        {
            auto &coefficients = potentialCoefficients(dissolve, nAtomTypes, ncoeffp);

            dissolve::for_each_pair(
                ParallelPolicies::seq, dissolve.atomTypes().begin(), dissolve.atomTypes().end(),
                [&](int i, auto at1, int j, auto at2) -> std::optional<bool>
                {
                    // Grab reference to coefficients
                    auto &potCoeff = coefficients[{i, j}];

                    LineParser fileParser;
                    if (!fileParser.openOutput(fmt::format("{}-PCof-{}-{}.txt", name_, at1->name(), at2->name())))
                        return procPool.decideFalse();
                    for (auto n : potCoeff)
                        if (!fileParser.writeLineF("{}\n", n))
                            return procPool.decideFalse();
                    fileParser.closeFiles();
                    return std::nullopt;
                });

            procPool.decideTrue();
        }
        else if (!procPool.decision())
            return false;
    }

    // Realise the phiMag array and make sure its object name is set
    auto &phiArray = dissolve.processingModuleData().realise<Data1D>("EPMag", name_, GenericItem::InRestartFileFlag);
    phiArray.addPoint(dissolve.iteration(), energabs);

    return true;
}