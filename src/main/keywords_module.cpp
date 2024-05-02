// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
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
    Messenger::print("\nParsing {} block '{}' ({})...\n", BlockKeywords::keywords().keyword(BlockKeywords::ModuleBlockKeyword),
                     module->name(), ModuleTypes::moduleType(module->type()));

    auto blockDone = false, errorsEncountered = false;

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
            {
                errorsEncountered = true;
                continue;
            }

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
                    errorsEncountered = true;
                    break;
            }
        }
        else
        {
            // Might be a keyword defined in the Module itself?
            auto result = module->keywords().deserialise(parser, dissolve->coreData());
            if (result == KeywordBase::ParseResult::Unrecognised)
            {
                Messenger::error("Unrecognised {} block keyword '{}' found, and the Module '{}' contains no "
                                 "option with this name.\n",
                                 BlockKeywords::keywords().keyword(BlockKeywords::ModuleBlockKeyword), parser.argsv(0),
                                 ModuleTypes::moduleType(module->type()));
                keywords().errorAndPrintValid(parser.argsv(0));
                module->printValidKeywords();
                errorsEncountered = true;
            }
            else if (result == KeywordBase::ParseResult::Deprecated)
                Messenger::warn("The '{}' keyword is deprecated and will be removed in a future version.\n", parser.argsv(0));
            else if (result == KeywordBase::ParseResult::Failed)
                errorsEncountered = true;
        }

        // End of block?
        if (blockDone)
            break;
    }

    // If there's no errorsEncountered and the blockDone flag isn't set, return an errorsEncountered
    if (!errorsEncountered && !blockDone)
    {
        Messenger::error("Unterminated {} block found.\n",
                         BlockKeywords::keywords().keyword(BlockKeywords::ModuleBlockKeyword));
        errorsEncountered = true;
    }

    return (!errorsEncountered);
}
