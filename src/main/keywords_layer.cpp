// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/sysFunc.h"
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

    auto blockDone = false, errorsEncountered = false;
    Module *module = nullptr;
    std::string niceName;

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
                if (ModuleTypes::moduleType(parser.argsv(1)))
		    module = ModuleRegistry::create(dissolve->coreData(), *ModuleTypes::moduleType(parser.argsv(1)), layer);
                else
                {
                    // In case a legacy module name is given, attempt to map the provided moduleType to the current namespace.
                    const std::map<std::string, ModuleTypes::ModuleType> legacyNameMap = {
                        {"CalculateAngle", ModuleTypes::Angle},
                        {"CalculateAvgMol", ModuleTypes::AvgMol},
                        {"CalculateAxisAngle", ModuleTypes::AxisAngle},
                        {"CalculateDAngle", ModuleTypes::DAngle},
                        {"CalculateRDF", ModuleTypes::SiteRDF},
                        {"CalculateSDF", ModuleTypes::SDF},
                        {"RDF", ModuleTypes::GR}};

                    auto legacyIt = legacyNameMap.find(parser.args(1));
                    if (legacyIt != legacyNameMap.end())
                    {
                        Messenger::warn("Legacy module name '{}' converted to '{}'...\n", legacyIt->first,
                                        ModuleTypes::moduleType(legacyIt->second));

                        module = ModuleRegistry::create(dissolve->coreData(), legacyIt->second, layer);
                    }
                    else
                        module = nullptr;
                }

                if (!module)
                {
                    Messenger::error("Module type '{}' does not exist.\n", parser.argsv(1));
                    errorsEncountered = true;
                    continue;
                }

                // Set unique name, if it was provided - need to check if it has been used elsewhere
                if (parser.hasArg(2))
                {
                    niceName = DissolveSys::niceName(parser.argsv(2));
                    auto *existingModule = Module::find(dissolve->coreData(), niceName);
                    if (existingModule && (existingModule != module))
                    {
                        Messenger::error("A Module with the unique name '{}' already exists.\n", niceName);
                        errorsEncountered = true;
                        break;
                    }
                    else if (dissolve->coreData().findConfigurationByNiceName(niceName))
                    {
                        Messenger::error("A Configuration with the unique name '{}' already exist, and so "
                                         "cannot be used as a Module name.\n",
                                         niceName);
                        errorsEncountered = true;
                        break;
                    }
                    else
                        module->setName(niceName);
                }

                // Parse rest of Module block
                if (!ModuleBlock::parse(parser, dissolve, module, dissolve->processingModuleData(), false))
                    errorsEncountered = true;
                if (errorsEncountered)
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
        Messenger::error("Unterminated {} block found.\n", BlockKeywords::keywords().keyword(BlockKeywords::LayerBlockKeyword));
        errorsEncountered = true;
    }

    return (!errorsEncountered);
}
