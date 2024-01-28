// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/lineParser.h"
#include "main/dissolve.h"
#include "main/keywords.h"

EnumOptions<TaskBlock::TaskKeyword> TaskBlock::keywords()
{
    return EnumOptions<TaskBlock::TaskKeyword>("TaskKeyword",
                                                {{TaskBlock::EndTaskKeyword, "EndTask"},
                                                 {TaskBlock::ProcedureKeyword, "Procedure"},
                                                 {TaskBlock::TargetsKeyword, "Targets"}});
}

bool TaskBlock::parse(LineParser &parser, Dissolve *dissolve, Task* task)
{
    Messenger::print("\nParsing {} block '{}'...\n", BlockKeywords::keywords().keyword(BlockKeywords::TaskBlockKeyword),
                     task->name());

    auto blockDone = false, errorsEncountered = false;


    while (!parser.eofOrBlank())
    {
        // Read in a line, which should contain a keyword and a minimum number of arguments
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        if (!keywords().isValid(parser.argsv(0)))
        {
            keywords().errorAndPrintValid(parser.argsv(0));
            errorsEncountered = true;
            continue;
        }
        auto kwd = keywords().enumeration(parser.argsv(0));
        if (!keywords().validNArgs(kwd, parser.nArgs() - 1))
        {
            errorsEncountered = true;
            continue;
        }

        // All OK, so process the keyword
        switch (kwd)
        {
            case (TaskBlock::ProcedureKeyword):
               if (!task->procedure().deserialise(parser, dissolve->coreData()))
                {
                    Messenger::error("Errors while reading procedure for Task.\n");
                    errorsEncountered = true;
                }
                break;
            case (TaskBlock::TargetsKeyword):
                // Read in targets vector
                break;
            case (TaskBlock::EndTaskKeyword):
                Messenger::print("Found end of {} block.\n",
                                 BlockKeywords::keywords().keyword(BlockKeywords::TaskBlockKeyword));
                blockDone = true;
                break;
            default:
                Messenger::error("{} block keyword '{}' not accounted for.\n",
                                 BlockKeywords::keywords().keyword(BlockKeywords::TaskBlockKeyword),
                                 keywords().keyword(kwd));
                errorsEncountered = true;
                break;
        }

        // End of block?
        if (blockDone)
            break;
    }

    // If there's no errorsEncountered and the blockDone flag isn't set, return an errorsEncountered
    if (!errorsEncountered && !blockDone)
    {
        Messenger::error("Unterminated {} block found.\n",
                         BlockKeywords::keywords().keyword(BlockKeywords::TaskBlockKeyword));
        errorsEncountered = true;
    }

    return (!errorsEncountered);

    return true;
}