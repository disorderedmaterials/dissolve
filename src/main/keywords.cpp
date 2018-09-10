/*
	*** Keyword Definitions
	*** src/main/keywords.cpp
	Copyright T. Youngs 2012-2018

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main/keywords.h"
#include "base/sysfunc.h"
#include "base/messenger.h"

// Print list of valid keywords for the block specified
void BlockKeywords::printValidKeywords(BlockKeywords::BlockKeyword block)
{
	Messenger::print("Valid Keywords for '%s' block are:\n", blockKeyword(block));
	int n;
	switch (block)
	{
		case (BlockKeywords::ConfigurationBlockKeyword):
			for (n=0; n<ConfigurationBlock::nConfigurationKeywords; ++n) Messenger::print("  %s", ConfigurationBlock::keyword( (ConfigurationBlock::ConfigurationKeyword) n ));
			break;
		case (BlockKeywords::MasterBlockKeyword):
			for (n=0; n<MasterBlock::nMasterKeywords; ++n) Messenger::print("  %s", MasterBlock::keyword( (MasterBlock::MasterKeyword) n ));
			break;
		case (BlockKeywords::ModuleBlockKeyword):
			for (n=0; n<ModuleBlock::nModuleKeywords; ++n) Messenger::print("  %s", ModuleBlock::keyword( (ModuleBlock::ModuleKeyword) n ));
			break;
		case (BlockKeywords::PairPotentialsBlockKeyword):
			for (n=0; n<PairPotentialsBlock::nPairPotentialsKeywords; ++n) Messenger::print("  %s", PairPotentialsBlock::keyword( (PairPotentialsBlock::PairPotentialsKeyword) n ));
			break;
		case (BlockKeywords::SimulationBlockKeyword):
			for (n=0; n<SimulationBlock::nSimulationKeywords; ++n) Messenger::print("  %s", SimulationBlock::keyword( (SimulationBlock::SimulationKeyword) n ));
			break;
		case (BlockKeywords::SiteBlockKeyword):
			for (n=0; n<SiteBlock::nSiteKeywords; ++n) Messenger::print("  %s", SiteBlock::keyword( (SiteBlock::SiteKeyword) n ));
			break;
		case (BlockKeywords::SpeciesBlockKeyword):
			for (n=0; n<SpeciesBlock::nSpeciesKeywords; ++n) Messenger::print("  %s", SpeciesBlock::keyword( (SpeciesBlock::SpeciesKeyword) n ));
			break;
		case (BlockKeywords::SpeciesInfoBlockKeyword):
			for (n=0; n<SpeciesInfoBlock::nSpeciesInfoKeywords; ++n) Messenger::print("  %s", SpeciesInfoBlock::keyword( (SpeciesInfoBlock::SpeciesInfoKeyword) n ));
			break;
		default:
			Messenger::print("Unrecognised block given to BlockKeywords::printValidKeywords.\n");
			break;
	}
	Messenger::print("\n");
}

/*
 * Block Keywords
 */

// Block Keywords - Those beginning with an underscore are not meant to be used as a main block
const char* BlockKeywordKeywords[] = { "Configuration", "Master", "Module", "PairPotentials", "Simulation", "_Site", "Species", "_SpeciesInfo" };

// Convert text string to MainInputKeyword
BlockKeywords::BlockKeyword BlockKeywords::blockKeyword(const char* s)
{
	for (int n=0; n<BlockKeywords::nBlockKeywords; ++n) if (DissolveSys::sameString(s,BlockKeywordKeywords[n])) return (BlockKeywords::BlockKeyword) n;
	return BlockKeywords::nBlockKeywords;
}

// Convert MainInputKeyword to text string
const char* BlockKeywords::blockKeyword(BlockKeywords::BlockKeyword id)
{
	return BlockKeywordKeywords[id];
}
