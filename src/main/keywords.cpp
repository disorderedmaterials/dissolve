// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "main/keywords.h"
#include "base/messenger.h"
#include "base/sysFunc.h"

/*
 * Block Keywords
 */

// Return enum option info for PairPotentialsKeyword
EnumOptions<BlockKeywords::BlockKeyword> BlockKeywords::keywords()
{
    return EnumOptions<BlockKeywords::BlockKeyword>("BlockKeyword",
                                                    {{BlockKeywords::ConfigurationBlockKeyword, "Configuration"},
                                                     {BlockKeywords::LayerBlockKeyword, "Layer"},
                                                     {BlockKeywords::TaskBlockKeyword, "Task"},
                                                     {BlockKeywords::MasterBlockKeyword, "Master"},
                                                     {BlockKeywords::ModuleBlockKeyword, "Module"},
                                                     {BlockKeywords::PairPotentialsBlockKeyword, "PairPotentials"},
                                                     {BlockKeywords::SiteBlockKeyword, "Site"},
                                                     {BlockKeywords::SpeciesBlockKeyword, "Species"}});
}
