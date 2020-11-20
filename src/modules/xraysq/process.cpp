// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/xrayweights.h"
#include "genericitems/listhelper.h"
#include "io/export/data1d.h"
#include "main/dissolve.h"
#include "math/filters.h"
#include "math/ft.h"
#include "modules/import/import.h"
#include "modules/rdf/rdf.h"
#include "modules/sq/sq.h"
#include "modules/xraysq/xraysq.h"
#include "templates/algorithms.h"

// Run set-up stage
bool XRaySQModule::setUp(Dissolve &dissolve, ProcessPool &procPool)
{
    /*
     * Load and set up reference data (if a file/format was given)
     */
    if (referenceFQ_.hasValidFileAndFormat())
    {
        // Load the data
        Data1D referenceData;
        if (!referenceFQ_.importData(referenceData, &procPool))
        {
            Messenger::error("Failed to load reference data '{}'.\n", referenceFQ_.filename());
            return false;
        }

        // Get dependent modules
        const SQModule *sqModule = keywords_.retrieve<const SQModule *>("SourceSQs", nullptr);
        if (!sqModule)
            return Messenger::error("A source SQ module must be provided.\n");
        const RDFModule *rdfModule = sqModule->keywords().retrieve<const RDFModule *>("SourceRDFs", nullptr);
        if (!rdfModule)
            return Messenger::error("A source RDF module (in the SQ module) must be provided.\n");

        // Remove normalisation factor from data
        auto normType = keywords_.enumeration<StructureFactors::NormalisationType>("ReferenceNormalisation");
        if (normType != StructureFactors::NoNormalisation)
        {
            // We need the x-ray weights in order to do the normalisation
            XRayFormFactors::XRayFormFactorData formFactors =
                keywords_.enumeration<XRayFormFactors::XRayFormFactorData>("FormFactors");
            XRayWeights weights;
            calculateWeights(rdfModule, weights, formFactors);

            // Remove normalisation from the data
            if (normType == StructureFactors::SquareOfAverageNormalisation)
            {
                auto bbar = weights.boundCoherentSquareOfAverage(referenceData.xAxis());
		std::transform(bbar.begin(), bbar.end(), referenceData.values().begin(), referenceData.values().begin(), [](auto b, auto ref){return ref / b;});
            }
            else if (normType == StructureFactors::AverageOfSquaresNormalisation)
            {
                auto bbar = weights.boundCoherentAverageOfSquares(referenceData.xAxis());
		std::transform(bbar.begin(), bbar.end(), referenceData.values().begin(), referenceData.values().begin(), [](auto b, auto ref){return ref / b;});
            }
        }

        // Get window function to use for transformation of S(Q) to g(r)
        const WindowFunction &referenceWindowFunction =
            keywords_.retrieve<WindowFunction>("ReferenceWindowFunction", WindowFunction());
        if (referenceWindowFunction.function() == WindowFunction::NoWindow)
            Messenger::print("No window function will be applied in Fourier transform of S(Q) to g(r).");
        else
            Messenger::print("Window function to be applied in Fourier transform of reference data is {} ({}).",
                             WindowFunction::functionType(referenceWindowFunction.function()),
                             referenceWindowFunction.parameterSummary());

        // Store the reference data in processing
        referenceData.setName(uniqueName());
        Data1D &storedData = GenericListHelper<Data1D>::realise(dissolve.processingModuleData(), "ReferenceData", uniqueName(),
                                                                GenericItem::ProtectedFlag);
        storedData.setObjectTag(fmt::format("{}//ReferenceData", uniqueName()));
        storedData = referenceData;

        // Calculate and store the FT of the reference data in processing
        referenceData.setName(uniqueName());
        Data1D &storedDataFT = GenericListHelper<Data1D>::realise(dissolve.processingModuleData(), "ReferenceDataFT",
                                                                  uniqueName(), GenericItem::ProtectedFlag);
        storedDataFT.setObjectTag(fmt::format("{}//ReferenceDataFT", uniqueName()));
        storedDataFT = referenceData;
        double rho = nTargetConfigurations() == 0 ? 0.1 : RDFModule::summedRho(this, dissolve.processingModuleData());
        Fourier::sineFT(storedDataFT, 1.0 / (2.0 * PI * PI * rho), 0.0, 0.05, 30.0, referenceWindowFunction);
        if (nTargetConfigurations() == 0)
            Messenger::warn("No configurations associated to module, so Fourier transform of reference data will use assumed "
                            "atomic density of 0.1.\n");

        // Save data?
        if (keywords_.asBool("SaveReference"))
        {
            if (procPool.isMaster())
            {
                Data1DExportFileFormat exportFormat(fmt::format("{}-ReferenceData.q", uniqueName()));
                if (!exportFormat.exportData(storedData))
                    return procPool.decideFalse();
                Data1DExportFileFormat exportFormatFT(fmt::format("{}-ReferenceData.r", uniqueName()));
                if (!exportFormatFT.exportData(storedDataFT))
                    return procPool.decideFalse();
                procPool.decideTrue();
            }
            else if (!procPool.decision())
                return false;
        }
    }

    return true;
}

// Run main processing
bool XRaySQModule::process(Dissolve &dissolve, ProcessPool &procPool)
{
    /*
     * Calculate x-ray structure factors from existing g(r) data
     *
     * This is a serial routine, with each process constructing its own copy of the data.
     * Partial calculation routines called by this routine are parallel.
     */

    const SQModule *sqModule = keywords_.retrieve<const SQModule *>("SourceSQs", nullptr);
    if (!sqModule)
        return Messenger::error("A source SQ module must be provided.\n");
    const RDFModule *rdfModule = sqModule->keywords().retrieve<const RDFModule *>("SourceRDFs", nullptr);
    if (!rdfModule)
        return Messenger::error("A source RDF module (in the SQ module) must be provided.\n");
    XRayFormFactors::XRayFormFactorData formFactors = keywords_.enumeration<XRayFormFactors::XRayFormFactorData>("FormFactors");
    auto normalisation = keywords_.enumeration<StructureFactors::NormalisationType>("Normalisation");
    const WindowFunction &referenceWindowFunction =
        keywords_.retrieve<WindowFunction>("ReferenceWindowFunction", WindowFunction());
    const bool saveFormFactors = keywords_.asBool("SaveFormFactors");
    const bool saveSQ = keywords_.asBool("SaveSQ");

    // Print argument/parameter summary
    Messenger::print("XRaySQ: Source unweighted S(Q) will be taken from module '{}'.\n", sqModule->uniqueName());
    Messenger::print("XRaySQ: Form factors to use are '{}'.\n", XRayFormFactors::xRayFormFactorData().keyword(formFactors));
    if (normalisation == StructureFactors::NoNormalisation)
        Messenger::print("XRaySQ: No normalisation will be applied to total F(Q).\n");
    else if (normalisation == StructureFactors::AverageOfSquaresNormalisation)
        Messenger::print("XRaySQ: Total F(Q) will be normalised to <b>**2");
    else if (normalisation == StructureFactors::SquareOfAverageNormalisation)
        Messenger::print("XRaySQ: Total F(Q) will be normalised to <b**2>");
    if (referenceWindowFunction.function() == WindowFunction::NoWindow)
        Messenger::print("XRaySQ: No window function will be applied when calculating representative g(r) from S(Q).");
    else
        Messenger::print("XRaySQ: Window function to be applied when calculating representative g(r) from S(Q) is {} ({}).",
                         WindowFunction::functionType(referenceWindowFunction.function()),
                         referenceWindowFunction.parameterSummary());
    if (saveFormFactors)
        Messenger::print("XRaySQ: Combined form factor weightings for atomtype pairs will be saved.\n");
    if (saveSQ)
        Messenger::print("XRaySQ: Weighted partial S(Q) and total F(Q) will be saved.\n");
    Messenger::print("\n");

    /*
     * Transform UnweightedSQ from provided SQ data into WeightedSQ.
     */

    bool created;

    // Get unweighted S(Q) from the specified SQMOdule
    if (!dissolve.processingModuleData().contains("UnweightedSQ", sqModule->uniqueName()))
        return Messenger::error("Couldn't locate unweighted S(Q) data from the SQModule '{}'.\n", sqModule->uniqueName());
    const auto &unweightedSQ =
        GenericListHelper<PartialSet>::value(dissolve.processingModuleData(), "UnweightedSQ", sqModule->uniqueName());

    // Construct weights matrix
    auto &weights = GenericListHelper<XRayWeights>::realise(dissolve.processingModuleData(), "FullWeights", uniqueName_,
                                                            GenericItem::InRestartFileFlag);
    calculateWeights(rdfModule, weights, formFactors);
    Messenger::print("Weights matrix:\n\n");
    weights.print();

    // Does a PartialSet for the unweighted S(Q) already exist for this Configuration?
    PartialSet &weightedSQ = GenericListHelper<PartialSet>::realise(dissolve.processingModuleData(), "WeightedSQ", uniqueName_,
                                                                    GenericItem::InRestartFileFlag, &created);
    if (created)
        weightedSQ.setUpPartials(unweightedSQ.atomTypes(), uniqueName(), "weighted", "sq", "Q, 1/Angstroms");

    // Calculate weighted S(Q)
    calculateWeightedSQ(unweightedSQ, weightedSQ, weights, normalisation);

    // Set names of resources (Data1D) within the PartialSet
    weightedSQ.setObjectTags(fmt::format("{}//{}", uniqueName_, "WeightedSQ"));

    // Save data if requested
    if (saveSQ && (!MPIRunMaster(procPool, weightedSQ.save())))
        return false;
    if (saveFormFactors)
    {
        auto result = for_each_pair_early(
            unweightedSQ.atomTypes().begin(), unweightedSQ.atomTypes().end(),
            [&](int i, auto &at1, int j, auto &at2) -> EarlyReturn<bool> {
                if (i == j)
                {
                    if (procPool.isMaster())
                    {
                        Data1D atomicData = unweightedSQ.partial(i, i);
                        atomicData.values() = weights.formFactor(i, atomicData.xAxis());
                        Data1DExportFileFormat exportFormat(fmt::format("{}-{}.form", uniqueName(), at1.atomTypeName()));
                        if (!exportFormat.exportData(atomicData))
                            return procPool.decideFalse();
                        procPool.decideTrue();
                    }
                    else if (!procPool.decision())
                        return false;
                }

                if (procPool.isMaster())
                {
                    Data1D ffData = unweightedSQ.partial(i, j);
                    ffData.values() = weights.weight(i, j, ffData.xAxis());
                    Data1DExportFileFormat exportFormat(
                        fmt::format("{}-{}-{}.form", uniqueName(), at1.atomTypeName(), at2.atomTypeName()));
                    if (!exportFormat.exportData(ffData))
                        return procPool.decideFalse();
                    procPool.decideTrue();
                }
                else if (!procPool.decision())
                    return false;

                return EarlyReturn<bool>::Continue;
            });

        if (!result.value_or(true))
            return Messenger::error("Failed to save form factor data.");
    }

    /*
     * Transform UnweightedGR from underlying RDF data into WeightedGR.
     */

    // Get summed unweighted g(r) from the specified RDFMOdule
    if (!dissolve.processingModuleData().contains("UnweightedGR", rdfModule->uniqueName()))
        return Messenger::error("Couldn't locate summed unweighted g(r) data.\n");
    const auto &unweightedGR =
        GenericListHelper<PartialSet>::value(dissolve.processingModuleData(), "UnweightedGR", rdfModule->uniqueName());

    // Create/retrieve PartialSet for summed weighted g(r)
    auto &weightedGR = GenericListHelper<PartialSet>::realise(dissolve.processingModuleData(), "WeightedGR", uniqueName_,
                                                              GenericItem::InRestartFileFlag, &created);
    if (created)
        weightedGR.setUpPartials(unweightedSQ.atomTypes(), uniqueName_, "weighted", "gr", "r, Angstroms");
    weightedGR.setObjectTags(fmt::format("{}//{}", uniqueName_, "WeightedGR"));

    // Calculate weighted g(r)
    calculateWeightedGR(unweightedGR, weightedGR, weights, normalisation);

    // Calculate representative total g(r) from FT of calculated S(Q)
    auto &repGR = GenericListHelper<Data1D>::realise(dissolve.processingModuleData(), "RepresentativeTotalGR", uniqueName_,
                                                     GenericItem::InRestartFileFlag);
    repGR = weightedSQ.total();
    auto rMin = weightedGR.total().xAxis().front();
    auto rMax = weightedGR.total().xAxis().back();
    auto rho = 0.1;
    if (dissolve.processingModuleData().contains("EffectiveRho", rdfModule->uniqueName()))
        rho = GenericListHelper<double>::value(dissolve.processingModuleData(), "EffectiveRho", rdfModule->uniqueName());
    else
        Messenger::warn("Couldn't locate effective atomic density for RDF module.\n");

    Fourier::sineFT(repGR, 1.0 / (2.0 * PI * PI * rho), rMin, 0.05, rMax, referenceWindowFunction);
    repGR.setObjectTag(fmt::format("{}//RepresentativeTotalGR", uniqueName_));

    // Save data if requested
    if (saveSQ && (!MPIRunMaster(procPool, weightedSQ.save())))
        return false;

    return true;
}
