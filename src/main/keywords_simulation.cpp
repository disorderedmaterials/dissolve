/*
	*** Keyword Parsing - Simulation Block
	*** src/main/keywords_simulation.cpp
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
#include "main/duq.h"
#include <string.h>

// Simulation Block Keywords
KeywordData SimulationBlockData[] = {
	{ "BoxNormalisationPoints",		1,	"" },
	{ "EndSimulation",			0,	"" },
	{ "Seed",				1,	"" },
	{ "WindowFunction",			1,	"" }
};

// Convert text string to SimulationKeyword
Keywords::SimulationKeyword Keywords::simulationKeyword(const char* s)
{
	for (int n=0; n<Keywords::nSimulationKeywords; ++n) if (strcmp(s,SimulationBlockData[n].name) == 0) return (Keywords::SimulationKeyword) n;
	return Keywords::nSimulationKeywords;
}

// Convert SimulationKeyword to text string
const char* Keywords::simulationKeyword(Keywords::SimulationKeyword id)
{
	return SimulationBlockData[id].name;
}

// Return minimum number of expected arguments
int Keywords::simulationBlockNArguments(Keywords::SimulationKeyword id)
{
	return SimulationBlockData[id].nArguments;
}

// Parse Simulation block
bool Keywords::parseSimulationBlock(LineParser& parser, DUQ* duq)
{
	Messenger::print("Parsing %s block\n", Keywords::inputBlock(Keywords::SimulationBlock));

	bool blockDone = false, error = false;

	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		parser.getArgsDelim(LineParser::SkipBlanks+LineParser::UseQuotes);
		Keywords::SimulationKeyword simKeyword = Keywords::simulationKeyword(parser.argc(0));
		if ((simKeyword != Keywords::nSimulationKeywords) && ((parser.nArgs()-1) < Keywords::simulationBlockNArguments(simKeyword)))
		{
			Messenger::error("Not enough arguments given to '%s' keyword.\n", Keywords::simulationKeyword(simKeyword));
			error = true;
			break;
		}
		switch (simKeyword)
		{
			case (Keywords::BoxNormalisationPointsKeyword):
				duq->setBoxNormalisationPoints(parser.argi(1));
				break;
			case (Keywords::EndSimulationKeyword):
				Messenger::print("Found end of %s block.\n", Keywords::inputBlock(Keywords::SimulationBlock));
				blockDone = true;
				break;
			case (Keywords::SeedKeyword):
				duq->setSeed(parser.argi(1));
				break;
			case (Keywords::nSimulationKeywords):
				Messenger::print("Unrecognised %s block keyword found - '%s'\n", Keywords::inputBlock(Keywords::SimulationBlock), parser.argc(0));
				Keywords::printValidKeywords(Keywords::SimulationBlock);
				error = true;
				break;
			case (Keywords::WindowFunctionKeyword):
				if (Data2D::windowFunction(parser.argc(1)) == Data2D::nWindowFunctions)
				{
					Messenger::error("Unrecognised window function = '%s'.\n", parser.argc(1));
					error = true;
				}
				else duq->setWindowFunction(Data2D::windowFunction(parser.argc(1)));
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", Keywords::inputBlock(Keywords::SimulationBlock), Keywords::simulationKeyword(simKeyword));
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
