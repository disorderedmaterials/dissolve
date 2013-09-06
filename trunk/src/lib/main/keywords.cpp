/*
	*** Keyword Definitions
	*** src/lib/main/keywords.cpp
	Copyright T. Youngs 2012-2013

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

/*!
 * \brief Print list of valid keywords for InputBlock specified
 */
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
		case (Keywords::SetupBlock):
			for (n=0; n<Keywords::nSetupKeywords; ++n) msg.print("%s%s", n == 0 ? "" : ", ", setupKeyword( (SetupKeyword) n ));
			break;
		case (Keywords::SimulationBlock):
			for (n=0; n<Keywords::nSimulationKeywords; ++n) msg.print("%s%s", n == 0 ? "" : ", ", simulationKeyword( (SimulationKeyword) n ));
			break;
		case (Keywords::SpeciesBlock):
			for (n=0; n<Keywords::nSpeciesKeywords; ++n) msg.print("%s%s", n == 0 ? "" : ", ", speciesKeyword( (SpeciesKeyword) n ));
			break;
		case (Keywords::SystemBlock):
			for (n=0; n<Keywords::nSystemKeywords; ++n) msg.print("%s%s", n == 0 ? "" : ", ", systemKeyword( (SystemKeyword) n ));
			break;
		default:
			printf("Unrecognised block given to Keywords::printValidKeywords.\n");
			break;
	}
	msg.print("\n");
}

/*
// Input Block Keywords
*/

// Input File Block Keywords
const char *InputBlockKeywords[] = { "AtomTypes", "PairPotentials", "Sample", "Setup", "Simulation", "Species", "System" };

/*!
 * \brief Convert text string to InputBlock
 */
Keywords::InputBlock Keywords::inputBlock(const char* s)
{
	for (int n=0; n<Keywords::nInputBlocks; ++n) if (strcmp(s,InputBlockKeywords[n]) == 0) return (Keywords::InputBlock) n;
	return Keywords::nInputBlocks;
}

/*!
 * \brief Convert InputBlock to text string
 */
const char *Keywords::inputBlock(Keywords::InputBlock id)
{
	return InputBlockKeywords[id];
}

/*
// AtomTypes Keywords
*/

// AtomTypes Block Keywords
const char *AtomTypesBlockKeywords[] = { "AtomType", "EndAtomTypes" };

// AtomTypes Block NArguments
int AtomTypesBlockNArguments[] = { 3, 0 };

/*!
 * \brief Convert text string to AtomTypesKeyword
 */
Keywords::AtomTypesKeyword Keywords::atomTypesKeyword(const char* s)
{
	for (int n=0; n<nAtomTypesKeywords; ++n) if (strcmp(s,AtomTypesBlockKeywords[n]) == 0) return (Keywords::AtomTypesKeyword) n;
	return nAtomTypesKeywords;
}

/*!
 * \brief Convert AtomTypesKeyword to text string
 */
const char *Keywords::atomTypesKeyword(Keywords::AtomTypesKeyword id)
{
	return AtomTypesBlockKeywords[id];
}

/*!
 * \brief Return minimum number of expected arguments
 */
int Keywords::atomTypesBlockNArguments(Keywords::AtomTypesKeyword id)
{
	return AtomTypesBlockNArguments[id];
}

/*
// Species Keywords
*/

// Species Block Keywords
const char *SpeciesBlockKeywords[] = { "Angle", "Atom", "Bond", "EndSpecies", "Grain", "Isotopologue" };

// Species Block NArguments
int SpeciesBlockNArguments[] = { 5, 7, 4, 0, 1, 1 };

/*!
 * \brief Convert text string to SpeciesKeyword
 */
Keywords::SpeciesKeyword Keywords::speciesKeyword(const char* s)
{
	for (int n=0; n<nSpeciesKeywords; ++n) if (strcmp(s,SpeciesBlockKeywords[n]) == 0) return (Keywords::SpeciesKeyword) n;
	return nSpeciesKeywords;
}

/*!
 * \brief Convert SpeciesKeyword to text string
 */
const char *Keywords::speciesKeyword(Keywords::SpeciesKeyword id)
{
	return SpeciesBlockKeywords[id];
}

/*!
 * \brief Return minimum number of expected arguments
 */
int Keywords::speciesBlockNArguments(Keywords::SpeciesKeyword id)
{
	return SpeciesBlockNArguments[id];
}

/*
// System Block Keywords
*/

// System Block Keywords
const char *SystemBlockKeywords[] = { "CellAngles", "CellLengths", "Component", "Density", "EndSystem", "File", "Multiplier", "NonPeriodic" };

// System Block NArguments
int SystemBlockNArguments[] = { 3, 3, 2, 1, 0, 1, 1, 0 };

/*!
 * \brief Convert text string to SystemKeyword
 */
Keywords::SystemKeyword Keywords::systemKeyword(const char* s)
{
	for (int n=0; n<Keywords::nSystemKeywords; ++n) if (strcmp(s,SystemBlockKeywords[n]) == 0) return (Keywords::SystemKeyword) n;
	return Keywords::nSystemKeywords;
}

/*!
 * \brief Convert SystemKeyword to text string
 */
const char *Keywords::systemKeyword(Keywords::SystemKeyword id)
{
	return SystemBlockKeywords[id];
}

/*!
 * \brief Return minimum number of expected arguments
 */
int Keywords::systemBlockNArguments(Keywords::SystemKeyword id)
{
	return SystemBlockNArguments[id];
}

/*
// Sample Block Keywords
*/

// Sample Block Keywords
const char *SampleBlockKeywords[] = { "EndSample", "FitQMax", "FitQMin", "Isotopologue", "NormalisedToAvSq", "NormalisedToSqAv", "ReferenceData", "SubtractSelf" };

// Sample Block NArguments
int SampleBlockNArguments[] = { 0, 1, 1, 3, 0, 0, 1, 0 };

/*!
 * \brief Convert text string to SampleKeyword
 */
Keywords::SampleKeyword Keywords::sampleKeyword(const char* s)
{
	for (int n=0; n<Keywords::nSampleKeywords; ++n) if (strcmp(s,SampleBlockKeywords[n]) == 0) return (Keywords::SampleKeyword) n;
	return Keywords::nSampleKeywords;
}

/*!
 * \brief Convert SampleKeyword to text string
 */
const char *Keywords::sampleKeyword(Keywords::SampleKeyword id)
{
	return SampleBlockKeywords[id];
}

/*!
 * \brief Return minimum number of expected arguments
 */
int Keywords::sampleBlockNArguments(Keywords::SampleKeyword id)
{
	return SampleBlockNArguments[id];
}

/*
// PairPotentials Block Keywords
*/

// PairPotentials Block Keywords
const char *PairPotentialsBlockKeywords[] = { "Coulomb", "Delta", "Dispersion", "EndPairPotentials", "Full", "Range", "TruncationWidth" };

// PairPotentials Block NArguments
int PairPotentialsBlockNArguments[] = { 4, 1, 4, 0, 6, 1, 1 };

/*!
 * \brief Convert text string to PairPotentialsKeyword
 */
Keywords::PairPotentialsKeyword Keywords::pairPotentialsKeyword(const char* s)
{
	for (int n=0; n<Keywords::nPairPotentialsKeywords; ++n) if (strcmp(s,PairPotentialsBlockKeywords[n]) == 0) return (Keywords::PairPotentialsKeyword) n;
	return Keywords::nPairPotentialsKeywords;
}

/*!
 * \brief Convert PairPotentialsKeyword to text string
 */
const char *Keywords::pairPotentialsKeyword(Keywords::PairPotentialsKeyword id)
{
	return PairPotentialsBlockKeywords[id];
}

/*!
 * \brief Return minimum number of expected arguments
 */
int Keywords::pairPotentialsBlockNArguments(Keywords::PairPotentialsKeyword id)
{
	return PairPotentialsBlockNArguments[id];
}

/*
// Setup Block Keywords
*/

// Setup Block Keywords
const char *SetupBlockKeywords[] = { "BoxNormalisationFile", "BoxNormalisationPoints", "Broadening", "CellDensityMultiplier", "EndSetup", "RDFBinWidth", "RDFExtensionLimit", "RDFMethod", "RDFRange", "RDFSmoothing", "RMSEDeltaQ", "Seed", "SimplexNCycles", "SimplexNMoves", "SimplexTemperature", "SimplexTolerance", "Temperature", "WindowFunction" };

// Setup Block NArguments
int SetupBlockNArguments[] = { 1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

/*!
 * \brief Convert text string to SetupKeyword
 */
Keywords::SetupKeyword Keywords::setupKeyword(const char* s)
{
	for (int n=0; n<Keywords::nSetupKeywords; ++n) if (strcmp(s,SetupBlockKeywords[n]) == 0) return (Keywords::SetupKeyword) n;
	return Keywords::nSetupKeywords;
}

/*!
 * \brief Convert SetupKeyword to text string
 */
const char *Keywords::setupKeyword(Keywords::SetupKeyword id)
{
	return SetupBlockKeywords[id];
}

/*!
 * \brief Return minimum number of expected arguments
 */
int Keywords::setupBlockNArguments(Keywords::SetupKeyword id)
{
	return SetupBlockNArguments[id];
}

/*
// Simulation Block Keywords
*/

// Simulation Block Keywords
const char *SimulationBlockKeywords[] = { "EndSimulation", "@Equilibration", "@Shake", "@Strategy" };

// Simulation Block NArguments
int SimulationBlockNArguments[] = { 0, 0, 0, 0 };

/*!
 * \brief Convert text string to SimulationKeyword
 */
Keywords::SimulationKeyword Keywords::simulationKeyword(const char* s)
{
	for (int n=0; n<Keywords::nSimulationKeywords; ++n) if (strcmp(s,SimulationBlockKeywords[n]) == 0) return (Keywords::SimulationKeyword) n;
	return Keywords::nSimulationKeywords;
}

/*!
 * \brief Convert SimulationKeyword to text string
 */
const char *Keywords::simulationKeyword(Keywords::SimulationKeyword id)
{
	return SimulationBlockKeywords[id];
}

/*!
 * \brief Return minimum number of expected arguments
 */
int Keywords::simulationBlockNArguments(Keywords::SimulationKeyword id)
{
	return SimulationBlockNArguments[id];
}

