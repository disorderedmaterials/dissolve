// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "main/dissolve.h"
#include "main/keywords.h"
#include "module/layer.h"
#include "modules/registry.h"

// Return enum option info for LayerKeyword
EnumOptions<LayerBlock::LayerKeyword> LayerBlock::keywords()
{
    return EnumOptions<LayerBlock::LayerKeyword>("LayerKeyword",
                                                 {{LayerBlock::DisabledKeyword, "Disabled"},
                                                  {LayerBlock::EndLayerKeyword, "EndLayer"},
                                                  {LayerBlock::FrequencyKeyword, "Frequency", 1},
                                                  {LayerBlock::ModuleKeyword, "Module", OptionArguments::OptionalSecond},
                                                  {LayerBlock::RequireEnergyStabilityKeyword, "RequireEnergyStability"},
                                                  {LayerBlock::RequireNoSizeFactorsKeyword, "RequireNoSizeFactors"}});
}

// Parse Layer block
bool LayerBlock::parse(LineParser &parser, Dissolve *dissolve, ModuleLayer *layer)
{
    Messenger::print("\nParsing {} block '{}'...\n", BlockKeywords::keywords().keyword(BlockKeywords::LayerBlockKeyword),
                     layer->name());

    auto blockDone = false, error = false;
    Module *module = nullptr;
    std::string niceName, moduleType;

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
                layer->runControlFlags().setFlag(ModuleLayer::RunControlFlag::Disabled);
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
                // The argument following the keyword is the module type, so try to create an instance of that type
                try
                {
                    // In case a legacy module name is given, attempt to map the provided moduleType to the current namespace.
                    moduleType = std::string(parser.argsv(1));
                    const std::map<std::string, std::string> legacyNameMap = {{"CalculateAngle", "Angle"},
                                                                              {"CalculateAvgMol", "AvgMol"},
                                                                              {"CalculateAxisAngle", "AxisAngle"},
                                                                              {"CalculateDAngle", "DAngle"},
                                                                              {"CalculateRDF", "SiteRDF"},
                                                                              {"CalculateSDF", "SDF"},
                                                                              {"RDF", "GR"}};
                    auto legacyIt = legacyNameMap.find(moduleType);
                    if (legacyIt != legacyNameMap.end())
                    {
                        moduleType = legacyIt->second;
                        Messenger::warn("Legacy module name '{}' converted to '{}'...\n", legacyIt->first, moduleType);
                    }

                    module = ModuleRegistry::create(moduleType, layer);
                }
                catch (...)
                {
                    Messenger::error("Module type '{}' does not exist.\n", moduleType);
                    error = true;
                    break;
                }

                // Set unique name, if it was provided - need to check if it has been used elsewhere
                if (parser.hasArg(2))
                {
                    niceName = DissolveSys::niceName(parser.argsv(2));
                    auto *existingModule = Module::find(niceName);
                    if (existingModule && (existingModule != module))
                    {
                        Messenger::error("A Module with the unique name '{}' already exists.\n", niceName);
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
                        module->setName(niceName);
                }

                // Parse rest of Module block
                if (!ModuleBlock::parse(parser, dissolve, module, dissolve->processingModuleData(), false))
                    error = true;
                if (error)
                    break;
                break;
            case (LayerBlock::RequireEnergyStabilityKeyword):
                layer->runControlFlags().setFlag(ModuleLayer::RunControlFlag::EnergyStability);
                break;
            case (LayerBlock::RequireNoSizeFactorsKeyword):
                layer->runControlFlags().setFlag(ModuleLayer::RunControlFlag::SizeFactors);
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
