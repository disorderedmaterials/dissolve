/*
	*** Keyword Definitions
	*** src/main/keywords.cpp
	Copyright T. Youngs 2012-2016

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "main/keywords.h"
#include "base/sysfunc.h"
#include "base/messenger.h"
#include <string.h>

// Print list of valid keywords for InputBlock specified
void Keywords::printValidKeywords(Keywords::InputBlock block)
{
	Messenger::print("Valid Keywords for '%s' block are:\n", inputBlock(block));
	int n;
	switch (block)
	{
		case (Keywords::AtomTypesBlock):
			for (n=0; n<Keywords::nAtomTypesKeywords; ++n) Messenger::print("%s%s", n == 0 ? "" : ", ", atomTypesKeyword( (AtomTypesKeyword) n ));
			break;
		case (Keywords::ConfigurationBlock):
			for (n=0; n<Keywords::nConfigurationKeywords; ++n) Messenger::print("%s%s", n == 0 ? "" : ", ", configurationKeyword( (ConfigurationKeyword) n ));
			break;
		case (Keywords::ModuleBlock):
			for (n=0; n<Keywords::nModuleKeywords; ++n) Messenger::print("%s%s", n == 0 ? "" : ", ", moduleKeyword( (ModuleKeyword) n ));
			break;
		case (Keywords::PairPotentialsBlock):
			for (n=0; n<Keywords::nPairPotentialsKeywords; ++n) Messenger::print("%s%s", n == 0 ? "" : ", ", pairPotentialsKeyword( (PairPotentialsKeyword) n ));
			break;
		case (Keywords::SampleBlock):
			for (n=0; n<Keywords::nSampleKeywords; ++n) Messenger::print("%s%s", n == 0 ? "" : ", ", sampleKeyword( (SampleKeyword) n ));
			break;
			break;
		case (Keywords::SimulationBlock):
			for (n=0; n<Keywords::nSimulationKeywords; ++n) Messenger::print("%s%s", n == 0 ? "" : ", ", simulationKeyword( (SimulationKeyword) n ));
			break;
		case (Keywords::SpeciesBlock):
			for (n=0; n<Keywords::nSpeciesKeywords; ++n) Messenger::print("%s%s", n == 0 ? "" : ", ", speciesKeyword( (SpeciesKeyword) n ));
		default:
			printf("Unrecognised block given to Keywords::printValidKeywords.\n");
			break;
	}
	Messenger::print("\n");
}

/*
 * Input Block Keywords
 */

// Input File Block Keywords
const char* InputBlockKeywords[] = { "AtomTypes", "Configuration", "Module", "PairPotentials", "Sample", "Simulation", "Species" };

// Convert text string to InputBlock
Keywords::InputBlock Keywords::inputBlock(const char* s)
{
	for (int n=0; n<Keywords::nInputBlocks; ++n) if (strcmp(s,InputBlockKeywords[n]) == 0) return (Keywords::InputBlock) n;
	return Keywords::nInputBlocks;
}

// Convert InputBlock to text string
const char* Keywords::inputBlock(Keywords::InputBlock id)
{
	return InputBlockKeywords[id];
}
