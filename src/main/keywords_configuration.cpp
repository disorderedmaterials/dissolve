// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/sysfunc.h"
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
                                 {ConfigurationBlock::InputCoordinatesKeyword, "InputCoordinates", 2},
                                 {ConfigurationBlock::SizeFactorKeyword, "SizeFactor", 1},
                                 {ConfigurationBlock::TemperatureKeyword, "Temperature", 1}});
}

// Parse Configuration block
bool ConfigurationBlock::parse(LineParser &parser, Dissolve *dissolve, Configuration *cfg)
{
    Messenger::print("\nParsing {} block '{}'...\n",
                     BlockKeywords::keywords().keyword(BlockKeywords::ConfigurationBlockKeyword), cfg->name());

    Module *module;
    std::string niceName;
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
                    Messenger::error("Failed to read generator procedure for Configuration.\n");
                    error = true;
                }
                break;
            case (ConfigurationBlock::InputCoordinatesKeyword):
                if (!cfg->inputCoordinates().read(parser, 1,
                                                  fmt::format("End{}", ConfigurationBlock::keywords().keyword(
                                                                           ConfigurationBlock::InputCoordinatesKeyword)),
                                                  dissolve->coreData()))
                {
                    Messenger::error("Failed to set input coordinates file / format.\n");
                    error = true;
                    break;
                }
                Messenger::printVerbose("Initial coordinates will be loaded from file '{}' ({})\n",
                                        cfg->inputCoordinates().filename(), cfg->inputCoordinates().format());
                break;
            case (ConfigurationBlock::SizeFactorKeyword):
                cfg->setRequestedSizeFactor(parser.argd(1));
                break;
            case (ConfigurationBlock::TemperatureKeyword):
                cfg->setTemperature(parser.argd(1));
                break;
            default:
                Messenger::error("{} block keyword '{}' not accounted for.\n",
                                 BlockKeywords::keywords().keyword(BlockKeywords::ConfigurationBlockKeyword),
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
                         BlockKeywords::keywords().keyword(BlockKeywords::ConfigurationBlockKeyword));
        error = true;
    }

    return (!error);
}
