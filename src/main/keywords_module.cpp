// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "main/keywords.h"

// Return enum option info for ModuleKeyword
EnumOptions<ModuleBlock::ModuleKeyword> ModuleBlock::keywords()
{
    return EnumOptions<ModuleBlock::ModuleKeyword>("ModuleKeyword", {{ModuleBlock::DisableKeyword, "Disabled"},
                                                                     {ModuleBlock::EndModuleKeyword, "EndModule"},
                                                                     {ModuleBlock::FrequencyKeyword, "Frequency", 1}});
}

// Parse Module block
bool ModuleBlock::parse(LineParser &parser, Dissolve *dissolve, Module *module, GenericList &targetList,
                        bool moduleInConfiguration)
{
    Messenger::print("\nParsing {} block '{}'...\n", BlockKeywords::keywords().keyword(BlockKeywords::ModuleBlockKeyword),
                     module->type());

    auto blockDone = false, error = false;

    while (!parser.eofOrBlank())
    {
        // Read in a line, which should contain a keyword and a minimum number of arguments
        if (parser.getArgsDelim() != LineParser::Success)
            return false;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        if (keywords().isValid(parser.argsv(0)))
        {
            auto kwd = keywords().enumeration(parser.argsv(0));
            if (!keywords().validNArgs(kwd, parser.nArgs() - 1))
                return false;

            // All OK, so process the keyword
            switch (kwd)
            {
                case (ModuleBlock::DisableKeyword):
                    module->setEnabled(false);
                    break;
                case (ModuleBlock::EndModuleKeyword):
                    Messenger::print("Found end of {} block.\n",
                                     BlockKeywords::keywords().keyword(BlockKeywords::ModuleBlockKeyword));
                    blockDone = true;
                    break;
                case (ModuleBlock::FrequencyKeyword):
                    module->setFrequency(parser.argi(1));
                    break;
                default:
                    Messenger::error("{} block keyword '{}' not accounted for.\n",
                                     BlockKeywords::keywords().keyword(BlockKeywords::ModuleBlockKeyword),
                                     keywords().keyword(kwd));
                    error = true;
                    break;
            }
        }
        else
        {
            // Might be a keyword defined in the Module itself?
            auto result = module->parseKeyword(parser, dissolve, targetList, std::string(module->uniqueName()));
            if (result == 0)
                error = true;
            else if (result == -1)
            {
                Messenger::error("Unrecognised {} block keyword '{}' found, and the Module '{}' contains no "
                                 "option with this name.\n",
                                 BlockKeywords::keywords().keyword(BlockKeywords::ModuleBlockKeyword), parser.argsv(0),
                                 module->type());
                keywords().errorAndPrintValid(parser.argsv(0));
                module->printValidKeywords();
                error = true;
            }
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
                         BlockKeywords::keywords().keyword(BlockKeywords::ModuleBlockKeyword));
        error = true;
    }

    return (!error);
}
