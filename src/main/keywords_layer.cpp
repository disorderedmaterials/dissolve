/*
    *** Keyword Parsing - Layer Block
    *** src/main/keywords_layer.cpp
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
#include "module/layer.h"

// Return enum option info for LayerKeyword
EnumOptions<LayerBlock::LayerKeyword> LayerBlock::keywords()
{
    static EnumOptionsList LayerKeywords =
        EnumOptionsList() << EnumOption(LayerBlock::DisabledKeyword, "Disabled")
                          << EnumOption(LayerBlock::EndLayerKeyword, "EndLayer")
                          << EnumOption(LayerBlock::FrequencyKeyword, "Frequency", 1)
                          << EnumOption(LayerBlock::ModuleKeyword, "Module", EnumOption::OptionalSecondArgument);

    static EnumOptions<LayerBlock::LayerKeyword> options("LayerKeyword", LayerKeywords);

    return options;
}

// Parse Layer block
bool LayerBlock::parse(LineParser &parser, Dissolve *dissolve, ModuleLayer *layer)
{
    Messenger::print("\nParsing {} block '{}'...\n", BlockKeywords::keywords().keyword(BlockKeywords::LayerBlockKeyword),
                     layer->name());

    auto blockDone = false, error = false;
    Module *module = nullptr;
    std::string niceName;

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
            case (LayerBlock::DisabledKeyword):
                layer->setEnabled(false);
                break;
            case (LayerBlock::EndLayerKeyword):
                Messenger::print("Found end of {} block.\n",
                                 BlockKeywords::keywords().keyword(BlockKeywords::LayerBlockKeyword));
                blockDone = true;
                break;
            case (LayerBlock::FrequencyKeyword):
                layer->setFrequency(parser.argi(1));
                break;
            case (LayerBlock::ModuleKeyword):
                // The argument following the keyword is the module name, so try to create an instance of that
                // Module
                module = dissolve->createModuleInstance(parser.argsv(1));
                if (!module)
                {
                    error = true;
                    break;
                }

                // Add the new instance to the processing list
                if (!layer->own(module))
                {
                    Messenger::error("Failed to add Module '{}' as processing layer task.\n", parser.argsv(1));
                    error = true;
                }
                if (error)
                    break;

                // Set unique name, if it was provided - need to check if it has been used elsewhere (in any
                // Module or instance of it)
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
                module->setConfigurationLocal(false);
                if (!ModuleBlock::parse(parser, dissolve, module, dissolve->processingModuleData(), false))
                    error = true;
                else if (!module->setUp(*dissolve, dissolve->worldPool()))
                    error = true;
                if (error)
                    break;
                break;
            default:
                Messenger::error("{} block keyword '{}' not accounted for.\n",
                                 BlockKeywords::keywords().keyword(BlockKeywords::LayerBlockKeyword), keywords().keyword(kwd));
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
        Messenger::error("Unterminated {} block found.\n", BlockKeywords::keywords().keyword(BlockKeywords::LayerBlockKeyword));
        error = true;
    }

    return (!error);
}
