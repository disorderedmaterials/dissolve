// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "main/keywords.h"
#include "base/messenger.h"
#include "base/sysfunc.h"

/*
 * Block Keywords
 */

// Return enum option info for PairPotentialsKeyword
EnumOptions<BlockKeywords::BlockKeyword> BlockKeywords::keywords()
{
    static EnumOptionsList BlockKeywordOptions = EnumOptionsList()
                                                 << EnumOption(BlockKeywords::ConfigurationBlockKeyword, "Configuration")
                                                 << EnumOption(BlockKeywords::LayerBlockKeyword, "Layer")
                                                 << EnumOption(BlockKeywords::MasterBlockKeyword, "Master")
                                                 << EnumOption(BlockKeywords::ModuleBlockKeyword, "Module")
                                                 << EnumOption(BlockKeywords::PairPotentialsBlockKeyword, "PairPotentials")
                                                 << EnumOption(BlockKeywords::SimulationBlockKeyword, "Simulation")
                                                 << EnumOption(BlockKeywords::SiteBlockKeyword, "Site")
                                                 << EnumOption(BlockKeywords::SpeciesBlockKeyword, "Species");

    static EnumOptions<BlockKeywords::BlockKeyword> options("BlockKeyword", BlockKeywordOptions);

    return options;
}
