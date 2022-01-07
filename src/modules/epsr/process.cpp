// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

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
#include "modules/neutronsq/neutronsq.h"
#include "modules/rdf/rdf.h"
#include "modules/sq/sq.h"
#include "modules/xraysq/xraysq.h"
#include "templates/algorithms.h"
#include "templates/array3d.h"
#include <functional>

// Run set-up stage
bool EPSRModule::setUp(Dissolve &dissolve, ProcessPool &procPool, KeywordSignals actionSignals)
{
    // Check for exactly one Configuration referenced through target modules
    targetConfiguration_ = nullptr;
    auto rho = 0.0;
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
                uniqueName_, module->uniqueName());

        auto *rdfModule = sqModule->sourceRDF();
        if (!rdfModule)
            return Messenger::error(
                "[SETUP {}] Target '{}'s S(Q) module doesn't reference an RDFModule, it can't be used as a target "
                "for the EPSR module.",
                uniqueName_, module->uniqueName());

        // Check for number of targets, or different target if there's only 1
        auto rdfConfigs = rdfModule->keywords().get<std::vector<Configuration *>>("Configuration");
        if (rdfConfigs.size() != 1)
            return Messenger::error(
                "[SETUP {}] RDF module '{}' targets multiple configurations, which is not permitted when using "
                "its data in the EPSR module.",
                uniqueName_, rdfModule->uniqueName());

        if ((targetConfiguration_ != nullptr) && (targetConfiguration_ != rdfConfigs.front()))
            return Messenger::error("[SETUP {}] RDF module '{}' targets a configuration which is different from another target "
                                    "module, and which is not permitted when using its data in the EPSR module.",
                                    uniqueName_, rdfModule->uniqueName());
        else
            targetConfiguration_ = rdfConfigs.front();

        rho = targetConfiguration_->atomicDensity();
    }

    // If a pcof file was provided, read in the parameters from it here
    if (!pCofFilename_.empty())
    {
        Messenger::print("[SETUP {}] Reading potential coefficients from '{}'...\n", uniqueName_, pCofFilename_);

        // Read in the coefficients / setup from the supplied file
        if (!readPCof(dissolve, procPool, pCofFilename_))
            return Messenger::error("[SETUP {}] Failed to read in potential coefficients from EPSR pcof file.\n");

        // Set up the additional potentials - reconstruct them from the current coefficients
        if (expansionFunction_ == EPSRModule::GaussianExpansionFunction)
        {
            if (!generateEmpiricalPotentials(dissolve, expansionFunction_, rho, nCoeffP_, rMinPT_, rMaxPT_, gSigma1_, gSigma2_))
                return false;
        }
        else
        {
            if (!generateEmpiricalPotentials(dissolve, expansionFunction_, rho, nCoeffP_, rMinPT_, rMaxPT_, pSigma1_, pSigma2_))
                return false;
        }
    }

    // Try to calculate the deltaSQ array
    updateDeltaSQ(dissolve.processingModuleData());

    return true;
}

// Run main processing
bool EPSRModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    std::string testDataName;

    // EPSR constants
    const auto mcoeff = 200;

    // Calculate some values if they were not provided
    if (rMaxPT_ < 0.0)
        rMaxPT_ = dissolve.pairPotentialRange();
    if (rMinPT_ < 0.0)
        rMinPT_ = rMaxPT_ - 2.0;
    if (nCoeffP_ <= 0)
        nCoeffP_ = std::min(int(10.0 * rMaxPT_ + 0.0001), mcoeff);

    // Print option summary
    Messenger::print("EPSR: Feedback factor is {}.\n", feedback_);
    Messenger::print("EPSR: {} functions will be used to approximate difference data.\n",
                     expansionFunctionTypes().keyword(expansionFunction_));
    Messenger::print("EPSR: Number of functions used in approximation is {}, sigma(Q) = {}.\n", nCoeffP_, pSigma2_);
    if (modifyPotential_)
        Messenger::print("EPSR: Perturbations to interatomic potentials will be generated and applied.\n");
    else
        Messenger::print("EPSR: Perturbations to interatomic potentials will be generated only (current potentials "
                         "will not be modified).\n");
    if (onlyWhenEnergyStable_)
        Messenger::print("EPSR: Potential refinement will only be performed if all related Configuration energies are "
                         "stable.\n");
    Messenger::print("EPSR: Range for potential generation is {} < Q < {} Angstroms**-1.\n", qMin_, qMax_);
    Messenger::print("EPSR: Weighting factor used when applying fluctuation coefficients is {}\n", weighting_);
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

    // Is the ctarget configuration's energy stable?
    if (onlyWhenEnergyStable_)
    {
        auto stabilityResult = EnergyModule::checkStability(dissolve.processingModuleData(), targetConfiguration_);
        if (stabilityResult == EnergyModule::NotAssessable)
            return false;
        else if (stabilityResult > 0)
        {
            Messenger::print("Configuration energy is not yet stable. No potential refinement will be "
                             "performed this iteration.\n");
            return true;
        }
    }

    /*
     * EPSR Main
     */

    // Set up storage for the changes to coefficients used to generate the empirical potentials
    const auto nAtomTypes = dissolve.nAtomTypes();
    Array3D<double> fluctuationCoefficients(nAtomTypes, nAtomTypes, nCoeffP_);
    fluctuationCoefficients = 0.0;

    // Create storage for our summed UnweightedSQ
    auto &calculatedUnweightedSQ =
        dissolve.processingModuleData().realise<Array2D<Data1D>>("UnweightedSQ", uniqueName_, GenericItem::InRestartFileFlag);
    calculatedUnweightedSQ.initialise(nAtomTypes, nAtomTypes, true);
    dissolve::for_each_pair(ParallelPolicies::par, dissolve.atomTypes().begin(), dissolve.atomTypes().end(),
                            [&](int i, auto at1, int j, auto at2) {
                                calculatedUnweightedSQ[{i, j}].setTag(fmt::format("{}-{}", at1->name(), at2->name()));
                            });

    // Realise storage for generated S(Q), and initialise a scattering matrix
    auto &estimatedSQ =
        dissolve.processingModuleData().realise<Array2D<Data1D>>("EstimatedSQ", uniqueName_, GenericItem::InRestartFileFlag);
    ScatteringMatrix scatteringMatrix;
    scatteringMatrix.initialise(dissolve.atomTypes(), estimatedSQ);

    // Loop over target data
    auto rFacTot = 0.0;
    for (auto *module : targets_)
    {
        /*
         * Retrieve data for this module
         */

        // Retrieve the weighted S(Q)/F(Q)
        const auto &weightedSQ = dissolve.processingModuleData().value<PartialSet>("WeightedSQ", module->uniqueName());

        // Get source SQModule in order to have access to the unweighted S(Q)
        auto optSQModule = module->keywords().get<const SQModule *, ModuleKeyword<const SQModule>>("SourceSQs");
        const SQModule *sqModule = nullptr;
        if (optSQModule)
            sqModule = optSQModule.value();
        if (!sqModule)
            return Messenger::error(
                "Module '{}' doesn't source any S(Q) data, so it can't be used to augment the scattering matrix.",
                module->uniqueName());

        // Retrieve the unweighted S(Q)/F(Q)
        const auto &unweightedSQ = dissolve.processingModuleData().value<PartialSet>("UnweightedSQ", sqModule->uniqueName());

        // Retrieve the ReferenceData
        const auto &originalReferenceData =
            dissolve.processingModuleData().value<Data1D>("ReferenceData", module->uniqueName());

        // Realise the r-factor array and make sure its object name is set
        auto &errors = dissolve.processingModuleData().realise<Data1D>(fmt::format("RFactor//{}", module->uniqueName()),
                                                                       uniqueName_, GenericItem::InRestartFileFlag);

        /*
         * Calculate difference functions and current percentage errors in calculated vs reference target data.
         * Do this over the widest Q-range allowed by both datasets
         */

        // Get difference data container and form the difference between the reference and calculated data
        auto &differenceData = dissolve.processingModuleData().realise<Data1D>(
            fmt::format("Difference//{}", module->uniqueName()), uniqueName(), GenericItem::InRestartFileFlag);
        differenceData = originalReferenceData;
        Interpolator::addInterpolated(differenceData, weightedSQ.total(), -1.0);

        // Calculate r-factor over fit range and store
        auto tempRefData = originalReferenceData;
        Filters::trim(tempRefData, qMin_, qMax_);
        auto rFactor = Error::rFactor(tempRefData, weightedSQ.total(), true);
        rFacTot += rFactor;
        errors.addPoint(dissolve.iteration(), rFactor);
        Messenger::print("Current R-Factor for reference data '{}' is {:.5f}.\n", module->uniqueName(), rFactor);

        /*
         * Generate difference function for fitting, spanning (maximally) only the range requested
         */

        // Get difference and fit function objects
        auto &deltaFQ = dissolve.processingModuleData().realise<Data1D>(fmt::format("DeltaFQ//{}", module->uniqueName()),
                                                                        uniqueName_, GenericItem::InRestartFileFlag);
        auto &deltaFQFit = dissolve.processingModuleData().realise<Data1D>(fmt::format("DeltaFQFit//{}", module->uniqueName()),
                                                                           uniqueName_, GenericItem::InRestartFileFlag);

        // Copy the original difference data and trim to the allowed range
        deltaFQ = differenceData;
        Filters::trim(deltaFQ, qMin_, qMax_);
        deltaFQ *= -1.0;

        // Fit a function expansion to the deltaFQ - if the coefficient arrays already exist then re-fit starting from those.
        auto [fitCoefficients, status] = dissolve.processingModuleData().realiseIf<std::vector<double>>(
            fmt::format("FitCoefficients_{}", module->uniqueName()), uniqueName_, GenericItem::InRestartFileFlag);

        auto fitError = 0.0;
        if (expansionFunction_ == EPSRModule::GaussianExpansionFunction)
        {
            // Construct our fitting object
            GaussFit coeffMinimiser(deltaFQ);

            if (status == GenericItem::ItemStatus::Created)
                fitError = coeffMinimiser.constructReciprocal(0.0, rMaxPT_, nCoeffP_, gSigma1_, nPItSs_, 0.01, 0, 3, 3, false);
            else
            {
                if (fitCoefficients.size() != nCoeffP_)
                {
                    Messenger::warn("Number of terms ({}) in existing FitCoefficients array for target '{}' does "
                                    "not match the current number ({}), so will fit from scratch.\n",
                                    fitCoefficients.size(), module->uniqueName(), nCoeffP_);
                    fitError =
                        coeffMinimiser.constructReciprocal(0.0, rMaxPT_, nCoeffP_, gSigma1_, nPItSs_, 0.01, 0, 3, 3, false);
                }
                else
                    fitError = coeffMinimiser.constructReciprocal(0.0, rMaxPT_, fitCoefficients, gSigma1_, nPItSs_, 0.01, 0, 3,
                                                                  3, false);
            }

            // Store the new fit coefficients
            std::copy(coeffMinimiser.A().begin(), coeffMinimiser.A().end(), fitCoefficients.begin());

            deltaFQFit = coeffMinimiser.approximation();
        }
        else if (expansionFunction_ == EPSRModule::PoissonExpansionFunction)
        {
            // Construct our fitting object
            PoissonFit coeffMinimiser(deltaFQ);

            if (status == GenericItem::ItemStatus::Created)
                fitError = coeffMinimiser.constructReciprocal(0.0, rMaxPT_, nCoeffP_, pSigma1_, pSigma2_, nPItSs_, 0.1, 0, 3, 3,
                                                              false);
            else
            {
                if (fitCoefficients.size() != nCoeffP_)
                {
                    Messenger::warn("Number of terms ({}) in existing FitCoefficients array for target '{}' does "
                                    "not match the current number ({}), so will fit from scratch.\n",
                                    fitCoefficients.size(), module->uniqueName(), nCoeffP_);
                    fitError = coeffMinimiser.constructReciprocal(0.0, rMaxPT_, nCoeffP_, pSigma1_, pSigma2_, nPItSs_, 0.01, 0,
                                                                  3, 3, false);
                }
                else
                    fitError = coeffMinimiser.constructReciprocal(0.0, rMaxPT_, fitCoefficients, pSigma1_, pSigma2_, nPItSs_,
                                                                  0.01, 0, 3, 3, false);
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
        auto &simulatedFR = dissolve.processingModuleData().realise<Data1D>(
            fmt::format("SimulatedFR//{}", module->uniqueName()), uniqueName_, GenericItem::InRestartFileFlag);

        // Copy the total calculated F(Q) and trim to the same range as the experimental data before FT
        simulatedFR = weightedSQ.total();
        Filters::trim(simulatedFR, originalReferenceData);
        Fourier::sineFT(simulatedFR, 1.0 / (2 * PI * PI * targetConfiguration_->atomicDensity()), 0.0, 0.03, 30.0,
                        WindowFunction(WindowFunction::Form::Lorch0));

        /*
         * Add the Data to the Scattering Matrix
         */

        if (module->type() == "NeutronSQ")
        {
            const auto &weights = dissolve.processingModuleData().value<NeutronWeights>("FullWeights", module->uniqueName());

            // Subtract intramolecular total from the reference data - this will enter into the ScatteringMatrix
            auto refMinusIntra = originalReferenceData;
            Interpolator::addInterpolated(refMinusIntra, weightedSQ.boundTotal(false), -1.0);

            // Always add absolute data to the scattering matrix - if the calculated data has been normalised, remove this
            // normalisation from the reference data (we assume that the two are consistent)
            auto normType = module->keywords().getEnumeration<StructureFactors::NormalisationType>("Normalisation");
            if (normType == StructureFactors::AverageOfSquaresNormalisation)
                refMinusIntra *= weights.boundCoherentAverageOfSquares();
            else if (normType == StructureFactors::SquareOfAverageNormalisation)
                refMinusIntra *= weights.boundCoherentSquareOfAverage();

            if (!scatteringMatrix.addReferenceData(refMinusIntra, weights, feedback_))
                return Messenger::error("Failed to add target data '{}' to weights matrix.\n", module->uniqueName());
        }
        else if (module->type() == "XRaySQ")
        {
            auto &weights = dissolve.processingModuleData().retrieve<XRayWeights>("FullWeights", module->uniqueName());

            // For X-ray data we always add the reference data normalised to AverageOfSquares in order to give consistency in
            // terms of magnitude with any neutron data. If the calculated data have not been normalised, or were normalised to
            // something else, we correct it before adding.
            auto normalisedRef = originalReferenceData;
            auto normType = module->keywords().getEnumeration<StructureFactors::NormalisationType>("Normalisation");
            if (normType == StructureFactors::SquareOfAverageNormalisation)
            {
                // Remove square of average normalisation, and apply average of squares
                auto bbarOld = weights.boundCoherentSquareOfAverage(normalisedRef.xAxis());
                auto bbarNew = weights.boundCoherentAverageOfSquares(normalisedRef.xAxis());
                for (auto &&[val, old, nw] : zip(normalisedRef.values(), bbarOld, bbarNew))
                    val *= old / nw;
            }
            else if (normType == StructureFactors::NoNormalisation)
            {
                auto bbar = weights.boundCoherentAverageOfSquares(normalisedRef.xAxis());
                std::transform(normalisedRef.values().begin(), normalisedRef.values().end(), bbar.begin(),
                               normalisedRef.values().begin(), std::divides<>());
            }

            // Subtract intramolecular total from the reference data - this will enter into the ScatteringMatrix
            // Our reference data is normalised to AverageOfSquares at this point, so must do the same to the
            // bound total before subtracting it.
            auto boundTotal = weightedSQ.boundTotal(false);
            auto bbar = weights.boundCoherentAverageOfSquares(boundTotal.xAxis());
            std::transform(boundTotal.values().begin(), boundTotal.values().end(), bbar.begin(), boundTotal.values().begin(),
                           std::divides<>());
            Interpolator::addInterpolated(normalisedRef, boundTotal, -1.0);

            if (!scatteringMatrix.addReferenceData(normalisedRef, weights, feedback_))
                return Messenger::error("Failed to add target data '{}' to weights matrix.\n", module->uniqueName());
        }
        else
            return Messenger::error("Don't know how to add data from a module of type '{}' to the scattering matrix.",
                                    module->type());

        /*
         * Sum Unweighted S(Q)
         */

        // Add the unweighted from this target to our combined, unweighted S(Q) data
        auto &types = unweightedSQ.atomTypeMix();
        dissolve::for_each_pair(
            ParallelPolicies::seq, types.begin(), types.end(),
            [&](int i, const AtomTypeData &atd1, int j, const AtomTypeData &atd2) {
                auto globalI = atd1.atomType()->index();
                auto globalJ = atd2.atomType()->index();

                const auto &partialIJ = unweightedSQ.unboundPartial(i, j);
                Interpolator::addInterpolated(calculatedUnweightedSQ[{globalI, globalJ}], partialIJ, 1.0 / targets_.size());
            });

        /*
         * Save Data
         */

        if (saveDifferenceFunctions_)
        {
            if (procPool.isMaster())
            {
                Data1DExportFileFormat exportFormat(fmt::format("{}-Diff.q", module->uniqueName()));
                if (exportFormat.exportData(differenceData))
                    procPool.decideTrue();
                else
                    return procPool.decideFalse();
            }
            else if (!procPool.decision())
                return true;

            if (procPool.isMaster())
            {
                Data1DExportFileFormat exportFormat(fmt::format("{}-DiffFit.q", module->uniqueName()));
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
                Data1DExportFileFormat exportFormat(fmt::format("{}-SimulatedFR.r", module->uniqueName()));
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
            testDataName = fmt::format("WeightedFR-{}-total", module->uniqueName());
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
    auto &totalRFactor =
        dissolve.processingModuleData().realise<Data1D>("RFactor", uniqueName_, GenericItem::InRestartFileFlag);
    totalRFactor.addPoint(dissolve.iteration(), rFacTot);
    Messenger::print("Current total R-Factor is {:.5f}.\n", rFacTot);

    /*
     * Augment the Scattering Matrix
     */

    // Add a contribution from each interatomic partial S(Q), weighted according to the feedback factor
    for_each_pair_early(dissolve.atomTypes().begin(), dissolve.atomTypes().end(),
                        [&](int i, auto at1, int j, auto at2) -> EarlyReturn<bool> {
                            // Copy and rename the data for clarity
                            auto data = calculatedUnweightedSQ[{i, j}];
                            data.setTag(fmt::format("Simulated {}-{}", at1->name(), at2->name()));

                            // Add this partial data to the scattering matrix - its factored weight will be (1.0 - feedback)
                            if (!scatteringMatrix.addPartialReferenceData(data, at1, at2, 1.0, (1.0 - feedback_)))
                                return Messenger::error("EPSR: Failed to augment scattering matrix with partial {}-{}.\n",
                                                        at1->name(), at2->name());
                            return EarlyReturn<bool>::Continue;
                        });

    scatteringMatrix.print();

    if (Messenger::isVerbose())
    {
        Messenger::print("\nScattering Matrix Inverse (Q = 0.0):\n");
        scatteringMatrix.printInverse();

        Messenger::print("\nIdentity (Ainv * A):\n");
        scatteringMatrix.matrixProduct().print();
    }

    /*
     * Generate S(Q) from completed scattering matrix
     */

    scatteringMatrix.generatePartials(estimatedSQ);
    updateDeltaSQ(dissolve.processingModuleData(), calculatedUnweightedSQ, estimatedSQ);

    // Save data?
    if (saveEstimatedPartials_)
    {
        if (procPool.isMaster())
        {
            for (auto &sq : estimatedSQ)
            {
                Data1DExportFileFormat exportFormat(sq.tag());
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
                            [&](int i, auto at1, int j, auto at2) -> EarlyReturn<bool> {
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
        dissolve.processingModuleData().realise<Array2D<Data1D>>("EstimatedGR", uniqueName_, GenericItem::InRestartFileFlag);
    estimatedGR.initialise(dissolve.nAtomTypes(), dissolve.nAtomTypes(), true);
    dissolve::for_each_pair(ParallelPolicies::seq, dissolve.atomTypes().begin(), dissolve.atomTypes().end(),
                            [&](int i, auto at1, int j, auto at2) {
                                auto &expGR = estimatedGR[{i, j}];
                                expGR.setTag(fmt::format("{}-{}", at1->name(), at2->name()));

                                // Copy experimental S(Q) and FT it
                                expGR = estimatedSQ[{i, j}];
                                Fourier::sineFT(expGR, 1.0 / (2 * PI * PI * targetConfiguration_->atomicDensity()), 0.0, 0.05,
                                                30.0, WindowFunction(WindowFunction::Form::Lorch0));
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
            fmt::format("FitCoefficients_{}", module->uniqueName()), uniqueName_);

        // Loop over pair potentials and retrieve the inverse weight from the scattering matrix
        dissolve::for_each_pair(ParallelPolicies::seq, dissolve.atomTypes().begin(), dissolve.atomTypes().end(),
                                [&](int i, auto at1, int j, auto at2) {
                                    auto weight = scatteringMatrix.pairWeightInverse(0.0, at1, at2, dataIndex);

                                    // Halve contribution from unlike terms to avoid adding double the potential for those
                                    // partials
                                    if (i != j)
                                        weight *= 0.5;

                                    // Store fluctuation coefficients ready for addition to potential coefficients later on.
                                    for (auto n = 0; n < nCoeffP_; ++n)
                                        fluctuationCoefficients[{i, j, n}] += weight * fitCoefficients[n];
                                });

        // Increase dataIndex
        ++dataIndex;
    }

    // Generate new empirical potentials
    auto energabs = 0.0;
    if (modifyPotential_)
    {
        // Sum fluctuation coefficients in to the potential coefficients
        auto &coefficients = potentialCoefficients(dissolve, nAtomTypes, nCoeffP_);
        dissolve::for_each_pair(ParallelPolicies::seq, dissolve.atomTypes().begin(), dissolve.atomTypes().end(),
                                [&](int i, auto at1, int j, auto at2) {
                                    auto &potCoeff = coefficients[{i, j}];

                                    // Zero potential before adding in fluctuation coefficients?
                                    if (overwritePotentials_)
                                        std::fill(potCoeff.begin(), potCoeff.end(), 0.0);

                                    // Perform smoothing of the fluctuation coefficients before we sum them into the potential
                                    // (the un-smoothed coefficients are stored)
                                    Data1D smoothedCoefficients;
                                    for (auto n = 0; n < nCoeffP_; ++n)
                                        smoothedCoefficients.addPoint(n, fluctuationCoefficients[{i, j, n}]);
                                    Filters::kolmogorovZurbenko(smoothedCoefficients, 3, 5);

                                    // Add in fluctuation coefficients
                                    for (auto n = 0; n < nCoeffP_; ++n)
                                        potCoeff[n] += weighting_ * smoothedCoefficients.value(n);

                                    // Set first term to zero (following EPSR)
                                    potCoeff[0] = 0.0;
                                });

        // Determine absolute energy of empirical potentials
        energabs = absEnergyEP(dissolve);

        // Apply factor of 1.0/rho to abs energy if using Poisson approximation (since this term is not present in the
        // fit functions)
        if (expansionFunction_ == EPSRModule::PoissonExpansionFunction)
            energabs /= targetConfiguration_->atomicDensity();

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

        if (!generateEmpiricalPotentials(dissolve, expansionFunction_, targetConfiguration_->atomicDensity(), nCoeffP_, rMinPT_,
                                         rMaxPT_, sigma1, sigma2))
            return false;
    }
    else
        energabs = absEnergyEP(dissolve);

    // Save data?
    if (saveEmpiricalPotentials_)
    {
        if (procPool.isMaster())
        {
            dissolve::for_each_pair(ParallelPolicies::seq, dissolve.atomTypes().begin(), dissolve.atomTypes().end(),
                                    [&](int i, auto at1, int j, auto at2) -> std::optional<bool> {
                                        // Grab pointer to the relevant pair potential
                                        PairPotential *pp = dissolve.pairPotential(at1, at2);

                                        Data1DExportFileFormat exportFormat(
                                            fmt::format("EP-{}-{}.txt", at1->name(), at2->name()));
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
            auto &coefficients = potentialCoefficients(dissolve, nAtomTypes, nCoeffP_);

            dissolve::for_each_pair(ParallelPolicies::seq, dissolve.atomTypes().begin(), dissolve.atomTypes().end(),
                                    [&](int i, auto at1, int j, auto at2) -> std::optional<bool> {
                                        // Grab reference to coefficients
                                        auto &potCoeff = coefficients[{i, j}];

                                        LineParser fileParser;
                                        if (!fileParser.openOutput(fmt::format("PCof-{}-{}.txt", at1->name(), at2->name())))
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
    auto &phiArray = dissolve.processingModuleData().realise<Data1D>("EPMag", uniqueName_, GenericItem::InRestartFileFlag);
    phiArray.addPoint(dissolve.iteration(), energabs);

    return true;
}
