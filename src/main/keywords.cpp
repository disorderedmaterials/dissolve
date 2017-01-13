/*
	*** Keyword Definitions
	*** src/main/keywords.cpp
	Copyright T. Youngs 2012-2017

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

// Print list of valid keywords for InputBlock specified
void InputBlocks::printValidKeywords(InputBlocks::InputBlock block)
{
	Messenger::print("Valid Keywords for '%s' block are:\n", inputBlock(block));
	int n;
	switch (block)
	{
		case (InputBlocks::ConfigurationBlock):
			for (n=0; n<ConfigurationBlock::nConfigurationKeywords; ++n) Messenger::print("  %s", ConfigurationBlock::keyword( (ConfigurationBlock::ConfigurationKeyword) n ));
			break;
		case (InputBlocks::ModuleBlock):
			for (n=0; n<ModuleBlock::nModuleKeywords; ++n) Messenger::print("  %s", ModuleBlock::keyword( (ModuleBlock::ModuleKeyword) n ));
			break;
		case (InputBlocks::PairPotentialsBlock):
			for (n=0; n<PairPotentialsBlock::nPairPotentialsKeywords; ++n) Messenger::print("  %s", PairPotentialsBlock::keyword( (PairPotentialsBlock::PairPotentialsKeyword) n ));
			break;
		case (InputBlocks::SampleBlock):
			for (n=0; n<SampleBlock::nSampleKeywords; ++n) Messenger::print("  %s", SampleBlock::keyword( (SampleBlock::SampleKeyword) n ));
			break;
			break;
		case (InputBlocks::SimulationBlock):
			for (n=0; n<SimulationBlock::nSimulationKeywords; ++n) Messenger::print("  %s", SimulationBlock::keyword( (SimulationBlock::SimulationKeyword) n ));
			break;
		case (InputBlocks::SpeciesBlock):
			for (n=0; n<SpeciesBlock::nSpeciesKeywords; ++n) Messenger::print("  %s", SpeciesBlock::keyword( (SpeciesBlock::SpeciesKeyword) n ));
		default:
			printf("Unrecognised block given to InputBlocks::printValidKeywords.\n");
			break;
	}
	Messenger::print("\n");
}

/*
 * Input Block Keywords
 */

// Input File Block Keywords
const char* InputBlockKeywords[] = { "Configuration", "Module", "PairPotentials", "Sample", "Simulation", "Species" };

// Convert text string to InputBlock
InputBlocks::InputBlock InputBlocks::inputBlock(const char* s)
{
	for (int n=0; n<InputBlocks::nInputBlocks; ++n) if (DUQSys::sameString(s,InputBlockKeywords[n])) return (InputBlocks::InputBlock) n;
	return InputBlocks::nInputBlocks;
}

// Convert InputBlock to text string
const char* InputBlocks::inputBlock(InputBlocks::InputBlock id)
{
	return InputBlockKeywords[id];
}
