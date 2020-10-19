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

        //         // Truncate data beyond QMax
        //         const double qMax = keywords_.asDouble("QMax") < 0.0 ? 30.0 : keywords_.asDouble("QMax");
        //         if (referenceData.constXAxis().lastValue() < qMax)
        //             Messenger::warn(
        //                 "Qmax limit of {:.4e} Angstroms**-1 for calculated XRaySQ ({}) is beyond limit of reference data
        //                 (Qmax "
        //                 "= {:.4e} Angstroms**-1).\n",
        //                 qMax, uniqueName(), referenceData.constXAxis().lastValue());
        //         else
        //             while (referenceData.constXAxis().lastValue() > qMax)
        //                 referenceData.removeLastPoint();

        // Remove first point?
        if (keywords_.asBool("ReferenceIgnoreFirst"))
        {
            referenceData.removeFirstPoint();
            Messenger::print("Removed first point from supplied reference data - new Qmin = {:.4e} Angstroms**-1.\n",
                             referenceData.constXAxis().firstValue());
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
    XRaySQModule::NormalisationType normalisation = keywords_.enumeration<XRaySQModule::NormalisationType>("Normalisation");
    const WindowFunction &referenceWindowFunction =
        keywords_.retrieve<WindowFunction>("ReferenceWindowFunction", WindowFunction());
    const bool saveFormFactors = keywords_.asBool("SaveFormFactors");
    const bool saveSQ = keywords_.asBool("SaveSQ");

    // Print argument/parameter summary
    Messenger::print("XRaySQ: Source unweighted S(Q) will be taken from module '{}'.\n", sqModule->uniqueName());
    Messenger::print("XRaySQ: Form factors to use are '{}'.\n", XRayFormFactors::xRayFormFactorData().keyword(formFactors));
    if (normalisation == XRaySQModule::NoNormalisation)
        Messenger::print("XRaySQ: No normalisation will be applied to total F(Q).\n");
    else if (normalisation == XRaySQModule::AverageOfSquaresNormalisation)
        Messenger::print("XRaySQ: Total F(Q) will be normalised to <b>**2");
    else if (normalisation == XRaySQModule::SquareOfAverageNormalisation)
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

    // Construct weights matrix containing each Species in the Configuration in the correct proportion
    // TODO This info would be better calculated by the RDFModule and stored there / associated to it (#400)
    // TODO Following code should exist locally in RDFModule::sumUnweightedGR() when suitable class storage is available.
    auto &weights = GenericListHelper<XRayWeights>::realise(dissolve.processingModuleData(), "FullWeights", uniqueName_,
                                                            GenericItem::InRestartFileFlag);
    weights.clear();
    for (auto *cfg : rdfModule->targetConfigurations())
    {
        // TODO Assume weight of 1.0 per configuration now, until #398/#400 are addressed.
        const auto CFGWEIGHT = 1.0;

        ListIterator<SpeciesInfo> spInfoIterator(cfg->usedSpecies());
        while (auto *spInfo = spInfoIterator.iterate())
            weights.addSpecies(spInfo->species(), spInfo->population() * CFGWEIGHT);
    }

    // Create, print, and store weights
    Messenger::print("Weights matrix:\n\n");
    weights.finalise(formFactors);
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
                        atomicData.values() = weights.formFactor(i, atomicData.constXAxis());
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
                    ffData.values() = weights.weight(i, j, ffData.constXAxis());
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
    auto qMin = weightedSQ.total().values().firstValue();
    auto qMax = weightedSQ.total().values().lastValue();
    double rho = nTargetConfigurations() == 0 ? 0.1 : RDFModule::summedRho(this, dissolve.processingModuleData());
    Fourier::sineFT(repGR, 1.0 / (2.0 * PI * PI * rho), qMin, 0.05, qMax, referenceWindowFunction);
    repGR.setObjectTag(fmt::format("{}//RepresentativeTotalGR", uniqueName_));

    // Save data if requested
    if (saveSQ && (!MPIRunMaster(procPool, weightedSQ.save())))
        return false;

    return true;
}
