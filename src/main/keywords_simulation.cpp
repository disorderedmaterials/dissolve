/*
	*** Keyword Parsing - Simulation Block
	*** src/main/keywords_simulation.cpp
	Copyright T. Youngs 2012-2019

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
#include "main/dissolve.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"

// Simulation Block Keywords
KeywordData SimulationBlockData[] = {
	{ "BoxNormalisationPoints",		1,	"Number of random insertions to use when generating the normalisation array" },
	{ "EndSimulation",			0,	"Signals the end of the Simulation block" },
	{ "ParallelStrategy",			1, 	"Determines the distribution of processes across Configurations" },
	{ "RestartFileFrequency",		1,	"Frequency with which to write restart file (0 = never)" },
	{ "Seed",				1,	"Random seed to use" }
};

// Convert text string to SimulationKeyword
SimulationBlock::SimulationKeyword SimulationBlock::keyword(const char* s)
{
	for (int n=0; n<SimulationBlock::nSimulationKeywords; ++n) if (DissolveSys::sameString(s,SimulationBlockData[n].name)) return (SimulationBlock::SimulationKeyword) n;
	return SimulationBlock::nSimulationKeywords;
}

// Convert SimulationKeyword to text string
const char* SimulationBlock::keyword(SimulationBlock::SimulationKeyword id)
{
	return SimulationBlockData[id].name;
}

// Return minimum number of expected arguments
int SimulationBlock::nArguments(SimulationBlock::SimulationKeyword id)
{
	return SimulationBlockData[id].nArguments;
}

// Parse Simulation block
bool SimulationBlock::parse(LineParser& parser, Dissolve* dissolve)
{
	Messenger::print("\nParsing %s block...\n", BlockKeywords::blockKeyword(BlockKeywords::SimulationBlockKeyword));

	bool blockDone = false, error = false;

	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(LineParser::SkipBlanks+LineParser::StripComments+LineParser::UseQuotes);
		SimulationBlock::SimulationKeyword simKeyword = SimulationBlock::keyword(parser.argc(0));
		if ((simKeyword != SimulationBlock::nSimulationKeywords) && ((parser.nArgs()-1) < SimulationBlock::nArguments(simKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", SimulationBlock::keyword(simKeyword));
			error = true;
			break;
		}
		switch (simKeyword)
		{
			case (SimulationBlock::BoxNormalisationPointsKeyword):
				dissolve->setNBoxNormalisationPoints(parser.argi(1));
				Messenger::print("Number of points to use in Box normalisation calculation = %i\n", dissolve->nBoxNormalisationPoints());
				break;
			case (SimulationBlock::EndSimulationKeyword):
				Messenger::print("Found end of %s block.\n", BlockKeywords::blockKeyword(BlockKeywords::SimulationBlockKeyword));
				blockDone = true;
				break;
			case (SimulationBlock::ParallelStrategyKeyword):
				if (Dissolve::parallelStrategy(parser.argc(1)) == Dissolve::nParallelStrategies)
				{
					Messenger::error("Unrecognised parallel strategy '%s'.\n", parser.argc(1));
					error = true;
				}
				else dissolve->setParallelStrategy(Dissolve::parallelStrategy(parser.argc(1)));
				break;
			case (SimulationBlock::RestartFileFrequencyKeyword):
				dissolve->setRestartFileFrequency(parser.argi(1));
				Messenger::print("Restart data will be written to disk every %i iteration(s).\n", dissolve->restartFileFrequency());
				break;
			case (SimulationBlock::SeedKeyword):
				dissolve->setSeed(parser.argi(1));
				Messenger::print("Random seed set to %i.\n", dissolve->seed());
				break;
			case (SimulationBlock::nSimulationKeywords):
				Messenger::print("Unrecognised %s block keyword '%s' found.\n", BlockKeywords::blockKeyword(BlockKeywords::SimulationBlockKeyword), parser.argc(0));
				BlockKeywords::printValidKeywords(BlockKeywords::SimulationBlockKeyword);
				error = true;
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", BlockKeywords::blockKeyword(BlockKeywords::SimulationBlockKeyword), SimulationBlock::keyword(simKeyword));
				error = true;
				break;
		}
		
		// Error encountered?
		if (error) break;
		
		// End of block?
		if (blockDone) break;
	}

	return (!error);
}
