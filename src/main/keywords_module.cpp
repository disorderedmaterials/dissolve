/*
    *** Keyword Parsing - Module Block
    *** src/main/keywords_module.cpp
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
#include "classes/species.h"
#include "genericitems/listhelper.h"
#include "main/dissolve.h"
#include "main/keywords.h"

// Return enum option info for ModuleKeyword
EnumOptions<ModuleBlock::ModuleKeyword> ModuleBlock::keywords()
{
    static EnumOptionsList ModuleKeywords = EnumOptionsList()
                                            << EnumOption(ModuleBlock::ConfigurationKeyword, "Configuration", 1)
                                            << EnumOption(ModuleBlock::DisableKeyword, "Disabled")
                                            << EnumOption(ModuleBlock::EndModuleKeyword, "EndModule")
                                            << EnumOption(ModuleBlock::FrequencyKeyword, "Frequency", 1);

    static EnumOptions<ModuleBlock::ModuleKeyword> options("ModuleKeyword", ModuleKeywords);

    return options;
}

// Parse Module block
bool ModuleBlock::parse(LineParser &parser, Dissolve *dissolve, Module *module, GenericList &targetList,
                        bool moduleInConfiguration)
{
    Messenger::print("\nParsing {} block '{}'...\n", BlockKeywords::keywords().keyword(BlockKeywords::ModuleBlockKeyword),
                     module->type());

    Configuration *targetCfg;
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
                case (ModuleBlock::ConfigurationKeyword):
                    // Find the named Configuration
                    targetCfg = dissolve->findConfiguration(parser.argsv(1));
                    if (!targetCfg)
                    {
                        Messenger::error("Can't associate Configuration '{}' to the Module '{}', since no "
                                         "Configuration by this name exists.\n",
                                         parser.argsv(1), module->type());
                        error = true;
                        break;
                    }

                    // Add it as a target
                    if (!module->addTargetConfiguration(targetCfg))
                    {
                        Messenger::error("Failed to add Configuration target in Module '{}'.\n", module->type());
                        error = true;
                        break;
                    }

                    // Create weight data if a second argument was provided
                    if (parser.hasArg(2))
                        GenericListHelper<double>::add(targetList, fmt::format("ConfigurationWeight_{}", targetCfg->niceName()),
                                                       module->uniqueName()) = parser.argd(2);
                    break;
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
                case (ModuleBlock::nModuleKeywords):
                    // Never used, since it is accounted for in the beginning 'if'
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
