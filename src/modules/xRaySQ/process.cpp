// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/xRayWeights.h"
#include "io/export/data1D.h"
#include "main/dissolve.h"
#include "math/filters.h"
#include "math/ft.h"
#include "modules/gr/gr.h"
#include "modules/sq/sq.h"
#include "modules/xRaySQ/xRaySQ.h"
#include "templates/algorithms.h"

// Set target data
void XRaySQModule::setTargets(const std::vector<std::unique_ptr<Configuration>> &configurations,
                              const std::map<ModuleTypes::ModuleType, std::vector<const Module *>> &moduleMap)
{
    auto sqIt = moduleMap.find(ModuleTypes::SQ);
    if (sqIt != moduleMap.end())
        sourceSQ_ = dynamic_cast<const SQModule *>(sqIt->second.front());
}

// Run set-up stage
bool XRaySQModule::setUp(Dissolve &dissolve, const ProcessPool &procPool, Flags<KeywordBase::KeywordSignal> actionSignals)
{
    /*
     * Load and set up reference data (if a file/format was given)
     */
    if (referenceFQ_.hasFilename() && actionSignals.isSetOrNone(KeywordBase::ReloadExternalData))
    {
        // Load the data
        Data1D referenceData;
        if (!referenceFQ_.importData(referenceData, &procPool))
            return Messenger::error("[SETUP {}] Failed to load reference data '{}'.\n", name_, referenceFQ_.filename());

        // Get dependent modules
        if (!sourceSQ_)
            return Messenger::error("[SETUP {}] A source SQ module must be provided.\n", name_);
        auto *grModule = sourceSQ_->sourceGR();
        if (!grModule)
            return Messenger::error("[SETUP {}] A source GR module (in the SQ module) must be provided.\n", name_);

        // Normalise reference data to be consistent with the calculated data
        if (referenceNormalisedTo_ != normaliseTo_)
        {
            // We need the x-ray weights in order to do the normalisation
            XRayWeights weights;
            calculateWeights(grModule, weights, formFactors_);
            auto bBarSquareOfAverage = weights.boundCoherentSquareOfAverage(referenceData.xAxis());
            auto bBarAverageOfSquares = weights.boundCoherentAverageOfSquares(referenceData.xAxis());
            std::vector<double> factors;

            // Set up the multiplication factors
            switch (referenceNormalisedTo_)
            {
                case (StructureFactors::NoNormalisation):
                    factors = normaliseTo_ == StructureFactors::SquareOfAverageNormalisation ? bBarSquareOfAverage
                                                                                             : bBarAverageOfSquares;
                    std::transform(factors.begin(), factors.end(), factors.begin(),
                                   [](const auto factor) { return 1.0 / factor; });
                    break;
                case (StructureFactors::SquareOfAverageNormalisation):
                    factors = bBarSquareOfAverage;
                    if (normaliseTo_ == StructureFactors::AverageOfSquaresNormalisation)
                        std::transform(factors.begin(), factors.end(), bBarAverageOfSquares.begin(), factors.begin(),
                                       std::divides<>());
                    break;
                case (StructureFactors::AverageOfSquaresNormalisation):
                    factors = bBarAverageOfSquares;
                    if (normaliseTo_ == StructureFactors::SquareOfAverageNormalisation)
                        std::transform(factors.begin(), factors.end(), bBarSquareOfAverage.begin(), factors.begin(),
                                       std::divides<>());
                    break;
                default:
                    throw(std::runtime_error(
                        fmt::format("Unhandled StructureFactor::NormalisationType ({}).\n",
                                    StructureFactors::normalisationTypes().keyword(referenceNormalisedTo_))));
            }

            // Apply normalisation factors to the data
            std::transform(referenceData.values().begin(), referenceData.values().end(), factors.begin(),
                           referenceData.values().begin(), std::multiplies<>());
        }

        // Get Q-range and window function to use for transformation of F(Q) to G(r)
        auto ftQMin = referenceFTQMin_.value_or(0.0);
        auto ftQMax = referenceFTQMax_.value_or(referenceData.xAxis().back() + 1.0);
        if (referenceWindowFunction_ == WindowFunction::Form::None)
            Messenger::print("[SETUP {}] No window function will be applied in Fourier transform of reference data to g(r).",
                             name_);
        else
            Messenger::print("[SETUP {}] Window function to be applied in Fourier transform of reference data is {}.", name_,
                             WindowFunction::forms().keyword(referenceWindowFunction_));

        // Store the reference data in processing
        referenceData.setTag(name());
        auto &storedData = dissolve.processingModuleData().realise<Data1D>("ReferenceData", name(), GenericItem::ProtectedFlag);
        storedData = referenceData;

        // Calculate and store the FT of the reference data in processing
        referenceData.setTag(name());
        auto &storedDataFT =
            dissolve.processingModuleData().realise<Data1D>("ReferenceDataFT", name(), GenericItem::ProtectedFlag);
        storedDataFT = referenceData;
        Filters::trim(storedDataFT, ftQMin, ftQMax);
        auto rho = grModule->effectiveDensity();
        if (rho)
            Messenger::print(
                "[SETUP {}] Effective atomic density used in Fourier transform of reference data is {} atoms/Angstrom3.\n",
                name_, rho.value());
        else
            Messenger::warn("[SETUP {}] Effective atomic density used in Fourier transform of reference data not yet "
                            "available, so a default of 0.1 atoms/Angstrom3 used.\n",
                            name_);
        Fourier::sineFT(storedDataFT, 1.0 / (2.0 * PI * PI * rho.value_or(0.1)), referenceFTDeltaR_, referenceFTDeltaR_, 30.0,
                        WindowFunction(referenceWindowFunction_));

        // Save data?
        if (saveReference_)
        {
            if (procPool.isMaster())
            {
                Data1DExportFileFormat exportFormat(fmt::format("{}-ReferenceData.q", name()));
                if (!exportFormat.exportData(storedData))
                    return procPool.decideFalse();
                Data1DExportFileFormat exportFormatFT(fmt::format("{}-ReferenceData.r", name()));
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
enum Module::executionResult XRaySQModule::process(Dissolve &dissolve, const ProcessPool &procPool)
{
    /*
     * Calculate x-ray structure factors from existing g(r) data
     *
     * This is a serial routine, with each process constructing its own copy of the data.
     * Partial calculation routines called by this routine are parallel.
     */

    if (!sourceSQ_)
    {
        Messenger::error("A source SQ module must be provided.\n");
        return failed;
    }
    auto *grModule = sourceSQ_->sourceGR();
    if (!grModule)
    {
        Messenger::error("A source GR module (in the SQ module) must be provided.\n");
        return failed;
    }

    // Print argument/parameter summary
    Messenger::print("XRaySQ: Source unweighted S(Q) will be taken from module '{}'.\n", sourceSQ_->name());
    Messenger::print("XRaySQ: Form factors to use are '{}'.\n", XRayFormFactors::xRayFormFactorData().keyword(formFactors_));
    if (normaliseTo_ == StructureFactors::NoNormalisation)
        Messenger::print("XRaySQ: No normalisation will be applied to total F(Q).\n");
    else if (normaliseTo_ == StructureFactors::AverageOfSquaresNormalisation)
        Messenger::print("XRaySQ: Total F(Q) will be normalised to <b**2>");
    else if (normaliseTo_ == StructureFactors::SquareOfAverageNormalisation)
        Messenger::print("XRaySQ: Total F(Q) will be normalised to <b>**2");
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
    if (!dissolve.processingModuleData().contains("UnweightedSQ", sourceSQ_->name()))
    {
        Messenger::error("Couldn't locate unweighted S(Q) data from the SQModule '{}'.\n", sourceSQ_->name());
        return failed;
    }
    const auto &unweightedSQ = dissolve.processingModuleData().value<PartialSet>("UnweightedSQ", sourceSQ_->name());

    // Construct weights matrix
    auto &weights = dissolve.processingModuleData().realise<XRayWeights>("FullWeights", name_, GenericItem::InRestartFileFlag);
    calculateWeights(grModule, weights, formFactors_);
    Messenger::print("Weights matrix:\n\n");
    weights.print();

    // Does a PartialSet for the unweighted S(Q) already exist for this Configuration?
    auto [weightedSQ, wSQtatus] =
        dissolve.processingModuleData().realiseIf<PartialSet>("WeightedSQ", name_, GenericItem::InRestartFileFlag);
    if (wSQtatus == GenericItem::ItemStatus::Created)
        weightedSQ.setUpPartials(unweightedSQ.atomTypeMix());

    // Calculate weighted S(Q)
    calculateWeightedSQ(unweightedSQ, weightedSQ, weights, normaliseTo_);

    // Save data if requested
    if (saveSQ_ && (!MPIRunMaster(procPool, weightedSQ.save(name_, "WeightedSQ", "sq", "Q, 1/Angstroms"))))
        return failed;
    if (saveFormFactors_)
    {
        auto result = for_each_pair_early(
            unweightedSQ.atomTypeMix().begin(), unweightedSQ.atomTypeMix().end(),
            [&](int i, auto &at1, int j, auto &at2) -> EarlyReturn<bool>
            {
                if (i == j)
                {
                    if (procPool.isMaster())
                    {
                        Data1D atomicData = unweightedSQ.partial(i, i);
                        atomicData.values() = weights.formFactor(i, atomicData.xAxis());
                        Data1DExportFileFormat exportFormat(fmt::format("{}-{}.form", name(), at1.atomTypeName()));
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
                        fmt::format("{}-{}-{}.form", name(), at1.atomTypeName(), at2.atomTypeName()));
                    if (!exportFormat.exportData(ffData))
                        return procPool.decideFalse();
                    procPool.decideTrue();
                }
                else if (!procPool.decision())
                    return false;

                return EarlyReturn<bool>::Continue;
            });

        if (!result.value_or(true))
        {
            Messenger::error("Failed to save form factor data.");
            return failed;
        }
    }

    /*
     * Transform UnweightedGR from underlying RDF data into WeightedGR.
     */

    // Get summed unweighted g(r) from the specified RDFMOdule
    if (!dissolve.processingModuleData().contains("UnweightedGR", grModule->name()))
    {
        Messenger::error("Couldn't locate summed unweighted g(r) data.\n");
        return failed;
    }
    const auto &unweightedGR = dissolve.processingModuleData().value<PartialSet>("UnweightedGR", grModule->name());

    // Create/retrieve PartialSet for summed weighted g(r)
    auto [weightedGR, wGRstatus] =
        dissolve.processingModuleData().realiseIf<PartialSet>("WeightedGR", name_, GenericItem::InRestartFileFlag);
    if (wGRstatus == GenericItem::ItemStatus::Created)
        weightedGR.setUpPartials(unweightedSQ.atomTypeMix());

    // Calculate weighted g(r)
    calculateWeightedGR(unweightedGR, weightedGR, weights, normaliseTo_);

    // Calculate representative total g(r) from FT of calculated F(Q)
    auto &repGR =
        dissolve.processingModuleData().realise<Data1D>("RepresentativeTotalGR", name_, GenericItem::InRestartFileFlag);
    repGR = weightedSQ.total();
    auto ftQMax = 0.0;
    if (referenceFTQMax_)
        ftQMax = referenceFTQMax_.value();
    else if (referenceFQ_.hasFilename())
    {
        // Take FT max Q limit from reference data
        auto &referenceData =
            dissolve.processingModuleData().realise<Data1D>("ReferenceData", name(), GenericItem::ProtectedFlag);
        ftQMax = referenceData.xAxis().back();
    }
    else
        ftQMax = weightedSQ.total().xAxis().back();
    Filters::trim(repGR, referenceFTQMin_.value_or(0.0), ftQMax);
    auto rMin = weightedGR.total().xAxis().front();
    auto rMax = weightedGR.total().xAxis().back();
    auto rho = grModule->effectiveDensity();
    if (!rho)
    {
        Messenger::error("No effective density available from RDF module '{}'\n", grModule->name());
        return failed;
    }
    Fourier::sineFT(repGR, 1.0 / (2.0 * PI * PI * *rho), rMin, 0.05, rMax, WindowFunction(referenceWindowFunction_));

    // Save data if requested
    if (saveRepresentativeGR_)
    {
        if (procPool.isMaster())
        {
            Data1DExportFileFormat exportFormat(fmt::format("{}-weighted-total.gr.broad", name_));
            if (exportFormat.exportData(repGR))
                procPool.decideTrue();
            else
                procPool.decideFalse();
        }
        else if (!procPool.decision())
            return failed;
    }

    return success;
}
