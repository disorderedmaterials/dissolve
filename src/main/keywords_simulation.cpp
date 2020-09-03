/*
    *** Keyword Parsing - Simulation Block
    *** src/main/keywords_simulation.cpp
    Copyright T. Youngs 2012-2020

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

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "main/dissolve.h"
#include "main/keywords.h"

// Return enum option info for SimulationKeyword
EnumOptions<SimulationBlock::SimulationKeyword> SimulationBlock::keywords()
{
    static EnumOptionsList SimulationKeywords =
        EnumOptionsList() << EnumOption(SimulationBlock::EndSimulationKeyword, "EndSimulation")
                          << EnumOption(SimulationBlock::ParallelStrategyKeyword, "ParallelStrategy", 1)
                          << EnumOption(SimulationBlock::ParallelGroupPopulationKeyword, "ParallelGroupPopulation", 1)
                          << EnumOption(SimulationBlock::SeedKeyword, "Seed", 1);

    static EnumOptions<SimulationBlock::SimulationKeyword> options("SimulationKeyword", SimulationKeywords);

    return options;
}

// Parse Simulation block
bool SimulationBlock::parse(LineParser &parser, Dissolve *dissolve)
{
    Messenger::print("\nParsing {} block...\n", BlockKeywords::keywords().keyword(BlockKeywords::SimulationBlockKeyword));

    auto blockDone = false, error = false;

    while (!parser.eofOrBlank())
    {
        // Read in a line, which should contain a keyword and a minimum number of arguments
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        if (!keywords().isValid(parser.argsv(0)))
            return keywords().errorAndPrintValid(parser.argsv(0));
        auto kwd = keywords().enumeration(parser.argsv(0));
        if (!keywords().validNArgs(kwd, parser.nArgs() - 1))
            return false;

        // All OK, so process the keyword
        switch (kwd)
        {
            case (SimulationBlock::EndSimulationKeyword):
                Messenger::print("Found end of {} block.\n",
                                 BlockKeywords::keywords().keyword(BlockKeywords::SimulationBlockKeyword));
                blockDone = true;
                break;
            case (SimulationBlock::ParallelStrategyKeyword):
                if (Dissolve::parallelStrategy(parser.argsv(1)) == Dissolve::nParallelStrategies)
                {
                    Messenger::error("Unrecognised parallel strategy '{}'.\n", parser.argsv(1));
                    error = true;
                }
                else
                    dissolve->setParallelStrategy(Dissolve::parallelStrategy(parser.argsv(1)));
                break;
            case (SimulationBlock::ParallelGroupPopulationKeyword):
                dissolve->setParallelGroupPopulation(parser.argi(1));
                break;
            case (SimulationBlock::SeedKeyword):
                dissolve->setSeed(parser.argi(1));
                Messenger::print("Random seed set to {}.\n", dissolve->seed());
                break;
            default:
                Messenger::error("{} block keyword '{}' not accounted for.\n",
                                 BlockKeywords::keywords().keyword(BlockKeywords::SimulationBlockKeyword),
                                 keywords().keyword(kwd));
                error = true;
                break;
        }

        // Error encountered?
        if (error)
            break;

        // End of block?
        if (blockDone)
            break;
    }

    // If there's no error and the blockDone flag isn't set, return an error
    if (!error && !blockDone)
    {
        Messenger::error("Unterminated {} block found.\n",
                         BlockKeywords::keywords().keyword(BlockKeywords::SimulationBlockKeyword));
        error = true;
    }

    return (!error);
}
