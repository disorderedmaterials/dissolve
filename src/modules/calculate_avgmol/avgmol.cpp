// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/configuration.h"
#include "keywords/speciessite.h"
#include "modules/calculate_avgmol/avgmol.h"

CalculateAvgMolModule::CalculateAvgMolModule() : Module()
{
    // Targets
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);
    keywords_
        .add<SpeciesSiteKeyword>("Control", "Site", "Target site about which to calculate average species geometry",
                                 targetSite_, true)
        ->setOptionMask(KeywordBase::ModificationRequiresSetUpOption);

    targetSpecies_ = nullptr;
}

// Return type of module
std::string_view CalculateAvgMolModule::type() const { return "CalculateAvgMol"; }
