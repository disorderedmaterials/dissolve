// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/xrayweights.h"
#include "io/export/data1d.h"
#include "main/dissolve.h"
#include "math/filters.h"
#include "math/ft.h"
#include "modules/rdf/rdf.h"
#include "modules/sq/sq.h"
#include "modules/xraysq/xraysq.h"
#include "templates/algorithms.h"

// Set target data
void XRaySQModule::setTargets(std::vector<std::unique_ptr<Configuration>> &configurations,
                              const std::map<std::string, std::vector<const Module *>> &moduleMap)
{
    auto sqIt = moduleMap.find("SQ");
    if (sqIt != moduleMap.end())
        sourceSQ_ = dynamic_cast<const SQModule *>(sqIt->second.front());
}

// Run set-up stage
bool XRaySQModule::setUp(Dissolve &dissolve, ProcessPool &procPool, KeywordSignals actionSignals)
{
    /*
     * Load and set up reference data (if a file/format was given)
     */
    if (referenceFQ_.hasFilename() && actionSignals.isSetOrNone(KeywordSignals::ReloadExternalData))
    {
        // Load the data
        Data1D referenceData;
        if (!referenceFQ_.importData(referenceData, &procPool))
        {
            Messenger::error("[SETUP {}] Failed to load reference data '{}'.\n", uniqueName_, referenceFQ_.filename());
            return false;
        }

        // Get dependent modules
        if (!sourceSQ_)
            return Messenger::error("[SETUP {}] A source SQ module must be provided.\n", uniqueName_);
        auto *rdfModule = sourceSQ_->sourceRDF();
        if (!rdfModule)
            return Messenger::error("[SETUP {}] A source RDF module (in the SQ module) must be provided.\n", uniqueName_);

        // Remove normalisation factor from data
        if (referenceNormalisation_ != StructureFactors::NoNormalisation)
        {
            // We need the x-ray weights in order to do the normalisation
            XRayWeights weights;
            calculateWeights(rdfModule, weights, formFactors_);

            // Remove normalisation from the data
            if (referenceNormalisation_ == StructureFactors::SquareOfAverageNormalisation)
            {
                auto bbar = weights.boundCoherentSquareOfAverage(referenceData.xAxis());
                std::transform(bbar.begin(), bbar.end(), referenceData.values().begin(), referenceData.values().begin(),
                               [](auto b, auto ref) { return ref / b; });
            }
            else if (referenceNormalisation_ == StructureFactors::AverageOfSquaresNormalisation)
            {
                auto bbar = weights.boundCoherentAverageOfSquares(referenceData.xAxis());
                std::transform(bbar.begin(), bbar.end(), referenceData.values().begin(), referenceData.values().begin(),
                               [](auto b, auto ref) { return ref / b; });
            }
        }

        // Get Q-range and window function to use for transformation of F(Q) to G(r)
        auto ftQMin = referenceFTQMin_.value_or(0.0);
        auto ftQMax = referenceFTQMax_.value_or(referenceData.xAxis().back() + 1.0);
        if (referenceWindowFunction_ == WindowFunction::Form::None)
            Messenger::print("[SETUP {}] No window function will be applied in Fourier transform of reference data to g(r).",
                             uniqueName_);
        else
            Messenger::print("[SETUP {}] Window function to be applied in Fourier transform of reference data is {}.",
                             uniqueName_, WindowFunction::forms().keyword(referenceWindowFunction_));

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
        Filters::trim(storedDataFT, ftQMin, ftQMax);
        auto rho = rdfModule->effectiveDensity();
        Messenger::print(
            "[SETUP {}] Effective atomic density used in Fourier transform of reference data is {} atoms/Angstrom3.\n",
            uniqueName_, rho);
        Fourier::sineFT(storedDataFT, 1.0 / (2.0 * PI * PI * rho), referenceFTDeltaR_, referenceFTDeltaR_, 30.0,
                        WindowFunction(referenceWindowFunction_));

        // Save data?
        if (saveReference_)
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

    if (!sourceSQ_)
        return Messenger::error("A source SQ module must be provided.\n");
    auto *rdfModule = sourceSQ_->sourceRDF();
    if (!rdfModule)
        return Messenger::error("A source RDF module (in the SQ module) must be provided.\n");

    // Print argument/parameter summary
    Messenger::print("XRaySQ: Source unweighted S(Q) will be taken from module '{}'.\n", sourceSQ_->uniqueName());
    Messenger::print("XRaySQ: Form factors to use are '{}'.\n", XRayFormFactors::xRayFormFactorData().keyword(formFactors_));
    if (normalisation_ == StructureFactors::NoNormalisation)
        Messenger::print("XRaySQ: No normalisation will be applied to total F(Q).\n");
    else if (normalisation_ == StructureFactors::AverageOfSquaresNormalisation)
        Messenger::print("XRaySQ: Total F(Q) will be normalised to <b>**2");
    else if (normalisation_ == StructureFactors::SquareOfAverageNormalisation)
        Messenger::print("XRaySQ: Total F(Q) will be normalised to <b**2>");
    if (referenceWindowFunction_ == WindowFunction::Form::None)
        Messenger::print("XRaySQ: No window function will be applied when calculating representative g(r) from S(Q).");
    else
        Messenger::print("XRaySQ: Window function to be applied when calculating representative g(r) from S(Q) is {}.",
                         WindowFunction::forms().keyword(referenceWindowFunction_));
    if (saveFormFactors_)
        Messenger::print("XRaySQ: Combined form factor weightings for atomtype pairs will be saved.\n");
    if (saveSQ_)
        Messenger::print("XRaySQ: Weighted partial S(Q) and total F(Q) will be saved.\n");
    if (saveGR_)
        Messenger::print("XRaySQ: Weighted partial g(r) and total G(r) will be saved.\n");
    if (saveRepresentativeGR_)
        Messenger::print("XRaySQ: Representative G(r) will be saved.\n");
    Messenger::print("\n");

    /*
     * Transform UnweightedSQ from provided SQ data into WeightedSQ.
     */

    // Get unweighted S(Q) from the specified SQMOdule
    if (!dissolve.processingModuleData().contains("UnweightedSQ", sourceSQ_->uniqueName()))
        return Messenger::error("Couldn't locate unweighted S(Q) data from the SQModule '{}'.\n", sourceSQ_->uniqueName());
    const auto &unweightedSQ = dissolve.processingModuleData().value<PartialSet>("UnweightedSQ", sourceSQ_->uniqueName());

    // Construct weights matrix
    auto &weights =
        dissolve.processingModuleData().realise<XRayWeights>("FullWeights", uniqueName_, GenericItem::InRestartFileFlag);
    calculateWeights(rdfModule, weights, formFactors_);
    Messenger::print("Weights matrix:\n\n");
    weights.print();

    // Does a PartialSet for the unweighted S(Q) already exist for this Configuration?
    auto [weightedSQ, wSQtatus] =
        dissolve.processingModuleData().realiseIf<PartialSet>("WeightedSQ", uniqueName_, GenericItem::InRestartFileFlag);
    if (wSQtatus == GenericItem::ItemStatus::Created)
        weightedSQ.setUpPartials(unweightedSQ.atomTypeMix());

    // Calculate weighted S(Q)
    calculateWeightedSQ(unweightedSQ, weightedSQ, weights, normalisation_);

    // Save data if requested
    if (saveSQ_ && (!MPIRunMaster(procPool, weightedSQ.save(uniqueName_, "WeightedSQ", "sq", "Q, 1/Angstroms"))))
        return false;
    if (saveFormFactors_)
    {
        auto result = for_each_pair_early(
            unweightedSQ.atomTypeMix().begin(), unweightedSQ.atomTypeMix().end(),
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
        weightedGR.setUpPartials(unweightedSQ.atomTypeMix());

    // Calculate weighted g(r)
    calculateWeightedGR(unweightedGR, weightedGR, weights, normalisation_);

    // Calculate representative total g(r) from FT of calculated F(Q)
    auto &repGR =
        dissolve.processingModuleData().realise<Data1D>("RepresentativeTotalGR", uniqueName_, GenericItem::InRestartFileFlag);
    repGR = weightedSQ.total();
    auto ftQMax = 0.0;
    if (referenceFTQMax_)
        ftQMax = referenceFTQMax_.value();
    else if (referenceFQ_.hasFilename())
    {
        // Take FT max Q limit from reference data
        auto &referenceData =
            dissolve.processingModuleData().realise<Data1D>("ReferenceData", uniqueName(), GenericItem::ProtectedFlag);
        ftQMax = referenceData.xAxis().back();
    }
    else
        ftQMax = weightedSQ.total().xAxis().back();
    Filters::trim(repGR, referenceFTQMin_.value_or(0.0), ftQMax);
    auto rMin = weightedGR.total().xAxis().front();
    auto rMax = weightedGR.total().xAxis().back();
    auto rho = rdfModule->effectiveDensity();
    Fourier::sineFT(repGR, 1.0 / (2.0 * PI * PI * rho), rMin, 0.05, rMax, WindowFunction(referenceWindowFunction_));

    // Save data if requested
    if (saveRepresentativeGR_)
    {
        if (procPool.isMaster())
        {
            Data1DExportFileFormat exportFormat(fmt::format("{}-weighted-total.gr.broad", uniqueName_));
            if (exportFormat.exportData(repGR))
                procPool.decideTrue();
            else
                procPool.decideFalse();
        }
        else if (!procPool.decision())
            return false;
    }

    return true;
}
