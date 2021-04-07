// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/xrayweights.h"
#include "io/export/data1d.h"
#include "main/dissolve.h"
#include "math/ft.h"
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
                std::transform(bbar.begin(), bbar.end(), referenceData.values().begin(), referenceData.values().begin(),
                               [](auto b, auto ref) { return ref / b; });
            }
            else if (normType == StructureFactors::AverageOfSquaresNormalisation)
            {
                auto bbar = weights.boundCoherentAverageOfSquares(referenceData.xAxis());
                std::transform(bbar.begin(), bbar.end(), referenceData.values().begin(), referenceData.values().begin(),
                               [](auto b, auto ref) { return ref / b; });
            }
        }

        // Get window function to use for transformation of reference S(Q) to g(r)
        const auto wf = keywords_.enumeration<WindowFunction::Form>("ReferenceWindowFunction");
        if (wf == WindowFunction::Form::None)
            Messenger::print("No window function will be applied in Fourier transform of reference data to g(r).");
        else
            Messenger::print("Window function to be applied in Fourier transform of reference data is {}.",
                             WindowFunction::forms().keyword(wf));

        // Store the reference data in processing
        referenceData.setTag(uniqueName());
        auto &storedData =
            dissolve.processingModuleData().realise<Data1D>("ReferenceData", uniqueName(), GenericItem::ProtectedFlag);
        storedData = referenceData;

        // Calculate and store the FT of the reference data in processing
        referenceData.setTag(uniqueName());
        auto &storedDataFT =
            dissolve.processingModuleData().realise<Data1D>("ReferenceDataFT", uniqueName(), GenericItem::ProtectedFlag);
        storedDataFT = referenceData;
        auto rho = rdfModule->effectiveDensity();
        Messenger::print("Effective atomic density used in Fourier transform of reference data is {} atoms/Angstrom3.\n", rho);
        Fourier::sineFT(storedDataFT, 1.0 / (2.0 * PI * PI * rho), 0.0, 0.05, 30.0, WindowFunction(wf));

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
    const auto rwf = keywords_.enumeration<WindowFunction::Form>("ReferenceWindowFunction");
    const bool saveFormFactors = keywords_.asBool("SaveFormFactors");
    const bool saveGR = keywords_.asBool("SaveGR");
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
    if (rwf == WindowFunction::Form::None)
        Messenger::print("XRaySQ: No window function will be applied when calculating representative g(r) from S(Q).");
    else
        Messenger::print("XRaySQ: Window function to be applied when calculating representative g(r) from S(Q) is {}.",
                         WindowFunction::forms().keyword(rwf));
    if (saveFormFactors)
        Messenger::print("XRaySQ: Combined form factor weightings for atomtype pairs will be saved.\n");
    if (saveSQ)
        Messenger::print("XRaySQ: Weighted partial S(Q) and total F(Q) will be saved.\n");
    if (saveGR)
        Messenger::print("XRaySQ: Weighted partial g(r) and total G(r) will be saved.\n");
    Messenger::print("\n");

    /*
     * Transform UnweightedSQ from provided SQ data into WeightedSQ.
     */

    // Get unweighted S(Q) from the specified SQMOdule
    if (!dissolve.processingModuleData().contains("UnweightedSQ", sqModule->uniqueName()))
        return Messenger::error("Couldn't locate unweighted S(Q) data from the SQModule '{}'.\n", sqModule->uniqueName());
    const auto &unweightedSQ = dissolve.processingModuleData().value<PartialSet>("UnweightedSQ", sqModule->uniqueName());

    // Construct weights matrix
    auto &weights =
        dissolve.processingModuleData().realise<XRayWeights>("FullWeights", uniqueName_, GenericItem::InRestartFileFlag);
    calculateWeights(rdfModule, weights, formFactors);
    Messenger::print("Weights matrix:\n\n");
    weights.print();

    // Does a PartialSet for the unweighted S(Q) already exist for this Configuration?
    auto [weightedSQ, wSQtatus] =
        dissolve.processingModuleData().realiseIf<PartialSet>("WeightedSQ", uniqueName_, GenericItem::InRestartFileFlag);
    if (wSQtatus == GenericItem::ItemStatus::Created)
        weightedSQ.setUpPartials(unweightedSQ.atomTypes());

    // Calculate weighted S(Q)
    calculateWeightedSQ(unweightedSQ, weightedSQ, weights, normalisation);

    // Save data if requested
    if (saveSQ && (!MPIRunMaster(procPool, weightedSQ.save(uniqueName_, "WeightedSQ", "sq", "Q, 1/Angstroms"))))
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
    const auto &unweightedGR = dissolve.processingModuleData().value<PartialSet>("UnweightedGR", rdfModule->uniqueName());

    // Create/retrieve PartialSet for summed weighted g(r)
    auto [weightedGR, wGRstatus] =
        dissolve.processingModuleData().realiseIf<PartialSet>("WeightedGR", uniqueName_, GenericItem::InRestartFileFlag);
    if (wGRstatus == GenericItem::ItemStatus::Created)
        weightedGR.setUpPartials(unweightedSQ.atomTypes());

    // Calculate weighted g(r)
    calculateWeightedGR(unweightedGR, weightedGR, weights, normalisation);

    // Calculate representative total g(r) from FT of calculated S(Q)
    auto &repGR =
        dissolve.processingModuleData().realise<Data1D>("RepresentativeTotalGR", uniqueName_, GenericItem::InRestartFileFlag);
    repGR = weightedSQ.total();
    auto rMin = weightedGR.total().xAxis().front();
    auto rMax = weightedGR.total().xAxis().back();
    auto rho = rdfModule->effectiveDensity();
    Fourier::sineFT(repGR, 1.0 / (2.0 * PI * PI * rho), rMin, 0.05, rMax, WindowFunction(rwf));

    // Save data if requested
    if (saveSQ && (!MPIRunMaster(procPool, weightedSQ.save(uniqueName_, "WeightedGR", "sq", "Q, 1/Angstroms"))))
        return false;

    return true;
}
