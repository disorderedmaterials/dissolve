// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/neutronWeights.h"
#include "classes/species.h"
#include "io/export/data1D.h"
#include "main/dissolve.h"
#include "math/filters.h"
#include "math/ft.h"
#include "module/context.h"
#include "modules/gr/gr.h"
#include "modules/neutronSQ/neutronSQ.h"
#include "modules/sq/sq.h"

// Set target data
void NeutronSQModule::setTargets(const std::vector<std::unique_ptr<Configuration>> &configurations,
                                 const std::map<ModuleTypes::ModuleType, std::vector<const Module *>> &moduleMap)
{
    auto sqIt = moduleMap.find(ModuleTypes::SQ);
    if (sqIt != moduleMap.end())
        sourceSQ_ = dynamic_cast<const SQModule *>(sqIt->second.front());
}

// Run set-up stage
bool NeutronSQModule::setUp(ModuleContext &moduleContext, Flags<KeywordBase::KeywordSignal> actionSignals)
{
    /*
     * Load and set up reference data (if a file/format was given)
     */
    if (referenceFQ_.hasFilename() && actionSignals.isSetOrNone(KeywordBase::ReloadExternalData))
    {
        // Load the data
        Data1D referenceData;
        if (!referenceFQ_.importData(referenceData, &moduleContext.processPool()))
            return Messenger::error("[SETUP {}] Failed to load reference data '{}'.\n", name_, referenceFQ_.filename());

        // Get dependent modules
        if (!sourceSQ_)
            return Messenger::error("[SETUP {}] A source SQ module must be provided.\n", name_);
        auto *rdfModule = sourceSQ_->sourceGR();
        if (!rdfModule)
            return Messenger::error("[SETUP {}] A source GR module (in the SQ module) must be provided.\n", name_);

        // Normalise reference data to be consistent with the calculated data
        if (referenceNormalisedTo_ != normaliseTo_)
        {
            // We need the neutron weights in order to do the normalisation
            NeutronWeights weights;
            calculateWeights(rdfModule, weights);
            auto factor = 1.0;

            // Set up the multiplication factors
            switch (referenceNormalisedTo_)
            {
                case (StructureFactors::NoNormalisation):
                    factor = 1.0 / normaliseTo_ == StructureFactors::SquareOfAverageNormalisation
                                 ? weights.boundCoherentSquareOfAverage()
                                 : weights.boundCoherentAverageOfSquares();
                    break;
                case (StructureFactors::SquareOfAverageNormalisation):
                    factor = weights.boundCoherentSquareOfAverage();
                    if (normaliseTo_ == StructureFactors::AverageOfSquaresNormalisation)
                        factor /= weights.boundCoherentAverageOfSquares();
                    break;
                case (StructureFactors::AverageOfSquaresNormalisation):
                    factor = weights.boundCoherentAverageOfSquares();
                    if (normaliseTo_ == StructureFactors::SquareOfAverageNormalisation)
                        factor /= weights.boundCoherentSquareOfAverage();
                    break;
                default:
                    throw(std::runtime_error(
                        fmt::format("Unhandled StructureFactor::NormalisationType ({}).\n",
                                    StructureFactors::normalisationTypes().keyword(referenceNormalisedTo_))));
            }

            // Apply normalisation factors to the data
            referenceData *= factor;
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
        auto &storedData = moduleContext.dissolve().processingModuleData().realise<Data1D>("ReferenceData", name(),
                                                                                           GenericItem::ProtectedFlag);
        storedData = referenceData;

        // Calculate and store the FT of the reference data in processing
        referenceData.setTag(name());
        auto &storedDataFT = moduleContext.dissolve().processingModuleData().realise<Data1D>("ReferenceDataFT", name(),
                                                                                             GenericItem::ProtectedFlag);
        storedDataFT = referenceData;
        Filters::trim(storedDataFT, ftQMin, ftQMax);
        auto rho = rdfModule->effectiveDensity();
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
            if (moduleContext.processPool().isMaster())
            {
                Data1DExportFileFormat exportFormat(fmt::format("{}-ReferenceData.q", name()));
                if (!exportFormat.exportData(storedData))
                    return moduleContext.processPool().decideFalse();
                Data1DExportFileFormat exportFormatFT(fmt::format("{}-ReferenceData.r", name()));
                if (!exportFormatFT.exportData(storedDataFT))
                    return moduleContext.processPool().decideFalse();
                moduleContext.processPool().decideTrue();
            }
            else if (!moduleContext.processPool().decision())
                return false;
        }
    }

    return true;
}

// Run main processing
Module::ExecutionResult NeutronSQModule::process(ModuleContext &moduleContext)
{
    /*
     * Calculate neutron structure factors from existing S(Q) data
     *
     * This is a serial routine, with each process constructing its own copy of the data.
     * Partial calculation routines called by this routine are parallel.
     */

    if (!sourceSQ_)
    {
        Messenger::error("A source SQ module must be provided.\n");
        return ExecutionResult::Failed;
    }

    const auto *rdfModule = sourceSQ_->sourceGR();
    if (!rdfModule)
    {
        Messenger::error("A source GR module (in the SQ module) must be provided.\n");
        return ExecutionResult::Failed;
    }

    // Print argument/parameter summary
    Messenger::print("NeutronSQ: Source unweighted S(Q) will be taken from module '{}'.\n", sourceSQ_->name());
    if (referenceWindowFunction_ == WindowFunction::Form::None)
        Messenger::print("No window function will be applied when calculating representative g(r) from S(Q).");
    else
        Messenger::print("Window function to be applied when calculating representative g(r) from S(Q) is {}.",
                         WindowFunction::forms().keyword(referenceWindowFunction_));
    if (normaliseTo_ == StructureFactors::NoNormalisation)
        Messenger::print("NeutronSQ: No normalisation will be applied to total F(Q).\n");
    else if (normaliseTo_ == StructureFactors::AverageOfSquaresNormalisation)
        Messenger::print("NeutronSQ: Total F(Q) will be normalised to <b**2>");
    else if (normaliseTo_ == StructureFactors::SquareOfAverageNormalisation)
        Messenger::print("NeutronSQ: Total F(Q) will be normalised to <b>**2");
    if (saveSQ_)
        Messenger::print("NeutronSQ: Weighted partial S(Q) and total F(Q) will be saved.\n");
    if (saveGR_)
        Messenger::print("NeutronSQ: Weighted partial g(r) and total G(r) will be saved.\n");
    if (saveRepresentativeGR_)
        Messenger::print("NeutronSQ: Representative G(r) will be saved.\n");
    Messenger::print("\n");

    /*
     * Transform UnweightedSQ from provided SQ data into WeightedSQ.
     */

    // Get unweighted S(Q) from the specified SQMOdule
    if (!moduleContext.dissolve().processingModuleData().contains("UnweightedSQ", sourceSQ_->name()))
    {
        Messenger::error("Couldn't locate unweighted S(Q) data from the SQModule '{}'.\n", sourceSQ_->name());
        return ExecutionResult::Failed;
    }
    const auto &unweightedSQ =
        moduleContext.dissolve().processingModuleData().value<PartialSet>("UnweightedSQ", sourceSQ_->name());

    // Calculate and store weights
    auto &weights = moduleContext.dissolve().processingModuleData().realise<NeutronWeights>("FullWeights", name_,
                                                                                            GenericItem::InRestartFileFlag);
    calculateWeights(rdfModule, weights);
    Messenger::print("Isotopologue and isotope composition:\n\n");
    weights.print();

    // Does a PartialSet for the weighted S(Q) already exist for this Configuration?
    auto [weightedSQ, wSQstatus] = moduleContext.dissolve().processingModuleData().realiseIf<PartialSet>(
        "WeightedSQ", name_, GenericItem::InRestartFileFlag);
    if (wSQstatus == GenericItem::ItemStatus::Created)
        weightedSQ.setUpPartials(unweightedSQ.atomTypeMix());

    // Calculate weighted S(Q)
    calculateWeightedSQ(unweightedSQ, weightedSQ, weights, normaliseTo_);

    // Save data if requested
    if (saveSQ_ && (!MPIRunMaster(moduleContext.processPool(), weightedSQ.save(name_, "WeightedSQ", "sq", "Q, 1/Angstroms"))))
        return ExecutionResult::Failed;

    /*
     * Transform UnweightedGR from underlying RDF data into WeightedGR.
     */

    // Get summed unweighted g(r) from the RDFMOdule
    if (!moduleContext.dissolve().processingModuleData().contains("UnweightedGR", rdfModule->name()))
    {
        Messenger::error("Couldn't locate summed unweighted g(r) data.\n");
        return ExecutionResult::Failed;
    }

    const auto &unweightedGR =
        moduleContext.dissolve().processingModuleData().value<PartialSet>("UnweightedGR", rdfModule->name());

    // Create/retrieve PartialSet for summed weighted g(r)
    auto [weightedGR, wGRstatus] = moduleContext.dissolve().processingModuleData().realiseIf<PartialSet>(
        "WeightedGR", name_, GenericItem::InRestartFileFlag);
    if (wGRstatus == GenericItem::ItemStatus::Created)
        weightedGR.setUpPartials(unweightedGR.atomTypeMix());

    // Calculate weighted g(r)
    calculateWeightedGR(unweightedGR, weightedGR, weights, normaliseTo_);

    // Save data if requested
    if (saveGR_ && (!MPIRunMaster(moduleContext.processPool(), weightedGR.save(name_, "WeightedGR", "gr", "r, Angstroms"))))
        return ExecutionResult::Failed;

    // Calculate representative total g(r) from FT of calculated F(Q)
    auto &repGR = moduleContext.dissolve().processingModuleData().realise<Data1D>("RepresentativeTotalGR", name_,
                                                                                  GenericItem::InRestartFileFlag);
    repGR = weightedSQ.total();
    auto ftQMax = 0.0;
    if (referenceFTQMax_)
        ftQMax = referenceFTQMax_.value();
    else if (referenceFQ_.hasFilename())
    {
        // Take FT max Q limit from reference data
        auto &referenceData = moduleContext.dissolve().processingModuleData().realise<Data1D>("ReferenceData", name(),
                                                                                              GenericItem::ProtectedFlag);
        ftQMax = referenceData.xAxis().back();
    }
    else
        ftQMax = weightedSQ.total().xAxis().back();
    Filters::trim(repGR, referenceFTQMin_.value_or(0.0), ftQMax);
    auto rMin = weightedGR.total().xAxis().front();
    auto rMax = weightedGR.total().xAxis().back();
    auto rho = rdfModule->effectiveDensity();
    if (!rho)
    {
        Messenger::error("No effective density available from RDF module '{}'\n", rdfModule->name());
        return ExecutionResult::Failed;
    }

    Fourier::sineFT(repGR, 1.0 / (2.0 * PI * PI * *rho), rMin, 0.05, rMax, WindowFunction(referenceWindowFunction_));

    // Save data if requested
    if (saveRepresentativeGR_)
    {
        if (moduleContext.processPool().isMaster())
        {
            Data1DExportFileFormat exportFormat(fmt::format("{}-weighted-total.gr.broad", name_));
            if (exportFormat.exportData(repGR))
                moduleContext.processPool().decideTrue();
            else
                moduleContext.processPool().decideFalse();
        }
        else if (!moduleContext.processPool().decision())
            return ExecutionResult::Failed;
    }

    return ExecutionResult::Success;
}
