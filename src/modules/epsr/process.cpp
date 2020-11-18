// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/neutronweights.h"
#include "classes/partialset.h"
#include "classes/scatteringmatrix.h"
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
#include "modules/rdf/rdf.h"
#include "modules/sq/sq.h"
#include "templates/algorithms.h"
#include "templates/array3d.h"

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

    /*
     * Calculate difference functions and current percentage errors in calculated vs reference target data
     */
    double rFacTot = 0.0;
    for (auto *module : targets_)
    {
        // Realise the error array and make sure its object name is set
        auto &errors =
            GenericListHelper<Data1D>::realise(dissolve.processingModuleData(), fmt::format("RFactor_{}", module->uniqueName()),
                                               uniqueName_, GenericItem::InRestartFileFlag);
        errors.setObjectTag(fmt::format("{}//RFactor//{}", uniqueName_, module->uniqueName()));

        // Calculate our error based on the type of Module
        double rFactor = 100.0;
        if (DissolveSys::sameString(module->type(), "NeutronSQ"))
        {
            bool found;

            // Get difference data container
            auto &differenceData = GenericListHelper<Data1D>::realise(dissolve.processingModuleData(),
                                                                      fmt::format("DifferenceData_{}", module->uniqueName()),
                                                                      uniqueName(), GenericItem::InRestartFileFlag);
            differenceData.setObjectTag(fmt::format("{}//Difference//{}", uniqueName_, module->uniqueName()));

            // Retrieve the ReferenceData from the Module (as Data1D)
            const auto &referenceData = GenericListHelper<Data1D>::value(dissolve.processingModuleData(), "ReferenceData",
                                                                         module->uniqueName(), Data1D(), &found);
            if (!found)
            {
                Messenger::warn("Could not locate ReferenceData for target '{}'.\n", module->uniqueName());
                return false;
            }
            differenceData = referenceData;

            // Retrieve the PartialSet from the Module
            const auto &calcSQ = GenericListHelper<PartialSet>::value(dissolve.processingModuleData(), "WeightedSQ",
                                                                      module->uniqueName(), PartialSet(), &found);
            if (!found)
            {
                Messenger::warn("Could not locate associated weighted neutron PartialSet for target '{}'.\n",
                                module->uniqueName());
                return false;
            }
            auto calcSQTotal = calcSQ.total();

            // Determine overlapping Q range between the two datasets
            double FQMin = qMin, FQMax = qMax;
            if ((FQMin < differenceData.xAxis().front()) || (FQMin < calcSQTotal.xAxis().front()))
                FQMin = std::max(differenceData.xAxis().front(), calcSQTotal.xAxis().front());
            if ((FQMax > differenceData.xAxis().back()) || (FQMax > calcSQTotal.xAxis().back()))
                FQMax = std::min(differenceData.xAxis().back(), calcSQTotal.xAxis().back());

            // Trim both datasets to the common range
            Filters::trim(calcSQTotal, FQMin, FQMax, true);
            Filters::trim(differenceData, FQMin, FQMax, true);

            rFactor = Error::rFactor(referenceData, calcSQTotal, true);
            rFacTot += rFactor;

            // Calculate difference function
            Interpolator::addInterpolated(differenceData, calcSQTotal, -1.0);

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
            }
        }
        else
            return Messenger::error("Unrecognised Module type '{}', so can't calculate error.", module->type());

        // Store the rFactor for this reference dataset
        errors.addPoint(dissolve.iteration(), rFactor);
        Messenger::print("Current R-Factor for reference data '{}' is {:.5f}.\n", module->uniqueName(), rFactor);
    }
    rFacTot /= targets_.nItems();

    // Realise the total rFactor array and make sure its object name is set
    auto &totalRFactor = GenericListHelper<Data1D>::realise(dissolve.processingModuleData(), "RFactor", uniqueName_,
                                                            GenericItem::InRestartFileFlag);
    totalRFactor.setObjectTag(fmt::format("{}//RFactor", uniqueName_));
    totalRFactor.addPoint(dissolve.iteration(), rFacTot);
    Messenger::print("Current total R-Factor is {:.5f}.\n", rFacTot);

    /*
     * Are the energies of all involved Configurations stable (if OnlyWhenEnergyStable option is on).
     * We still need to determine current errors for reference data, so store the result and carry on.
     */
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
     * Calculate difference functions for each experimental dataset and fit them.
     * Also calculate the simulated F(r) - for consistency with EPSR, this is the inverse FT of the simulated F(Q), rather
     * than the directly-calculated function
     */

    for (auto *module : targets_)
    {
        bool found;

        // Retrieve the reference (experimental) data for the target Module
        const auto &referenceData = GenericListHelper<Data1D>::value(dissolve.processingModuleData(), "ReferenceData",
                                                                     module->uniqueName(), Data1D(), &found);
        if (!found)
            return Messenger::error("Could not locate ReferenceData for target '{}'.\n", module->uniqueName());

        // Retrieve the simulated data for the target Module
        const auto &weightedSQ = GenericListHelper<PartialSet>::value(dissolve.processingModuleData(), "WeightedSQ",
                                                                      module->uniqueName(), PartialSet(), &found);
        if (!found)
            return Messenger::error("Could not locate WeightedSQ for target '{}'.\n", module->uniqueName());

        // Get associated S(Q) and RDF modules
        const SQModule *sqModule = module->keywords().retrieve<const SQModule *>("SourceSQs", nullptr);
        if (!sqModule)
            return Messenger::error("Module '{}' doesn't source any S(Q) data, so difference functions can't be calculated.",
                                    module->uniqueName());
        const RDFModule *rdfModule = sqModule->keywords().retrieve<const RDFModule *>("SourceRDFs", nullptr);
        if (!rdfModule)
            return Messenger::error(
                "Source S(Q) module doesn't reference an RDFModule, so the effective density can't be retrieved.");

        // Get difference and fit function objects
        auto &deltaFQ =
            GenericListHelper<Data1D>::realise(dissolve.processingModuleData(), fmt::format("DeltaFQ_{}", module->uniqueName()),
                                               uniqueName_, GenericItem::InRestartFileFlag);
        auto &deltaFQFit = GenericListHelper<Data1D>::realise(dissolve.processingModuleData(),
                                                              fmt::format("DeltaFQFit_{}", module->uniqueName()), uniqueName_,
                                                              GenericItem::InRestartFileFlag);
        deltaFQ.setObjectTag(fmt::format("{}//DeltaFQ//{}", uniqueName_, module->uniqueName()));
        deltaFQFit.setObjectTag(fmt::format("{}//DeltaFQFit//{}", uniqueName_, module->uniqueName()));

        // Set up data for construction of the deltaFQ
        deltaFQ.clear();
        const auto x1 = referenceData.xAxis();
        const auto y1 = referenceData.values();
        auto simulatedFQ = weightedSQ.total();
        Interpolator interpolatedSimFQ(simulatedFQ);

        // Determine allowable range for fit, based on requested values and limits of generated / simulated datasets.
        double deltaSQMin = qMin, deltaSQMax = (qMax < 0.0 ? x1.back() : qMax);
        if ((deltaSQMin < x1.front()) || (deltaSQMin < simulatedFQ.xAxis().front()))
            deltaSQMin = std::max(x1.front(), simulatedFQ.xAxis().front());
        if ((deltaSQMax > x1.back()) || (deltaSQMax > simulatedFQ.xAxis().back()))
            deltaSQMax = std::min(x1.back(), simulatedFQ.xAxis().back());

        double x;
        for (auto n = 0; n < x1.size(); ++n)
        {
            // Grab experimental data x value
            x = x1[n];

            // If this x value is below the minimum Q value we are fitting, set the difference to zero. Otherwise,
            // store the "inverse" value ([sim - exp], for consistency with EPSR)
            if (x < deltaSQMin)
                continue;
            else if (x > deltaSQMax)
                break;
            else
                deltaFQ.addPoint(x, interpolatedSimFQ.y(x) - y1[n]);
        }

        /*
         * Fit a function expansion to the deltaFQ
         */

        // Do the coefficient arrays already exist? If so, we re-fit from this set of coefficients. If not, start from
        // scratch
        bool created;
        auto &fitCoefficients = GenericListHelper<std::vector<double>>::realise(
            dissolve.processingModuleData(), fmt::format("FitCoefficients_{}", module->uniqueName()), uniqueName_,
            GenericItem::InRestartFileFlag, &created);

        if (functionType == EPSRModule::GaussianExpansionFunction)
        {
            // Construct our fitting object
            GaussFit coeffMinimiser(deltaFQ);

            if (created)
                coeffMinimiser.constructReciprocal(0.0, rmaxpt, ncoeffp, gsigma1, npitss, 0.01, 0, 3, 3, false);
            else
            {
                if (fitCoefficients.size() != ncoeffp)
                {
                    Messenger::warn("Number of terms ({}) in existing FitCoefficients array for target '{}' does "
                                    "not match the current number ({}), so will fit from scratch.\n",
                                    fitCoefficients.size(), module->uniqueName(), ncoeffp);
                    coeffMinimiser.constructReciprocal(0.0, rmaxpt, ncoeffp, gsigma1, npitss, 0.01, 0, 3, 3, false);
                }
                else
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
                coeffMinimiser.constructReciprocal(0.0, rmaxpt, ncoeffp, psigma1, psigma2, npitss, 0.1, 0, 3, 3, false);
            else
            {
                if (fitCoefficients.size() != ncoeffp)
                {
                    Messenger::warn("Number of terms ({}) in existing FitCoefficients array for target '{}' does "
                                    "not match the current number ({}), so will fit from scratch.\n",
                                    fitCoefficients.size(), module->uniqueName(), ncoeffp);
                    coeffMinimiser.constructReciprocal(0.0, rmaxpt, ncoeffp, psigma1, psigma2, npitss, 0.01, 0, 3, 3, false);
                }
                else
                    coeffMinimiser.constructReciprocal(0.0, rmaxpt, fitCoefficients, psigma1, psigma2, npitss, 0.01, 0, 3, 3,
                                                       false);
            }

            // Store the new fit coefficients
            fitCoefficients = coeffMinimiser.C();

            deltaFQFit = coeffMinimiser.approximation();
        }

        // Calculate F(r)
        auto &simulatedFR = GenericListHelper<Data1D>::realise(dissolve.processingModuleData(), "SimulatedFR",
                                                               module->uniqueName(), GenericItem::InRestartFileFlag);
        simulatedFR.setObjectTag(fmt::format("{}//SimulatedFR//{}", uniqueName_, module->uniqueName()));
        simulatedFR = simulatedFQ;
        Fourier::sineFT(simulatedFR,
                        1.0 / (2 * PI * PI *
                               GenericListHelper<double>::value(dissolve.processingModuleData(), "EffectiveRho",
                                                                rdfModule->uniqueName(), 0.0)),
                        0.0, 0.03, 30.0, WindowFunction(WindowFunction::Lorch0Window));

        // Save data?
        if (saveDifferences)
        {
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

        // Test Mode
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

    /*
     * Loop over groups of defined Module targets.
     * We will generate a contribution to dPhiR from each and blend them together.
     */

    // Set up storage for the changes to coefficients used to generate the empirical potentials
    const auto nAtomTypes = dissolve.nAtomTypes();
    Array3D<double> fluctuationCoefficients(nAtomTypes, nAtomTypes, ncoeffp);
    fluctuationCoefficients = 0.0;

    Messenger::print("Generating dPhiR...\n");

    /*
     * Create the full scattering matrix using all the reference (experimental) data we have.
     */

    // Create temporary storage for our summed UnweightedSQ, and related quantities such as density and sum factors
    auto &combinedUnweightedSQ = GenericListHelper<Array2D<Data1D>>::realise(dissolve.processingModuleData(), "UnweightedSQ",
                                                                             uniqueName_, GenericItem::InRestartFileFlag);
    Array2D<double> combinedRho, combinedFactor, combinedCWeights;
    combinedUnweightedSQ.initialise(nAtomTypes, nAtomTypes, true);
    combinedRho.initialise(nAtomTypes, nAtomTypes, true);
    combinedFactor.initialise(nAtomTypes, nAtomTypes, true);
    combinedCWeights.initialise(nAtomTypes, nAtomTypes, true);
    combinedRho = 0.0;
    combinedFactor = 0.0;
    combinedCWeights = 0.0;

    // Set object names in combinedUnweightedSQ
    for_each_pair(dissolve.atomTypes().begin(), dissolve.atomTypes().end(), [&](int i, auto at1, int j, auto at2) {
        combinedUnweightedSQ.at(i, j).setObjectTag(
            fmt::format("{}//UnweightedSQ//{}-{}", uniqueName(), at1->name(), at2->name()));
    });

    // Realise storage for generated S(Q), and initialise a scattering matrix
    auto &estimatedSQ = GenericListHelper<Array2D<Data1D>>::realise(dissolve.processingModuleData(), "EstimatedSQ", uniqueName_,
                                                                    GenericItem::InRestartFileFlag);
    ScatteringMatrix scatteringMatrix;
    scatteringMatrix.initialise(dissolve.atomTypes(), estimatedSQ, uniqueName_, "Default");

    // Add a row in the scattering matrix for each target in the group.
    for (Module *module : targets_)
    {
        bool found;

        // Retrieve the reference data, associated neutron weights and source unweighted and weighted partials
        const auto &referenceData = GenericListHelper<Data1D>::value(dissolve.processingModuleData(), "ReferenceData",
                                                                     module->uniqueName(), Data1D(), &found);
        if (!found)
            return Messenger::error("Could not locate ReferenceData for target '{}'.\n", module->uniqueName());

        auto &weights = GenericListHelper<NeutronWeights>::retrieve(dissolve.processingModuleData(), "FullWeights",
                                                                    module->uniqueName(), NeutronWeights(), &found);
        if (!found)
            return Messenger::error("Could not locate NeutronWeights for target '{}'.\n", module->uniqueName());

        const SQModule *sqModule = module->keywords().retrieve<const SQModule *>("SourceSQs", nullptr);
        if (!sqModule)
            return Messenger::error(
                "Module '{}' doesn't source any S(Q) data, so it can't be used to augment the scattering matrix.",
                module->uniqueName());
        const auto &unweightedSQ = GenericListHelper<PartialSet>::value(dissolve.processingModuleData(), "UnweightedSQ",
                                                                        sqModule->uniqueName(), PartialSet(), &found);
        if (!found)
            return Messenger::error("Could not locate UnweightedSQ for target '{}'.\n", module->uniqueName());

        const auto &weightedSQ = GenericListHelper<PartialSet>::value(dissolve.processingModuleData(), "WeightedSQ",
                                                                      module->uniqueName(), PartialSet(), &found);
        if (!found)
            return Messenger::error("Could not locate WeightedSQ for target '{}'.\n", module->uniqueName());

        const RDFModule *rdfModule = sqModule->keywords().retrieve<const RDFModule *>("SourceRDFs", nullptr);
        if (!rdfModule)
            return Messenger::error(
                "Source S(Q) module doesn't reference an RDFModule, so the effective density can't be retrieved.");
        auto rho = GenericListHelper<double>::value(dissolve.processingModuleData(), "EffectiveRho", rdfModule->uniqueName(),
                                                    0.0, &found);
        if (!found)
            return Messenger::error("Could not locate EffectiveRho for target '{}'.\n", module->uniqueName());

        // Subtract intramolecular total from the reference data - this will enter into the ScatteringMatrix
        auto refMinusIntra = referenceData, boundTotal = weightedSQ.boundTotal(false);
        Interpolator::addInterpolated(refMinusIntra, boundTotal, -1.0);

        // Add a row to our scattering matrix
        if (!scatteringMatrix.addReferenceData(refMinusIntra, weights, feedback))
            return Messenger::error("Failed to add target data '{}' to weights matrix.\n", module->uniqueName());

        // Sum up the unweighted partials and density from this target
        double factor = 1.0;
        auto &types = unweightedSQ.atomTypes();
        for_each_pair(types.begin(), types.end(), [&](int i, const AtomTypeData &atd1, int j, const AtomTypeData &atd2) {
            auto globalI = atd1.atomType()->index();
            auto globalJ = atd2.atomType()->index();

            Data1D partialIJ = unweightedSQ.unboundPartial(i, j);
            Interpolator::addInterpolated(combinedUnweightedSQ.at(globalI, globalJ), partialIJ, factor);
            combinedRho.at(globalI, globalJ) += rho * factor;
            combinedFactor.at(globalI, globalJ) += factor;
            combinedCWeights.at(globalI, globalJ) += weights.concentrationProduct(i, j);
        });
    }

    // Normalise the combined values
    for (i = 0; i < nAtomTypes; ++i)
    {
        for (j = i; j < nAtomTypes; ++j)
        {
            combinedUnweightedSQ.at(i, j) /= combinedFactor.constAt(i, j);
            combinedRho.at(i, j) /= combinedFactor.constAt(i, j);
            combinedCWeights.at(i, j) /= combinedFactor.constAt(i, j);
        }
    }

    /*
     * Augment the scattering matrix.
     */

    // Clear any existing simulated data
    simulatedReferenceData_.clear();

    // Create the array
    simulatedReferenceData_.createEmpty(combinedUnweightedSQ.linearArraySize());
    for_each_pair_early(dissolve.atomTypes().begin(), dissolve.atomTypes().end(),
                        [&](int i, auto at1, int j, auto at2) -> EarlyReturn<bool> {
                            // Copy the unweighted data and wight weight it according to the natural isotope / concentration
                            // factor calculated above
                            auto data = combinedUnweightedSQ.at(i, j);
                            data.setName(fmt::format("Simulated {}-{}", at1->name(), at2->name()));

                            // Add this partial data to the scattering matrix - its factored weight will be (1.0 - feedback)
                            if (!scatteringMatrix.addPartialReferenceData(data, at1, at2, 1.0, (1.0 - feedback)))
                                return Messenger::error("EPSR: Failed to augment scattering matrix with partial {}-{}.\n",
                                                        at1->name(), at2->name());
                            return EarlyReturn<bool>::Continue;
                        });

    scatteringMatrix.finalise();
    scatteringMatrix.print();

    if (Messenger::isVerbose())
    {
        Messenger::print("\nScattering Matrix Inverse:\n");
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
            Data1D *generatedArray = estimatedSQ.linearArray();
            for (auto n = 0; n < estimatedSQ.linearArraySize(); ++n)
            {
                // generatedArray[n].save(generatedArray[n].name());
                Data1DExportFileFormat exportFormat(generatedArray[n].name());
                if (!exportFormat.exportData(generatedArray[n]))
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
                                    double error = Error::percent(estimatedSQ.at(i, j), testData_.data(testDataName));
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
        auto &expGR = estimatedGR.at(i, j);
        expGR.setObjectTag(fmt::format("{}//EstimatedGR//{}-{}", uniqueName_, at1->name(), at2->name()));

        // Copy experimental S(Q) and FT it
        expGR = estimatedSQ.at(i, j);
        Fourier::sineFT(expGR, 1.0 / (2 * PI * PI * combinedRho.at(i, j)), 0.0, 0.05, 30.0,
                        WindowFunction(WindowFunction::Lorch0Window));
        expGR += 1.0;
    });

    /*
     * Calculate contribution to potential coefficients from this group.
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
            double weight = scatteringMatrix.pairWeightInverse(at1, at2, dataIndex);

            // Halve contribution from unlike terms to avoid adding double the potential for those partials
            if (i != j)
                weight *= 0.5;

            // Store fluctuation coefficients ready for addition to potential coefficients later on.
            for (auto n = 0; n < ncoeffp; ++n)
                fluctuationCoefficients.at(i, j, n) += weight * fitCoefficients[n];
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
            auto &potCoeff = coefficients.at(i, j);

            // Zero potential before adding in fluctuation coefficients?
            if (overwritePotentials)
                std::fill(potCoeff.begin(), potCoeff.end(), 0.0);

            // Perform smoothing of the fluctuation coefficients before we sum them into the potential (the
            // un-smoothed coefficients are stored)
            Data1D smoothedCoefficients;
            for (auto n = 0; n < ncoeffp; ++n)
                smoothedCoefficients.addPoint(n, fluctuationCoefficients.constAt(i, j, n));
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
        for (i = 0; i < coefficients.linearArraySize(); ++i)
            std::transform(coefficients.linearArray()[i].begin(), coefficients.linearArray()[i].end(),
                           coefficients.linearArray()[i].begin(), [pressfac](auto value) { return value * pressfac; });
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
                              auto &potCoeff = coefficients.at(i, j);

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
