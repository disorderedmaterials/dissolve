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

// Return enum option info for SimulationKeyword
EnumOptions<SimulationBlock::SimulationKeyword> SimulationBlock::keywords()
{
	static EnumOptionsList SimulationKeywords = EnumOptionsList() <<
		EnumOption(SimulationBlock::EndSimulationKeyword,		"EndSimulation") <<
		EnumOption(SimulationBlock::ParallelStrategyKeyword, 		"ParallelStrategy",		1) <<
		EnumOption(SimulationBlock::ParallelGroupPopulationKeyword,	"ParallelGroupPopulation",	1) <<
		EnumOption(SimulationBlock::SeedKeyword, 			"Seed",				1);

	static EnumOptions<SimulationBlock::SimulationKeyword> options("SimulationKeyword", SimulationKeywords);

	return options;
}

// Parse Simulation block
bool SimulationBlock::parse(LineParser& parser, Dissolve* dissolve)
{
	Messenger::print("\nParsing %s block...\n", BlockKeywords::keywords().keyword(BlockKeywords::SimulationBlockKeyword));

	bool blockDone = false, error = false;

	while (!parser.eofOrBlank())
	{
		// Read in a line, which should contain a keyword and a minimum number of arguments
		if (parser.getArgsDelim() != LineParser::Success) return false;

		// Do we recognise this keyword and, if so, do we have the appropriate number of arguments?
		if (!keywords().isValid(parser.argc(0))) return keywords().errorAndPrintValid(parser.argc(0));
		SimulationKeyword kwd = keywords().enumeration(parser.argc(0));
		if (!keywords().validNArgs(kwd, parser.nArgs()-1)) return false;

		// All OK, so process the keyword
		switch (kwd)
		{
			case (SimulationBlock::EndSimulationKeyword):
				Messenger::print("Found end of %s block.\n", BlockKeywords::keywords().keyword(BlockKeywords::SimulationBlockKeyword));
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
			case (SimulationBlock::ParallelGroupPopulationKeyword):
				dissolve->setParallelGroupPopulation(parser.argi(1));
				break;
			case (SimulationBlock::SeedKeyword):
				dissolve->setSeed(parser.argi(1));
				Messenger::print("Random seed set to %i.\n", dissolve->seed());
				break;
			default:
				printf("DEV_OOPS - %s block keyword '%s' not accounted for.\n", BlockKeywords::keywords().keyword(BlockKeywords::SimulationBlockKeyword), keywords().keyword(kwd));
				error = true;
				break;
		}
		
		// Error encountered?
		if (error) break;
		
		// End of block?
		if (blockDone) break;
	}

	// If there's no error and the blockDone flag isn't set, return an error
	if (!error && !blockDone)
	{
		Messenger::error("Unterminated %s block found.\n", BlockKeywords::keywords().keyword(BlockKeywords::SimulationBlockKeyword));
		error = true;
	}

	return (!error);
}
