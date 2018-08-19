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
void MainInputKeywords::printValidBlockKeywords(MainInputKeywords::MainInputKeyword block)
{
	Messenger::print("Valid Keywords for '%s' block are:\n", mainInputKeyword(block));
	int n;
	switch (block)
	{
		case (MainInputKeywords::ConfigurationBlockKeyword):
			for (n=0; n<ConfigurationBlock::nConfigurationKeywords; ++n) Messenger::print("  %s", ConfigurationBlock::keyword( (ConfigurationBlock::ConfigurationKeyword) n ));
			break;
		case (MainInputKeywords::MasterBlockKeyword):
			for (n=0; n<MasterBlock::nMasterKeywords; ++n) Messenger::print("  %s", MasterBlock::keyword( (MasterBlock::MasterKeyword) n ));
			break;
		case (MainInputKeywords::ModuleBlockKeyword):
			for (n=0; n<ModuleBlock::nModuleKeywords; ++n) Messenger::print("  %s", ModuleBlock::keyword( (ModuleBlock::ModuleKeyword) n ));
			break;
		case (MainInputKeywords::PairPotentialsBlockKeyword):
			for (n=0; n<PairPotentialsBlock::nPairPotentialsKeywords; ++n) Messenger::print("  %s", PairPotentialsBlock::keyword( (PairPotentialsBlock::PairPotentialsKeyword) n ));
			break;
		case (MainInputKeywords::SimulationBlockKeyword):
			for (n=0; n<SimulationBlock::nSimulationKeywords; ++n) Messenger::print("  %s", SimulationBlock::keyword( (SimulationBlock::SimulationKeyword) n ));
			break;
		case (MainInputKeywords::SiteBlockKeyword):
			for (n=0; n<SiteBlock::nSiteKeywords; ++n) Messenger::print("  %s", SiteBlock::keyword( (SiteBlock::SiteKeyword) n ));
			break;
		case (MainInputKeywords::SpeciesBlockKeyword):
			for (n=0; n<SpeciesBlock::nSpeciesKeywords; ++n) Messenger::print("  %s", SpeciesBlock::keyword( (SpeciesBlock::SpeciesKeyword) n ));
			break;
		case (MainInputKeywords::SpeciesInfoBlockKeyword):
			for (n=0; n<SpeciesInfoBlock::nSpeciesInfoKeywords; ++n) Messenger::print("  %s", SpeciesInfoBlock::keyword( (SpeciesInfoBlock::SpeciesInfoKeyword) n ));
			break;
		default:
			Messenger::print("Unrecognised block given to MainInputKeywords::printValidBlockKeywords.\n");
			break;
	}
	Messenger::print("\n");
}

/*
 * Main Input Keywords
 */

// Main Input Keywords
const char* MainInputKeywordKeywords[] = { "Configuration", "Master", "Module", "PairPotentials", "Simulation", "Site", "Species", "SpeciesInfo" };

// Convert text string to MainInputKeyword
MainInputKeywords::MainInputKeyword MainInputKeywords::mainInputKeyword(const char* s)
{
	for (int n=0; n<MainInputKeywords::nMainInputKeywords; ++n) if (DissolveSys::sameString(s,MainInputKeywordKeywords[n])) return (MainInputKeywords::MainInputKeyword) n;
	return MainInputKeywords::nMainInputKeywords;
}

// Convert MainInputKeyword to text string
const char* MainInputKeywords::mainInputKeyword(MainInputKeywords::MainInputKeyword id)
{
	return MainInputKeywordKeywords[id];
}
