/*
	*** Keyword Parsing - Simulation Block
	*** src/main/keywords_simulation.cpp
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
#include "main/duq.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"

// Simulation Block Keywords
KeywordData SimulationBlockData[] = {
	{ "BoxNormalisationPoints",		1,	"Number of random insertions to use when generating the normalisation array" },
	{ "EndSimulation",			0,	"Signals the end of the Simulation block" },
	{ "MaxIterations",			1,	"Maximum number of main loop iterations to perform, or -1 for no limit" },
	{ "ParallelStrategy",			1, 	"Determines the distribution of processes across Configurations" },
	{ "Seed",				1,	"Random seed to use" },
	{ "WindowFunction",			1,	"Window function to use in all Fourier transforms" },
	{ "WriteIterationData",			1,	"Write various data at the end of each iteration" }
};

// Convert text string to SimulationKeyword
SimulationBlock::SimulationKeyword SimulationBlock::keyword(const char* s)
{
	for (int n=0; n<SimulationBlock::nSimulationKeywords; ++n) if (DUQSys::sameString(s,SimulationBlockData[n].name)) return (SimulationBlock::SimulationKeyword) n;
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
bool SimulationBlock::parse(LineParser& parser, DUQ* duq)
{
	Messenger::print("\nParsing %s block...\n", InputBlocks::inputBlock(InputBlocks::SimulationBlock));

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
				duq->setNBoxNormalisationPoints(parser.argi(1));
				Messenger::print("Number of points to use in Box normalisation calculation = %i\n", duq->nBoxNormalisationPoints());
				break;
			case (SimulationBlock::EndSimulationKeyword):
				Messenger::print("Found end of %s block.\n", InputBlocks::inputBlock(InputBlocks::SimulationBlock));
				blockDone = true;
				break;
			case (SimulationBlock::MaxIterationsKeyword):
				duq->setMaxIterations(parser.argi(1));
				Messenger::print("Maximum number of main loop iterations set to %i.\n", duq->maxIterations());
				break;
			case (SimulationBlock::ParallelStrategyKeyword):
				if (DUQ::parallelStrategy(parser.argc(1)) == DUQ::nParallelStrategies)
				{
					Messenger::error("Unrecognised parallel strategy '%s'.\n", parser.argc(1));
					error = true;
				}
				else duq->setParallelStrategy(DUQ::parallelStrategy(parser.argc(1)));
				break;
			case (SimulationBlock::SeedKeyword):
				duq->setSeed(parser.argi(1));
				Messenger::print("Random seed set to %i.\n", duq->seed());
				break;
			case (SimulationBlock::WindowFunctionKeyword):
				if (XYData::windowFunction(parser.argc(1)) == XYData::nWindowFunctions)
				{
					Messenger::error("Unrecognised window function '%s'.\n", parser.argc(1));
					error = true;
				}
				else duq->setWindowFunction(XYData::windowFunction(parser.argc(1)));
				break;
			case (SimulationBlock::WriteIterationDataKeyword):
				duq->setWriteIterationData(parser.argb(1));
				Messenger::print("Data will be written to disk after each iteration.\n");
				break;
			case (SimulationBlock::nSimulationKeywords):
				Messenger::print("Unrecognised %s block keyword '%s' found. block keyword found - '%s'\n", InputBlocks::inputBlock(InputBlocks::SimulationBlock), parser.argc(0));
				InputBlocks::printValidKeywords(InputBlocks::SimulationBlock);
				error = true;
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", InputBlocks::inputBlock(InputBlocks::SimulationBlock), SimulationBlock::keyword(simKeyword));
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
