// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "classes/neutronweights.h"
#include "classes/partialset.h"
#include "classes/scatteringmatrix.h"
#include "classes/xrayweights.h"
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
#include "modules/gr/gr.h"
#include "modules/refine/refine.h"
#include "modules/sq/sq.h"
#include "templates/algorithms.h"
#include "templates/array3d.h"
#include <functional>

// Run set-up stage
bool RefineModule::setUp(Dissolve &dissolve, const ProcessPool &procPool, Flags<KeywordBase::KeywordSignal> actionSignals)
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
                "[SETUP {}] Target '{}' doesn't source any S(Q) data, so it can't be used as a target for the Refine module.",
                name_, module->name());

        auto *grModule = sqModule->sourceGR();
        if (!grModule)
            return Messenger::error(
                "[SETUP {}] Target '{}'s S(Q) module doesn't reference a GRModule, it can't be used as a target "
                "for the Refine module.",
                name_, module->name());

        // Check for number of targets, or different target if there's only 1
        auto rdfConfigs = grModule->keywords().getVectorConfiguration("Configurations");
        if (rdfConfigs.size() != 1)
            return Messenger::error(
                "[SETUP {}] GR module '{}' targets multiple configurations, which is not permitted when using "
                "its data in the Refine module.",
                name_, grModule->name());

        if ((targetConfiguration_ != nullptr) && (targetConfiguration_ != rdfConfigs.front()))
            return Messenger::error("[SETUP {}] GR module '{}' targets a configuration which is different from another target "
                                    "module, and which is not permitted when using its data in the Refine module.",
                                    name_, grModule->name());
        else
            targetConfiguration_ = rdfConfigs.front();

        rho = targetConfiguration_->atomicDensity();
    }

    // Try to calculate the deltaSQ array
    updateDeltaSQ(dissolve.processingModuleData());

    return true;
}

// Run main processing
bool RefineModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    std::string testDataName;

    // Refine constants
    const auto mcoeff = 200;

    // Calculate some values if they were not provided
    auto rmaxpt = rMaxPT_ ? rMaxPT_.value() : dissolve.pairPotentialRange();
    auto rminpt = rMinPT_ ? rMinPT_.value() : rmaxpt - 2.0;
    auto ncoeffp = nCoeffP_ ? nCoeffP_.value() : std::min(int(10.0 * rmaxpt + 0.0001), mcoeff);

    // Print option summary
    Messenger::print("Refine: Feedback factor is {}.\n", feedback_);
    Messenger::print("Refine: {} functions will be used to approximate difference data.\n",
                     expansionFunctionTypes().keyword(expansionFunction_));
    Messenger::print("Refine: Number of functions used in approximation is {}, sigma(Q) = {}.\n", ncoeffp, pSigma2_);
    if (modifyPotential_)
        Messenger::print(
            "Refine: Perturbations to interatomic potentials will be generated and applied with a frequency of {}.\n",
            *modifyPotential_);
    else
        Messenger::print("Refine: Perturbations to interatomic potentials will be generated only (current potentials "
                         "will not be modified).\n");
    Messenger::print("Refine: Range for potential generation is {} < Q < {} Angstroms**-1.\n", qMin_, qMax_);
    Messenger::print("Refine: Weighting factor used when applying fluctuation coefficients is {}\n", weighting_);
    if (fluctuationSmoothing_)
        Messenger::print("Refine: Coefficients will be smoothed (average length = 2N+1, N = {})", *fluctuationSmoothing_);
    if (saveDifferenceFunctions_)
        Messenger::print("Refine: Difference functions will be saved.\n");
    if (saveEmpiricalPotentials_)
        Messenger::print("Refine: Empirical potentials will be saved.\n");
    if (saveEstimatedPartials_)
        Messenger::print("Refine: Estimated partials will be saved.\n");
    if (savePotentialCoefficients_)
        Messenger::print("Refine: Potential coefficients will be saved.\n");
    if (saveSimulatedFR_)
        Messenger::print("Refine: Simulated F(r) (from FT of F(Q)) will be saved.\n");
    if (test_)
        Messenger::print("Refine: Test mode is enabled (threshold = {}%).", testThreshold_);
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
     * Refine Main
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
        differenceData = originalReferenceData;
        Interpolator::addInterpolated(weightedSQ.total(), differenceData, -1.0);

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

        // Copy the original difference data and trim to the allowed range
        deltaFQ = differenceData;
        Filters::trim(deltaFQ, qMin_, qMax_);
        deltaFQ *= -1.0;

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
            auto normType = module->keywords().getEnumeration<StructureFactors::NormalisationType>("Normalisation");
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
            auto boundTotal = weightedSQ.boundTotal();
            auto bbar = weights.boundCoherentAverageOfSquares(boundTotal.xAxis());
            std::transform(boundTotal.values().begin(), boundTotal.values().end(), bbar.begin(), boundTotal.values().begin(),
                           std::divides<>());
            Interpolator::addInterpolated(boundTotal, normalisedRef, -1.0);

            if (scatteringMatrixSetUp ? !scatteringMatrix_.updateReferenceData(normalisedRef, feedback_)
                                      : !scatteringMatrix_.addReferenceData(normalisedRef, weights, feedback_))
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
                                return Messenger::error("Refine: Failed to augment scattering matrix with partial {}-{}.\n",
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
     * TESTING BEGINS HERE
     * TESTING BEGINS HERE
     * TESTING BEGINS HERE
     * TESTING BEGINS HERE
     */



    /*
     * TESTING ENDS HERE
     * TESTING ENDS HERE
     * TESTING ENDS HERE
     * TESTING ENDS HERE
     */

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

    return true;
}
