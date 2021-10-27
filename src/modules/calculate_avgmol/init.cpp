// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/calculate_avgmol/avgmol.h"

// Perform any necessary initialisation for the Module
void CalculateAvgMolModule::initialise()
{
    // Set up keywords
    keywords_
        .add<SpeciesSiteKeyword>("Control", "Site", "Target site about which to calculate average species geometry",
                                 targetSite_, true)
        .optionMask = KeywordBase::ModificationRequiresSetUpOption;

    targetSpecies_ = nullptr;
}
