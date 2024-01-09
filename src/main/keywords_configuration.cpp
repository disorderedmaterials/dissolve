// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "main/keywords.h"

// Return enum option info for ConfigurationKeyword
EnumOptions<ConfigurationBlock::ConfigurationKeyword> ConfigurationBlock::keywords()
{
    return EnumOptions<ConfigurationBlock::ConfigurationKeyword>(
        "ConfigurationKeyword", {{ConfigurationBlock::CellDivisionLengthKeyword, "CellDivisionLength", 1},
                                 {ConfigurationBlock::EndConfigurationKeyword, "EndConfiguration"},
                                 {ConfigurationBlock::GeneratorKeyword, "Generator"},
                                 {ConfigurationBlock::SizeFactorKeyword, "SizeFactor", 1},
                                 {ConfigurationBlock::TemperatureKeyword, "Temperature", 1}});
}

// Parse Configuration block
bool ConfigurationBlock::parse(LineParser &parser, Dissolve *dissolve, Configuration *cfg)
{
    Messenger::print("\nParsing {} block '{}'...\n",
                     BlockKeywords::keywords().keyword(BlockKeywords::ConfigurationBlockKeyword), cfg->name());

    std::string niceName;
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
            case (ConfigurationBlock::CellDivisionLengthKeyword):
                cfg->setRequestedCellDivisionLength(parser.argd(1));
                break;
            case (ConfigurationBlock::EndConfigurationKeyword):
                Messenger::print("Found end of {} block.\n",
                                 BlockKeywords::keywords().keyword(BlockKeywords::ConfigurationBlockKeyword));
                blockDone = true;
                break;
            case (ConfigurationBlock::GeneratorKeyword):
                if (!cfg->generator().deserialise(parser, dissolve->coreData()))
                {
                    Messenger::error("Errors while reading generator procedure for Configuration.\n");
                    errorsEncountered = true;
                }
                break;
            case (ConfigurationBlock::SizeFactorKeyword):
                Messenger::warn("The 'SizeFactor' keyword will be deprecated in a future version.\n");
                cfg->setRequestedSizeFactor(parser.argd(1));
                break;
            case (ConfigurationBlock::TemperatureKeyword):
                Messenger::warn("The 'Temperature' keyword will be deprecated in a future version.\n");
                cfg->setTemperature(parser.argd(1));
                break;
            default:
                Messenger::error("{} block keyword '{}' not accounted for.\n",
                                 BlockKeywords::keywords().keyword(BlockKeywords::ConfigurationBlockKeyword),
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
                         BlockKeywords::keywords().keyword(BlockKeywords::ConfigurationBlockKeyword));
        errorsEncountered = true;
    }

    return (!errorsEncountered);
}
