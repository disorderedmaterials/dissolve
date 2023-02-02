// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/calculate_avgmol/avgmol.h"
#include "keywords/configuration.h"
#include "keywords/speciessite.h"

CalculateAvgMolModule::CalculateAvgMolModule() : Module("CalculateAvgMol")
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Sites");
    keywords_
        .add<SpeciesSiteKeyword>("Site", "Target site about which to calculate average species geometry", targetSite_, true)
        ->setEditSignals(KeywordBase::ReloadExternalData);

    targetSpecies_ = nullptr;
}
