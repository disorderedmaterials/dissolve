// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/neutronweights.h"
#include "classes/partialset.h"
#include "classes/scatteringmatrix.h"
#include "classes/xrayweights.h"
#include "data/isotopes.h"
#include "genericitems/listhelper.h"
#include "io/export/data1d.h"
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
bool EPSRModule::setUp(Dissolve &dissolve, ProcessPool &procPool)
{
    // Check for exactly one Configuration referenced through target modules
    targetConfiguration_ = nullptr;
    auto rho = 0.0;
    for (auto *module : targets_)
    {
        // Retrieve source SQ module, and then the related RDF module
        const SQModule *sqModule = module->keywords().retrieve<const SQModule *>("SourceSQs", nullptr);
        if (!sqModule)
            return Messenger::error(
                "Target '{}' doesn't source any S(Q) data, so it can't be used as a target for the EPSR module.",
                module->uniqueName());
        const RDFModule *rdfModule = sqModule->keywords().retrieve<const RDFModule *>("SourceRDFs", nullptr);
        if (!rdfModule)
            return Messenger::error("Target '{}'s S(Q) module doesn't reference an RDFModule, it can't be used as a target "
                                    "for the EPSR module.",
                                    module->uniqueName());

        // Check for number of targets, or different target if there's only 1
        if (rdfModule->nTargetConfigurations() != 1)
            return Messenger::error("RDF module '{}' targets multiple configurations, which is not permitted when using "
                                    "it's data in the EPSR module.",
                                    rdfModule->uniqueName());

        if ((targetConfiguration_ != nullptr) && (targetConfiguration_ != rdfModule->targetConfigurations().firstItem()))
            return Messenger::error("RDF module '{}' targets a configuration which is different from another target "
                                    "module, and which is not permitted when using it's data in the EPSR module.",
                                    rdfModule->uniqueName());
        else
            targetConfiguration_ = rdfModule->targetConfigurations().firstItem();

        rho = targetConfiguration_->atomicDensity();
    }

    // If a pcof file was provided, read in the parameters from it here
    std::string pcofFile = keywords_.asString("PCofFile");
    if (!pcofFile.empty())
    {
        Messenger::print("Reading potential coefficients from '{}'...\n", pcofFile);

        // Read in the coefficients / setup from the supplied file
        if (!readPCof(dissolve, procPool, pcofFile))
            return Messenger::error("Failed to read in potential coefficients from EPSR pcof file.\n");

        // Get some control parameters
        // const double ereq = keywords_.asDouble("EReq");
        auto ncoeffp = keywords_.asInt("NCoeffP");
        double rmaxpt = keywords_.asDouble("RMaxPT");
        double rminpt = keywords_.asDouble("RMinPT");

        // Set up the additional potentials - reconstruct them from the current coefficients
        auto functionType = keywords_.enumeration<EPSRModule::ExpansionFunctionType>("ExpansionFunction");
        if (functionType == EPSRModule::GaussianExpansionFunction)
        {
            const auto gsigma1 = keywords_.asDouble("GSigma1");
            const auto gsigma2 = keywords_.asDouble("GSigma2");
            if (!generateEmpiricalPotentials(dissolve, functionType, rho, ncoeffp, rminpt, rmaxpt, gsigma1, gsigma2))
                return false;
        }
        else
        {
            const auto psigma1 = keywords_.asDouble("PSigma1");
            const auto psigma2 = keywords_.asDouble("PSigma2");
            if (!generateEmpiricalPotentials(dissolve, functionType, rho, ncoeffp, rminpt, rmaxpt, psigma1, psigma2))
                return false;
        }
    }

    return true;
}

// Run main processing
bool EPSRModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    int i, j;
    std::string testDataName;

    /*
     * Get Keyword Options
     */
    const auto ereq = keywords_.asDouble("EReq");
    auto functionType = keywords_.enumeration<EPSRModule::ExpansionFunctionType>("ExpansionFunction");
    const auto feedback = keywords_.asDouble("Feedback");
    const auto gsigma1 = keywords_.asDouble("GSigma1");
    const auto gsigma2 = keywords_.asDouble("GSigma2");
    const bool modifyPotential = keywords_.asBool("ModifyPotential");
    auto ncoeffp = keywords_.asInt("NCoeffP");
    const auto npitss = keywords_.asInt("NPItSs");
    const bool onlyWhenEnergyStable = keywords_.asBool("OnlyWhenEnergyStable");
    const auto psigma1 = keywords_.asDouble("PSigma1");
    const auto psigma2 = keywords_.asDouble("PSigma2");
    const auto qMax = keywords_.asDouble("QMax");
    const auto qMin = keywords_.asDouble("QMin");
    double rmaxpt = keywords_.asDouble("RMaxPT");
    double rminpt = keywords_.asDouble("RMinPT");
    const bool saveDifferences = keywords_.asBool("SaveDifferenceFunctions");
    const bool saveSimulatedFR = keywords_.asBool("SaveSimulatedFR");
    const bool saveEmpiricalPotentials = keywords_.asBool("SaveEmpiricalPotentials");
    const bool saveEstimatedPartials = keywords_.asBool("SaveEstimatedPartials");
    const bool savePotentialCoefficients = keywords_.asBool("SavePCof");
    const bool testMode = keywords_.asBool("Test");
    const bool overwritePotentials = keywords_.asBool("OverwritePotentials");
    const auto testThreshold = keywords_.asDouble("TestThreshold");
    const auto weighting = keywords_.asDouble("Weighting");

    // EPSR constants
    const auto mcoeff = 200;

    // Calculate some values if they were not provided
    if (rmaxpt < 0.0)
        rmaxpt = dissolve.pairPotentialRange();
    if (rminpt < 0.0)
        rminpt = rmaxpt - 2.0;
    if (ncoeffp <= 0)
        ncoeffp = std::min(int(10.0 * rmaxpt + 0.0001), mcoeff);

    // Print option summary
    Messenger::print("EPSR: Feedback factor is {}.\n", feedback);
    Messenger::print("EPSR: {} functions will be used to approximate difference data.\n",
                     expansionFunctionTypes().keyword(functionType));
    Messenger::print("EPSR: Number of functions used in approximation is {}, sigma(Q) = {}.\n", ncoeffp, psigma2);
    if (modifyPotential)
        Messenger::print("EPSR: Perturbations to interatomic potentials will be generated and applied.\n");
    else
        Messenger::print("EPSR: Perturbations to interatomic potentials will be generated only (current potentials "
                         "will not be modified).\n");
    if (onlyWhenEnergyStable)
        Messenger::print("EPSR: Potential refinement will only be performed if all related Configuration energies are "
                         "stable.\n");
    Messenger::print("EPSR: Range for potential generation is {} < Q < {} Angstroms**-1.\n", qMin, qMax);
    Messenger::print("EPSR: Weighting factor used when applying fluctuation coefficients is {}\n", weighting);
    if (saveDifferences)
        Messenger::print("EPSR: Difference functions will be saved.\n");
    if (saveEmpiricalPotentials)
        Messenger::print("EPSR: Empirical potentials will be saved.\n");
    if (saveEstimatedPartials)
        Messenger::print("EPSR: Estimated partials will be saved.\n");
    if (savePotentialCoefficients)
        Messenger::print("EPSR: Potential coefficients will be saved.\n");
    if (saveSimulatedFR)
        Messenger::print("EPSR: Simulated F(r) (from FT of F(Q)) will be saved.\n");
    if (testMode)
        Messenger::print("EPSR: Test mode is enabled (threshold = {}%).", testThreshold);
    Messenger::print("\n");

    /*
     * Do we have targets to refine against?
     */
    if (targets_.nItems() == 0)
        return Messenger::error("At least one Module target containing suitable data must be provided.\n");

    if (!targetConfiguration_)
        return Messenger::error("No target configuration is set.\n");

    // Is the ctarget configuration's energy stable?
    if (onlyWhenEnergyStable)
    {
        auto stabilityResult = EnergyModule::checkStability(targetConfiguration_);
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
    Array3D<double> fluctuationCoefficients(nAtomTypes, nAtomTypes, ncoeffp);
    fluctuationCoefficients = 0.0;

    // Create storage for our summed UnweightedSQ
    auto &combinedUnweightedSQ = GenericListHelper<Array2D<Data1D>>::realise(dissolve.processingModuleData(), "UnweightedSQ",
                                                                             uniqueName_, GenericItem::InRestartFileFlag);
    combinedUnweightedSQ.initialise(nAtomTypes, nAtomTypes, true);

    // Set object names in combinedUnweightedSQ
    for_each_pair(dissolve.atomTypes().begin(), dissolve.atomTypes().end(), [&](int i, auto at1, int j, auto at2) {
        combinedUnweightedSQ[{i, j}].setObjectTag(
            fmt::format("{}//UnweightedSQ//{}-{}", uniqueName(), at1->name(), at2->name()));
    });

    // Realise storage for generated S(Q), and initialise a scattering matrix
    auto &estimatedSQ = GenericListHelper<Array2D<Data1D>>::realise(dissolve.processingModuleData(), "EstimatedSQ", uniqueName_,
                                                                    GenericItem::InRestartFileFlag);
    ScatteringMatrix scatteringMatrix;
    scatteringMatrix.initialise(dissolve.atomTypes(), estimatedSQ, uniqueName_, "Default");

    // Loop over target data
    auto rFacTot = 0.0;
    bool found, created;
    for (auto *module : targets_)
    {
        /*
         * Retrieve data for this module
         */

        // Retrieve the weighted S(Q)/F(Q)
        const auto &weightedSQ = GenericListHelper<PartialSet>::value(dissolve.processingModuleData(), "WeightedSQ",
                                                                      module->uniqueName(), PartialSet(), &found);
        if (!found)
            return Messenger::error("Could not locate associated weighted neutron PartialSet for target '{}'.\n",
                                    module->uniqueName());

        // Get source SQModule in order to have access to the unweighted S(Q)
        const SQModule *sqModule = module->keywords().retrieve<const SQModule *>("SourceSQs", nullptr);
        if (!sqModule)
            return Messenger::error(
                "Module '{}' doesn't source any S(Q) data, so it can't be used to augment the scattering matrix.",
                module->uniqueName());

        // Retrieve the unweighted S(Q)/F(Q)
        const auto &unweightedSQ = GenericListHelper<PartialSet>::value(dissolve.processingModuleData(), "UnweightedSQ",
                                                                        sqModule->uniqueName(), PartialSet(), &found);
        if (!found)
            return Messenger::error("Could not locate UnweightedSQ for target '{}'.\n", module->uniqueName());

        // Retrieve the ReferenceData
        const auto &originalReferenceData = GenericListHelper<Data1D>::value(dissolve.processingModuleData(), "ReferenceData",
                                                                             module->uniqueName(), Data1D(), &found);
        if (!found)
            return Messenger::error("Could not locate ReferenceData for target '{}'.\n", module->uniqueName());

        // Realise the r-factor array and make sure its object name is set
        auto &errors =
            GenericListHelper<Data1D>::realise(dissolve.processingModuleData(), fmt::format("RFactor_{}", module->uniqueName()),
                                               uniqueName_, GenericItem::InRestartFileFlag);
        errors.setObjectTag(fmt::format("{}//RFactor//{}", uniqueName_, module->uniqueName()));

        /*
         * Calculate difference functions and current percentage errors in calculated vs reference target data.
         * Do this over the widest Q-range allowed by both datasets
         */

        // Get difference data container and form the difference between the reference and calculated data
        auto &differenceData = GenericListHelper<Data1D>::realise(dissolve.processingModuleData(),
                                                                  fmt::format("DifferenceData_{}", module->uniqueName()),
                                                                  uniqueName(), GenericItem::InRestartFileFlag);
        differenceData.setObjectTag(fmt::format("{}//Difference//{}", uniqueName_, module->uniqueName()));
        differenceData = originalReferenceData;
        Interpolator::addInterpolated(differenceData, weightedSQ.total(), -1.0);

        // Calculate and store r-factor
        auto rFactor = Error::rFactor(originalReferenceData, weightedSQ.total(), true);
        rFacTot += rFactor;
        errors.addPoint(dissolve.iteration(), rFactor);
        Messenger::print("Current R-Factor for reference data '{}' is {:.5f}.\n", module->uniqueName(), rFactor);

        /*
         * Generate difference function for fitting, spanning (maximally) only the range requested
         */

        // Get difference and fit function objects
        auto &deltaFQ =
            GenericListHelper<Data1D>::realise(dissolve.processingModuleData(), fmt::format("DeltaFQ_{}", module->uniqueName()),
                                               uniqueName_, GenericItem::InRestartFileFlag);
        auto &deltaFQFit = GenericListHelper<Data1D>::realise(dissolve.processingModuleData(),
                                                              fmt::format("DeltaFQFit_{}", module->uniqueName()), uniqueName_,
                                                              GenericItem::InRestartFileFlag);
        deltaFQ.setObjectTag(fmt::format("{}//DeltaFQ//{}", uniqueName_, module->uniqueName()));
        deltaFQFit.setObjectTag(fmt::format("{}//DeltaFQFit//{}", uniqueName_, module->uniqueName()));

        // Copy the original difference data and trim to the allowed range
        deltaFQ = differenceData;
        Filters::trim(deltaFQ, qMin, qMax);
        Interpolator::addInterpolated(deltaFQ, weightedSQ.total(), -1.0);
        deltaFQ *= -1.0;

        // Fit a function expansion to the deltaFQ - if the coefficient arrays already exist then re-fit starting from those.
        auto &fitCoefficients = GenericListHelper<std::vector<double>>::realise(
            dissolve.processingModuleData(), fmt::format("FitCoefficients_{}", module->uniqueName()), uniqueName_,
            GenericItem::InRestartFileFlag, &created);

        auto fitError = 0.0;
        if (functionType == EPSRModule::GaussianExpansionFunction)
        {
            // Construct our fitting object
            GaussFit coeffMinimiser(deltaFQ);

            if (created)
                fitError = coeffMinimiser.constructReciprocal(0.0, rmaxpt, ncoeffp, gsigma1, npitss, 0.01, 0, 3, 3, false);
            else
            {
                if (fitCoefficients.size() != ncoeffp)
                {
                    Messenger::warn("Number of terms ({}) in existing FitCoefficients array for target '{}' does "
                                    "not match the current number ({}), so will fit from scratch.\n",
                                    fitCoefficients.size(), module->uniqueName(), ncoeffp);
                    fitError = coeffMinimiser.constructReciprocal(0.0, rmaxpt, ncoeffp, gsigma1, npitss, 0.01, 0, 3, 3, false);
                }
                else
                    fitError =
                        coeffMinimiser.constructReciprocal(0.0, rmaxpt, fitCoefficients, gsigma1, npitss, 0.01, 0, 3, 3, false);
            }

            // Store the new fit coefficients
            std::copy(coeffMinimiser.A().begin(), coeffMinimiser.A().end(), fitCoefficients.begin());

            deltaFQFit = coeffMinimiser.approximation();
        }
        else if (functionType == EPSRModule::PoissonExpansionFunction)
        {
            // Construct our fitting object
            PoissonFit coeffMinimiser(deltaFQ);

            if (created)
                fitError =
                    coeffMinimiser.constructReciprocal(0.0, rmaxpt, ncoeffp, psigma1, psigma2, npitss, 0.1, 0, 3, 3, false);
            else
            {
                if (fitCoefficients.size() != ncoeffp)
                {
                    Messenger::warn("Number of terms ({}) in existing FitCoefficients array for target '{}' does "
                                    "not match the current number ({}), so will fit from scratch.\n",
                                    fitCoefficients.size(), module->uniqueName(), ncoeffp);
                    fitError = coeffMinimiser.constructReciprocal(0.0, rmaxpt, ncoeffp, psigma1, psigma2, npitss, 0.01, 0, 3, 3,
                                                                  false);
                }
                else
                    fitError = coeffMinimiser.constructReciprocal(0.0, rmaxpt, fitCoefficients, psigma1, psigma2, npitss, 0.01,
                                                                  0, 3, 3, false);
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
        auto &simulatedFR = GenericListHelper<Data1D>::realise(dissolve.processingModuleData(), "SimulatedFR",
                                                               module->uniqueName(), GenericItem::InRestartFileFlag);
        simulatedFR.setObjectTag(fmt::format("{}//SimulatedFR//{}", uniqueName_, module->uniqueName()));

        // Copy the total calculated F(Q) and trim to the same range as the experimental data before FT
        simulatedFR = weightedSQ.total();
        Filters::trim(simulatedFR, originalReferenceData);
        Fourier::sineFT(simulatedFR, 1.0 / (2 * PI * PI * targetConfiguration_->atomicDensity()), 0.0, 0.03, 30.0,
                        WindowFunction(WindowFunction::Lorch0Window));

        /*
         * Add the Data to the Scattering Matrix
         */

        if (module->type() == "NeutronSQ")
        {
            const auto &weights = GenericListHelper<NeutronWeights>::value(dissolve.processingModuleData(), "FullWeights",
                                                                           module->uniqueName(), NeutronWeights(), &found);
            if (!found)
                return Messenger::error("Could not locate NeutronWeights for target '{}'.\n", module->uniqueName());

            // Subtract intramolecular total from the reference data - this will enter into the ScatteringMatrix
            auto refMinusIntra = originalReferenceData;
            Interpolator::addInterpolated(refMinusIntra, weightedSQ.boundTotal(false), -1.0);

            // Always add absolute data to the scattering matrix - if the calculated data has been normalised, remove this
            // normalisation from the reference data (we assume that the two are consistent)
            auto normType = module->keywords().enumeration<StructureFactors::NormalisationType>("Normalisation");
            if (normType == StructureFactors::AverageOfSquaresNormalisation)
                refMinusIntra *= weights.boundCoherentAverageOfSquares();
            else if (normType == StructureFactors::SquareOfAverageNormalisation)
                refMinusIntra *= weights.boundCoherentSquareOfAverage();

            if (!scatteringMatrix.addReferenceData(refMinusIntra, weights, feedback))
                return Messenger::error("Failed to add target data '{}' to weights matrix.\n", module->uniqueName());
        }
        else if (module->type() == "XRaySQ")
        {
            auto &weights = GenericListHelper<XRayWeights>::retrieve(dissolve.processingModuleData(), "FullWeights",
                                                                     module->uniqueName(), XRayWeights(), &found);
            if (!found)
                return Messenger::error("Could not locate XRayWeights for target '{}'.\n", module->uniqueName());

            // For X-ray data we always add the reference data normalised to AverageOfSquares in order to give consistency in
            // terms of magnitude with any neutron data. If the calculated data have not been normalised, or were normalised to
            // something else, we correct it before adding.
            auto normalisedRef = originalReferenceData;
            auto normType = module->keywords().enumeration<StructureFactors::NormalisationType>("Normalisation");
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

            if (!scatteringMatrix.addReferenceData(normalisedRef, weights, feedback))
                return Messenger::error("Failed to add target data '{}' to weights matrix.\n", module->uniqueName());
        }
        else
            return Messenger::error("Don't know how to add data from a module of type '{}' to the scattering matrix.",
                                    module->type());

        /*
         * Sum Unweighted S(Q)
         */

        // Add the unweighted from this target to our combined, unweighted S(Q) data
        auto &types = unweightedSQ.atomTypes();
        for_each_pair(types.begin(), types.end(), [&](int i, const AtomTypeData &atd1, int j, const AtomTypeData &atd2) {
            auto globalI = atd1.atomType()->index();
            auto globalJ = atd2.atomType()->index();

            Data1D partialIJ = unweightedSQ.unboundPartial(i, j);
            Interpolator::addInterpolated(combinedUnweightedSQ[{globalI, globalJ}], partialIJ, 1.0 / targets_.nItems());
        });

        /*
         * Save Data
         */

        if (saveDifferences)
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
        if (saveSimulatedFR)
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

        if (testMode)
        {
            testDataName = fmt::format("WeightedFR-{}-total", module->uniqueName());
            if (testData_.containsData(testDataName))
            {
                double error = Error::percent(simulatedFR, testData_.data(testDataName));
                Messenger::print("Simulated F(r) reference data '{}' has error of {:7.3f}% with calculated data "
                                 "and is {} (threshold is {:6.3f}%)\n\n",
                                 testDataName, error, error <= testThreshold ? "OK" : "NOT OK", testThreshold);
                if (error > testThreshold)
                    return false;
            }
        }
    }

    // Finalise and store the total r-factor
    rFacTot /= targets_.nItems();
    auto &totalRFactor = GenericListHelper<Data1D>::realise(dissolve.processingModuleData(), "RFactor", uniqueName_,
                                                            GenericItem::InRestartFileFlag);
    totalRFactor.setObjectTag(fmt::format("{}//RFactor", uniqueName_));
    totalRFactor.addPoint(dissolve.iteration(), rFacTot);
    Messenger::print("Current total R-Factor is {:.5f}.\n", rFacTot);

    /*
     * Augment the Scattering Matrix
     */

    // Add a contribution from each interatomic partial S(Q), weighted according to the feedback factor
    for_each_pair_early(dissolve.atomTypes().begin(), dissolve.atomTypes().end(),
                        [&](int i, auto at1, int j, auto at2) -> EarlyReturn<bool> {
                            // Copy and rename the data for clarity
                            auto data = combinedUnweightedSQ[{i, j}];
                            data.setName(fmt::format("Simulated {}-{}", at1->name(), at2->name()));

                            // Add this partial data to the scattering matrix - its factored weight will be (1.0 - feedback)
                            if (!scatteringMatrix.addPartialReferenceData(data, at1, at2, 1.0, (1.0 - feedback)))
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

    // Save data?
    if (saveEstimatedPartials)
    {
        if (procPool.isMaster())
        {
            for (auto &sq : estimatedSQ)
            {
                // generatedArray[n].save(generatedArray[n].name());
                Data1DExportFileFormat exportFormat(sq.name());
                if (!exportFormat.exportData(sq))
                    return procPool.decideFalse();
            }
            procPool.decideTrue();
        }
        else if (!procPool.decision())
            return true;
    }

    // Test Mode
    if (testMode)
    {
        for_each_pair_early(dissolve.atomTypes().begin(), dissolve.atomTypes().end(),
                            [&](int i, auto at1, int j, auto at2) -> EarlyReturn<bool> {
                                testDataName = fmt::format("EstimatedSQ-{}-{}", at1->name(), at2->name());
                                if (testData_.containsData(testDataName))
                                {
                                    double error = Error::percent(estimatedSQ[{i, j}], testData_.data(testDataName));
                                    Messenger::print("Generated S(Q) reference data '{}' has error of {:7.3f}% with "
                                                     "calculated data and is {} (threshold is {:6.3f}%)\n\n",
                                                     testDataName, error, error <= testThreshold ? "OK" : "NOT OK",
                                                     testThreshold);
                                    if (error > testThreshold)
                                        return false;
                                }
                                return EarlyReturn<bool>::Continue;
                            });
    }

    /*
     * Calculate g(r) from estimatedSQ
     */

    auto &estimatedGR = GenericListHelper<Array2D<Data1D>>::realise(dissolve.processingModuleData(), "EstimatedGR", uniqueName_,
                                                                    GenericItem::InRestartFileFlag);
    estimatedGR.initialise(dissolve.nAtomTypes(), dissolve.nAtomTypes(), true);
    for_each_pair(dissolve.atomTypes().begin(), dissolve.atomTypes().end(), [&](int i, auto at1, int j, auto at2) {
        // Grab experimental g(r) container and make sure its object name is set
        auto &expGR = estimatedGR[{i, j}];
        expGR.setObjectTag(fmt::format("{}//EstimatedGR//{}-{}", uniqueName_, at1->name(), at2->name()));

        // Copy experimental S(Q) and FT it
        expGR = estimatedSQ[{i, j}];
        Fourier::sineFT(expGR, 1.0 / (2 * PI * PI * targetConfiguration_->atomicDensity()), 0.0, 0.05, 30.0,
                        WindowFunction(WindowFunction::Lorch0Window));
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
        // For this Module, retrive the coefficents of the fit performed above.
        const auto &fitCoefficients = GenericListHelper<std::vector<double>>::value(
            dissolve.processingModuleData(), fmt::format("FitCoefficients_{}", module->uniqueName()), uniqueName_);

        // Loop over pair potentials and retrieve the inverse weight from the scattering matrix
        for_each_pair(dissolve.atomTypes().begin(), dissolve.atomTypes().end(), [&](int i, auto at1, int j, auto at2) {
            auto weight = scatteringMatrix.pairWeightInverse(0.0, at1, at2, dataIndex);

            // Halve contribution from unlike terms to avoid adding double the potential for those partials
            if (i != j)
                weight *= 0.5;

            // Store fluctuation coefficients ready for addition to potential coefficients later on.
            for (auto n = 0; n < ncoeffp; ++n)
                fluctuationCoefficients[{i, j, n}] += weight * fitCoefficients[n];
        });

        // Increase dataIndex
        ++dataIndex;
    }

    // Generate new empirical potentials
    double energabs = 0.0;
    if (modifyPotential)
    {
        // Sum fluctuation coefficients in to the potential coefficients
        auto &coefficients = potentialCoefficients(dissolve, nAtomTypes, ncoeffp);
        for_each_pair(dissolve.atomTypes().begin(), dissolve.atomTypes().end(), [&](int i, auto at1, int j, auto at2) {
            auto &potCoeff = coefficients[{i, j}];

            // Zero potential before adding in fluctuation coefficients?
            if (overwritePotentials)
                std::fill(potCoeff.begin(), potCoeff.end(), 0.0);

            // Perform smoothing of the fluctuation coefficients before we sum them into the potential (the
            // un-smoothed coefficients are stored)
            Data1D smoothedCoefficients;
            for (auto n = 0; n < ncoeffp; ++n)
                smoothedCoefficients.addPoint(n, fluctuationCoefficients[{i, j, n}]);
            Filters::kolmogorovZurbenko(smoothedCoefficients, 3, 5);

            // Add in fluctuation coefficients
            for (auto n = 0; n < ncoeffp; ++n)
                potCoeff[n] += weighting * smoothedCoefficients.value(n);

            // Set first term to zero (following EPSR)
            potCoeff[0] = 0.0;
        });

        // Determine absolute energy of empirical potentials
        energabs = absEnergyEP(dissolve);

        // Apply factor of 1.0/rho to abs energy if using Poisson approximation (since this term is not present in the
        // fit functions)
        if (functionType == EPSRModule::PoissonExpansionFunction)
            energabs /= targetConfiguration_->atomicDensity();

        /*
         * Determine the scaling we will apply to the coefficients (if any)
         * Notes:
         * 	- ereq is actually read in as a variable called 'pressreq', then assigned to another variable
         * 'absolute_energy' in the EPSR25 source.
         * 	- erequnit appears to be set to the value of ereqstep read in from the 'ereqstep' command (clamped to
         * 0.0-1.0).
         */
        double pressfac = 1.0;
        double erequnit = 0.0, ereqstep = 0.0;

        if (fabs(ereq) == 0.0)
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
                pressfac = fabs(ereq) / fabs(energabs);
                if ((pressfac > 1.0) && (ereqstep == 0.0))
                    pressfac = 1.0;
            }
        }
        Messenger::print("  generate_ep>  {}  {}  {}\n", ereq, energabs, pressfac);

        // Scale coefficients
        for (auto &n : coefficients)
            std::transform(n.begin(), n.end(), n.begin(), [pressfac](auto value) { return value * pressfac; });
        energabs *= pressfac;

        // Generate additional potentials from the coefficients
        double sigma1 = functionType == EPSRModule::PoissonExpansionFunction ? psigma1 : gsigma1;
        double sigma2 = functionType == EPSRModule::PoissonExpansionFunction ? psigma2 : gsigma2;

        if (!generateEmpiricalPotentials(dissolve, functionType, targetConfiguration_->atomicDensity(), ncoeffp, rminpt, rmaxpt,
                                         sigma1, sigma2))
            return false;
    }
    else
        energabs = absEnergyEP(dissolve);

    // Save data?
    if (saveEmpiricalPotentials)
    {
        if (procPool.isMaster())
        {
            for_each_pair(dissolve.atomTypes().begin(), dissolve.atomTypes().end(),
                          [&](int i, auto at1, int j, auto at2) -> std::optional<bool> {
                              // Grab pointer to the relevant pair potential
                              PairPotential *pp = dissolve.pairPotential(at1, at2);

                              Data1DExportFileFormat exportFormat(fmt::format("EP-{}-{}.txt", at1->name(), at2->name()));
                              if (!exportFormat.exportData(pp->uAdditional()))
                                  return procPool.decideFalse();
                              return std::nullopt;
                          });
            procPool.decideTrue();
        }
        else if (!procPool.decision())
            return false;
    }
    if (savePotentialCoefficients)
    {
        if (procPool.isMaster())
        {
            auto &coefficients = potentialCoefficients(dissolve, nAtomTypes, ncoeffp);

            for_each_pair(dissolve.atomTypes().begin(), dissolve.atomTypes().end(),
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
    auto &phiArray = GenericListHelper<Data1D>::realise(dissolve.processingModuleData(), "EPMag", uniqueName_,
                                                        GenericItem::InRestartFileFlag);
    phiArray.setObjectTag(fmt::format("{}//EPMag", uniqueName_));
    phiArray.addPoint(dissolve.iteration(), energabs);

    return true;
}
