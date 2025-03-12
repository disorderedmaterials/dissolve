// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "modules/moleculeTorsion/moleculeTorsion.h"
#include "analyser/dataExporter.h"
#include "analyser/dataOperator1D.h"
#include "classes/species.h"
#include "io/export/data1D.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"
#include "keywords/integer.h"
#include "keywords/species.h"
#include "main/dissolve.h"
#include "module/context.h"

MoleculeTorsionModule::MoleculeTorsionModule() : Module(ModuleTypes::MoleculeTorsion)
{
    /*
     * Keywords
     */

    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setEditSignals({KeywordBase::ClearModuleData, KeywordBase::RecreateRenderables});

    keywords_.setOrganisation("Options", "Torsion", "Specify torsion target");
    keywords_.add<SpeciesKeyword>("Species", "Target species containing the torsion", species_);
    keywords_.add<IntegerKeyword>("I", "Index of atom i in the torsion i-j-k-l", i_, 1);
    keywords_.add<IntegerKeyword>("J", "Index of atom j in the torsion i-j-k-l", j_, 1);
    keywords_.add<IntegerKeyword>("K", "Index of atom k in the torsion i-j-k-l", k_, 1);
    keywords_.add<IntegerKeyword>("L", "Index of atom l in the torsion i-j-k-l", l_, 1);

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("Export", "File format and file name under which to save calculated histogram data",
                                        exportFileAndFormat_, "EndExport");
}

// Run set-up stage
bool MoleculeTorsionModule::setUp(ModuleContext &moduleContext, Flags<KeywordBase::KeywordSignal> actionSignals)
{
    if (!species_)
        return Messenger::error("[MoleculeTorsion] No target species provided.\n");

    if (i_ > species_->nAtoms())
        return Messenger::error("[MoleculeTorsion] Torsion atom index i={} is out of range.\n", i_);
    if (j_ > species_->nAtoms())
        return Messenger::error("[MoleculeTorsion] Torsion atom index j={} is out of range.\n", j_);
    if (k_ > species_->nAtoms())
        return Messenger::error("[MoleculeTorsion] Torsion atom index k={} is out of range.\n", k_);
    if (l_ > species_->nAtoms())
        return Messenger::error("[MoleculeTorsion] Torsion atom index l={} is out of range.\n", l_);

    std::set<int> atoms = {i_, j_, k_, l_};
    if (atoms.size() != 4)
        return Messenger::error("[MoleculeTorsion] Atoms defining torsion must be unique.");

    return true;
}

// Run main processing
Module::ExecutionResult MoleculeTorsionModule::process(ModuleContext &moduleContext)
{
    auto &processingData = moduleContext.dissolve().processingModuleData();

    // Calculate phi(ijkl)
    auto [histIJKL, status] = processingData.realiseIf<Histogram1D>("Histo-IJKL", name(), GenericItem::InRestartFileFlag);
    if (status == GenericItem::ItemStatus::Created)
        histIJKL.initialise(-180.0, 180.0, 1.0);
    histIJKL.zeroBins();

    // Loop over molecules in the Configuration
    const auto *box = targetConfiguration_->box();
    for (const auto &mol : targetConfiguration_->molecules())
    {
        if (mol->species() != species_)
            continue;

        auto rj = mol->atom(j_ - 1)->r();
        auto rk = mol->atom(k_ - 1)->r();
        histIJKL.bin(Box::torsionInDegrees(box->minimumVector(rj, mol->atom(i_ - 1)->r()), box->minimumVector(rj, rk),
                                           box->minimumVector(rk, mol->atom(l_ - 1)->r())));
    }

    // Accumulate histogram
    histIJKL.accumulate();

    // Distance(A-B)
    auto &dataNormalisedHisto = processingData.realise<Data1D>("NormalisedHistogram", name(), GenericItem::InRestartFileFlag);
    dataNormalisedHisto = histIJKL.accumulatedData();

    // Normalise
    DataOperator1D histogramNormaliser(dataNormalisedHisto);
    // Normalise by value
    histogramNormaliser.normaliseSumTo();

    // Save phi(ijkl) data?
    if (!DataExporter<Data1D, Data1DExportFileFormat>::exportData(dataNormalisedHisto, exportFileAndFormat_,
                                                                  moduleContext.processPool()))
        return ExecutionResult::Failed;

    return ExecutionResult::Success;
}
