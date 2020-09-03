/*
    *** Keyword Parsing - Configuration Block
    *** src/main/keywords_configuration.cpp
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
#include "classes/configuration.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "main/keywords.h"

// Return enum option info for ConfigurationKeyword
EnumOptions<ConfigurationBlock::ConfigurationKeyword> ConfigurationBlock::keywords()
{
    static EnumOptionsList ConfigurationKeywords =
        EnumOptionsList() << EnumOption(ConfigurationBlock::CellDivisionLengthKeyword, "CellDivisionLength", 1)
                          << EnumOption(ConfigurationBlock::EndConfigurationKeyword, "EndConfiguration")
                          << EnumOption(ConfigurationBlock::GeneratorKeyword, "Generator")
                          << EnumOption(ConfigurationBlock::InputCoordinatesKeyword, "InputCoordinates", 2)
                          << EnumOption(ConfigurationBlock::ModuleKeyword, "Module", EnumOption::OptionalSecondArgument)
                          << EnumOption(ConfigurationBlock::SizeFactorKeyword, "SizeFactor", 1)
                          << EnumOption(ConfigurationBlock::TemperatureKeyword, "Temperature", 1);

    static EnumOptions<ConfigurationBlock::ConfigurationKeyword> options("ConfigurationKeyword", ConfigurationKeywords);

    return options;
}

// Parse Configuration block
bool ConfigurationBlock::parse(LineParser &parser, Dissolve *dissolve, Configuration *cfg)
{
    Messenger::print("\nParsing {} block '{}'...\n",
                     BlockKeywords::keywords().keyword(BlockKeywords::ConfigurationBlockKeyword), cfg->name());

    Species *sp;
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
                if (!cfg->generator().read(parser, dissolve->coreData()))
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
            case (ConfigurationBlock::ModuleKeyword):
                // The argument following the keyword is the module name, so try to create an instance of that
                // Module
                module = dissolve->createModuleInstance(parser.argsv(1));
                if (!module)
                {
                    error = true;
                    break;
                }

                // Add the new instance to the current Configuration
                if (cfg->ownModule(module))
                {
                    // Add our pointer to the Module's list of associated Configurations
                    if (!module->addTargetConfiguration(cfg))
                    {
                        Messenger::error("Failed to add Configuration '{}' to Module '{}' as a target.\n", cfg->name(),
                                         module->type());
                        error = true;
                    }
                }
                else
                {
                    Messenger::error("Failed to add Module '{}' to Configuration.\n", parser.argsv(1));
                    error = true;
                }
                if (error)
                    break;

                // Set unique name, if it was provided - need to check if it has been used elsewhere (in any
                // Module or instance of it, or any Configuration)
                if (parser.hasArg(2))
                {
                    niceName = DissolveSys::niceName(parser.argsv(2));
                    Module *existingModule = dissolve->findModuleInstance(niceName);
                    if (existingModule && (existingModule != module))
                    {
                        Messenger::error("A Module with the unique name '{}' already exist.\n", niceName);
                        error = true;
                        break;
                    }
                    else if (dissolve->findConfigurationByNiceName(niceName))
                    {
                        Messenger::error("A Configuration with the unique name '{}' already exist, and so "
                                         "cannot be used as a Module name.\n",
                                         niceName);
                        error = true;
                        break;
                    }
                    else
                        module->setUniqueName(niceName);
                }

                // Parse rest of Module block
                module->setConfigurationLocal(true);
                if (!ModuleBlock::parse(parser, dissolve, module, cfg->moduleData(), true))
                    error = true;
                else if (!module->setUp(*dissolve, dissolve->worldPool()))
                    error = true;
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
