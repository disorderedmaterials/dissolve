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
	msg.print("Valid Keywords for '%s' block are:\n", inputBlock(block));
	int n;
	switch (block)
	{
		case (Keywords::AtomTypesBlock):
			for (n=0; n<Keywords::nAtomTypesKeywords; ++n) msg.print("%s%s", n == 0 ? "" : ", ", atomTypesKeyword( (AtomTypesKeyword) n ));
			break;
		case (Keywords::PairPotentialsBlock):
			for (n=0; n<Keywords::nPairPotentialsKeywords; ++n) msg.print("%s%s", n == 0 ? "" : ", ", pairPotentialsKeyword( (PairPotentialsKeyword) n ));
			break;
		case (Keywords::SampleBlock):
			for (n=0; n<Keywords::nSampleKeywords; ++n) msg.print("%s%s", n == 0 ? "" : ", ", sampleKeyword( (SampleKeyword) n ));
			break;
		case (Keywords::ConfigurationBlock):
			for (n=0; n<Keywords::nConfigurationKeywords; ++n) msg.print("%s%s", n == 0 ? "" : ", ", configurationKeyword( (ConfigurationKeyword) n ));
			break;
		case (Keywords::SpeciesBlock):
			for (n=0; n<Keywords::nSpeciesKeywords; ++n) msg.print("%s%s", n == 0 ? "" : ", ", speciesKeyword( (SpeciesKeyword) n ));
			break;
		case (Keywords::SimulationBlock):
			for (n=0; n<Keywords::nSimulationKeywords; ++n) msg.print("%s%s", n == 0 ? "" : ", ", simulationKeyword( (SimulationKeyword) n ));
			break;
		default:
			printf("Unrecognised block given to Keywords::printValidKeywords.\n");
			break;
	}
	msg.print("\n");
}

/*
 * Input Block Keywords
 */

// Input File Block Keywords
const char* InputBlockKeywords[] = { "AtomTypes", "Configuration", "PairPotentials", "Sample", "Species", "Simulation" };

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

/*
 * AtomTypes Keywords
 */

// AtomTypes Block Keywords
const char* AtomTypesBlockKeywords[] = { "AtomType", "EndAtomTypes" };

// AtomTypes Block NArguments
int AtomTypesBlockNArguments[] = { 3, 0 };

// Convert text string to AtomTypesKeyword
Keywords::AtomTypesKeyword Keywords::atomTypesKeyword(const char* s)
{
	for (int n=0; n<nAtomTypesKeywords; ++n) if (strcmp(s,AtomTypesBlockKeywords[n]) == 0) return (Keywords::AtomTypesKeyword) n;
	return nAtomTypesKeywords;
}

// Convert AtomTypesKeyword to text string
const char* Keywords::atomTypesKeyword(Keywords::AtomTypesKeyword id)
{
	return AtomTypesBlockKeywords[id];
}

// Return minimum number of expected arguments
int Keywords::atomTypesBlockNArguments(Keywords::AtomTypesKeyword id)
{
	return AtomTypesBlockNArguments[id];
}

/*
 * Configuration Block Keywords
 */

// Configuration Block Keywords
const char* ConfigurationBlockKeywords[] = {
	"BoxNormalisationFile", "Bragg", "BraggBroadening", "BraggMaximumQ",
	"CellAngles", "CellLengths",
	"Density",
	"EndConfiguration",
	"File",
	"Multiplier",
	"NonPeriodic",
	"QDelta", "QMax", 
	"RDFBinWidth", "RDFMethod", "RDFRange", "RDFSmoothing", "RMSEDeltaQ",
	"Sample", "Species",
	"Temperature",
	"WindowFunction"
};
 
// Configuration Block NArguments
int ConfigurationBlockNArguments[] = {
	1, 1, 1, 1, 
	3, 3,
	1,
	0,
	1,
	1, 
	0, 
	1, 1,
	1, 1, 1, 1, 1,
	1, 2,
	1,
	1
	};

// Convert text string to ConfigurationKeyword
Keywords::ConfigurationKeyword Keywords::configurationKeyword(const char* s)
{
	for (int n=0; n<Keywords::nConfigurationKeywords; ++n) if (strcmp(s,ConfigurationBlockKeywords[n]) == 0) return (Keywords::ConfigurationKeyword) n;
	return Keywords::nConfigurationKeywords;
}

// Convert ConfigurationKeyword to text string
const char* Keywords::configurationKeyword(Keywords::ConfigurationKeyword id)
{
	return ConfigurationBlockKeywords[id];
}

// Return minimum number of expected arguments
int Keywords::configurationBlockNArguments(Keywords::ConfigurationKeyword id)
{
	return ConfigurationBlockNArguments[id];
}


/*
 * PairPotentials Block Keywords
 */

// PairPotentials Block Keywords
const char* PairPotentialsBlockKeywords[] = { "Coulomb", "Delta", "Dispersion", "EndPairPotentials", "Full", "Range", "TruncationWidth" };

// PairPotentials Block NArguments
int PairPotentialsBlockNArguments[] = { 4, 1, 4, 0, 6, 1, 1 };

// Convert text string to PairPotentialsKeyword
Keywords::PairPotentialsKeyword Keywords::pairPotentialsKeyword(const char* s)
{
	for (int n=0; n<Keywords::nPairPotentialsKeywords; ++n) if (strcmp(s,PairPotentialsBlockKeywords[n]) == 0) return (Keywords::PairPotentialsKeyword) n;
	return Keywords::nPairPotentialsKeywords;
}

// Convert PairPotentialsKeyword to text string
const char* Keywords::pairPotentialsKeyword(Keywords::PairPotentialsKeyword id)
{
	return PairPotentialsBlockKeywords[id];
}

// Return minimum number of expected arguments
int Keywords::pairPotentialsBlockNArguments(Keywords::PairPotentialsKeyword id)
{
	return PairPotentialsBlockNArguments[id];
}


/*
 * Sample Block Keywords
 */

// Sample Block Keywords
const char* SampleBlockKeywords[] = {  "Broadening", "EndSample", "FitMax", "FitMin", "Isotopologue", "NormalisedToAvSq", "NormalisedToSqAv", "ReferenceData", "SubtractSelf", "Type" };

// Sample Block NArguments
int SampleBlockNArguments[] = { 2, 0, 1, 1, 3, 0, 0, 1, 0, 1 };

// Convert text string to SampleKeyword
Keywords::SampleKeyword Keywords::sampleKeyword(const char* s)
{
	for (int n=0; n<Keywords::nSampleKeywords; ++n) if (strcmp(s,SampleBlockKeywords[n]) == 0) return (Keywords::SampleKeyword) n;
	return Keywords::nSampleKeywords;
}

// Convert SampleKeyword to text string
const char* Keywords::sampleKeyword(Keywords::SampleKeyword id)
{
	return SampleBlockKeywords[id];
}

// Return minimum number of expected arguments
int Keywords::sampleBlockNArguments(Keywords::SampleKeyword id)
{
	return SampleBlockNArguments[id];
}

/*
 * Simulation Block Keywords
 */

// Simulation Block Keywords
const char* SimulationBlockKeywords[] = { "BoxNormalisationPoints", "EndSimulation", "Seed", "SimplexNCycles", "SimplexNMoves", "SimplexTemperature", "SimplexTolerance" };

// Simulation Block NArguments
int SimulationBlockNArguments[] = { 1, 0, 1, 1, 1, 1, 1 };

// Convert text string to SimulationKeyword
Keywords::SimulationKeyword Keywords::simulationKeyword(const char* s)
{
	for (int n=0; n<Keywords::nSimulationKeywords; ++n) if (strcmp(s,SimulationBlockKeywords[n]) == 0) return (Keywords::SimulationKeyword) n;
	return Keywords::nSimulationKeywords;
}

// Convert SimulationKeyword to text string
const char* Keywords::simulationKeyword(Keywords::SimulationKeyword id)
{
	return SimulationBlockKeywords[id];
}

// Return minimum number of expected arguments
int Keywords::simulationBlockNArguments(Keywords::SimulationKeyword id)
{
	return SimulationBlockNArguments[id];
}

/*
 * Species Keywords
 */

// Species Block Keywords
const char* SpeciesBlockKeywords[] = { "Angle", "Atom", "Bond", "EndSpecies", "Grain", "Isotopologue" };

// Species Block NArguments
int SpeciesBlockNArguments[] = { 5, 7, 4, 0, 1, 1 };

// Convert text string to SpeciesKeyword
Keywords::SpeciesKeyword Keywords::speciesKeyword(const char* s)
{
	for (int n=0; n<nSpeciesKeywords; ++n) if (strcmp(s,SpeciesBlockKeywords[n]) == 0) return (Keywords::SpeciesKeyword) n;
	return nSpeciesKeywords;
}

// Convert SpeciesKeyword to text string
const char* Keywords::speciesKeyword(Keywords::SpeciesKeyword id)
{
	return SpeciesBlockKeywords[id];
}

// Return minimum number of expected arguments
int Keywords::speciesBlockNArguments(Keywords::SpeciesKeyword id)
{
	return SpeciesBlockNArguments[id];
}
