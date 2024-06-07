// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/avgMol/avgMol.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/fileAndFormat.h"
#include "keywords/speciesSite.h"

AvgMolModule::AvgMolModule() : Module(ModuleTypes::AvgMol)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setEditSignals({KeywordBase::ClearModuleData, KeywordBase::RecreateRenderables});

    keywords_.setOrganisation("Options", "Sites");
    keywords_
        .add<SpeciesSiteKeyword>("Site", "Target site about which to calculate average species geometry", targetSite_, true)
        ->setEditSignals(KeywordBase::ReloadExternalData);

    keywords_.setOrganisation("Export");
    keywords_.add<FileAndFormatKeyword>("ExportCoordinates", "Whether to save average coordinates to disk",
                                        exportFileAndFormat_, "EndExportCoordinates");

    targetSpecies_ = nullptr;
}
