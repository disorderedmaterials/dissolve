// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "base/lineParser.h"
#include "base/messenger.h"
#include "base/serialiser.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/species.h"
#include "data/isotopes.h"
#include "main/compatibility.h"
#include "main/dissolve.h"
#include "main/keywords.h"
#include "main/version.h"
#include <cstring>
#include <fstream>
#include <toml/parser.hpp>
#include <functional>
#include <map>

// Load input file through supplied parser
bool Dissolve::loadInput(LineParser &parser)
{
    // Clear all existing data before we begin
    clear();

    // Variables
    Configuration *cfg;
    ModuleLayer *layer = nullptr;
    Species *sp;
    auto errorsEncountered = false;

    while (!parser.eofOrBlank())
    {
        // Master will read the next line from the file, and broadcast it to slaves (who will then parse it)
        if (parser.getArgsDelim() != LineParser::Success)
            break;

        // Do we recognise this keyword and, if so, do we have an appropriate number of arguments?
        if (!BlockKeywords::keywords().isValid(parser.argsv(0)))
        {
            BlockKeywords::keywords().errorAndPrintValid(parser.argsv(0));
            errorsEncountered = true;
            continue;
        }
        auto kwd = BlockKeywords::keywords().enumeration(parser.argsv(0));

        // All OK, so process the keyword
        switch (kwd)
        {
            case (BlockKeywords::ConfigurationBlockKeyword):
                // Check to see if a Configuration with this name already exists...
                if (coreData_.findConfiguration(parser.argsv(1)))
                    return Messenger::error("Redefinition of Configuration '{}'.\n", parser.argsv(1));

                cfg = coreData_.addConfiguration();
                cfg->setName(parser.argsv(1));
                Messenger::print("\n--> Created Configuration '{}'\n", cfg->name());
                if (!ConfigurationBlock::parse(parser, this, cfg))
                    errorsEncountered = true;
                break;
            case (BlockKeywords::LayerBlockKeyword):
                // Check to see if a processing layer with this name already exists...
                if (coreData_.findProcessingLayer(parser.argsv(1)))
                    Messenger::error("Redefinition of processing layer '{}'.\n", parser.argsv(1));

                layer = coreData_.addProcessingLayer();
                layer->setName(parser.argsv(1));
                Messenger::print("\n--> Created processing layer '{}'\n", layer->name());
                if (!LayerBlock::parse(parser, this, layer))
                    errorsEncountered = true;
                break;
            case (BlockKeywords::MasterBlockKeyword):
                if (!MasterBlock::parse(parser, coreData_))
                    errorsEncountered = true;
                break;
            case (BlockKeywords::PairPotentialsBlockKeyword):
                if (!PairPotentialsBlock::parse(parser, this))
                    errorsEncountered = true;
                break;
            case (BlockKeywords::SpeciesBlockKeyword):
                // Check to see if a Species with this name already exists...
                if (coreData_.findSpecies(parser.argsv(1)))
                    return Messenger::error("Redefinition of species '{}'.\n", parser.argsv(1));

                sp = coreData_.addSpecies();
                sp->setName(parser.argsv(1));
                Messenger::print("\n--> Created Species '{}'\n", sp->name());
                if (!sp->read(parser, coreData_))
                    errorsEncountered = true;
                else if (Messenger::isVerbose())
                {
                    Messenger::print("\n--- Species '{}'...\n", sp->name());
                    sp->print();
                }
                break;
            default:
                Messenger::error("Block keyword '{}' is not relevant in this context.\n",
                                 BlockKeywords::keywords().keyword(kwd));
                errorsEncountered = true;
                break;
        }
    }

    // Error encountered?
    if (errorsEncountered)
        Messenger::error("Errors encountered while parsing input.");

    // Done
    parser.closeFiles();

    return (!errorsEncountered);
}

// Load input from supplied string
bool Dissolve::loadInputFromString(std::string_view inputString)
{
    // Set strings and check that we're OK to proceed reading from them
    LineParser parser(&worldPool());
    if (!parser.openInputString(inputString))
        return false;

    auto result = loadInput(parser);

    if (result)
        Messenger::print("Finished reading input.\n");

    return result;
}

// Serialise pair potential
SerialisedValue Dissolve::serialisePairPotentials() const
{
    SerialisedValue pairPotentials = {
        {"range", pairPotentialRange_},
        {"delta", pairPotentialDelta_},
        {"autoChargeSource", automaticChargeSource_},
        {"coulombTruncation", PairPotential::coulombTruncationSchemes().serialise(PairPotential::coulombTruncationScheme_)},
        {"shortRangeTruncation",
         PairPotential::shortRangeTruncationSchemes().serialise(PairPotential::shortRangeTruncationScheme_)}};
    if (forceChargeSource_)
        pairPotentials["forceChargeSource"] = true;
    if (atomTypeChargeSource_)
        pairPotentials["includeCoulomb"] = true;
    Serialisable::fromVectorToTable(coreData_.atomTypes(), "atomTypes", pairPotentials);
    return pairPotentials;
}

// Express as a serialisable value
SerialisedValue Dissolve::serialise() const
{
    SerialisedValue root;

    root["version"] = Version::semantic();

    if (!coreData_.masterBonds().empty() || !coreData_.masterAngles().empty() || !coreData_.masterTorsions().empty() ||
        !coreData_.masterImpropers().empty())
        root["master"] = coreData_.serialiseMaster();

    Serialisable::fromVectorToTable<>(coreData_.species(), "species", root);

    root["pairPotentials"] = serialisePairPotentials();

    Serialisable::fromVectorToTable(coreData_.configurations(), "configurations", root);

    Serialisable::fromVectorToTable(coreData_.processingLayers(), "layers", root);

    return root;
}

// This method populates the object's members with values read from a 'pairPotentials' TOML node
void Dissolve::deserialisePairPotentials(const SerialisedValue &node)
{
    pairPotentialRange_ = toml::find_or<double>(node, "range", 15.0);
    pairPotentialDelta_ = toml::find_or<double>(node, "delta", 0.005);
    atomTypeChargeSource_ = toml::find_or<bool>(node, "includeCoulomb", false);
    forceChargeSource_ = toml::find_or<bool>(node, "forceChargeSource", false);
    automaticChargeSource_ = toml::find_or<bool>(node, "autoChargeSource", true);

    PairPotential::coulombTruncationScheme_ =
        PairPotential::coulombTruncationSchemes().deserialise(toml::find_or<std::string>(node, "coulombTruncation", "Shifted"));
    PairPotential::shortRangeTruncationScheme_ = PairPotential::shortRangeTruncationSchemes().deserialise(
        toml::find_or<std::string>(node, "shortRangeTruncation", "Shifted"));

    toMap(node, "atomTypes",
          [this](const std::string &name, const auto &data)
          { coreData().atomTypes().emplace_back(std::make_unique<AtomType>(name))->deserialise(data); });
}

// Read values from a serialisable value
void Dissolve::deserialise(const SerialisedValue &originalNode)
{
    // Default to current version if no version info is given.
    auto hasVersion = originalNode.contains("version");
    if (!hasVersion)
        Messenger::warn("File does not contain version information.  Assuming the current version: {}", Version::semantic());
    const SerialisedValue node = hasVersion ? dissolve::backwardsUpgrade(originalNode) : originalNode;

    Serialisable::optionalOn(node, "pairPotentials", [this](const auto node) { deserialisePairPotentials(node); });
    Serialisable::optionalOn(node, "master", [this](const auto node) { coreData_.deserialiseMaster(node); });

    toMap(node, "species",
          [this](const std::string &name, const SerialisedValue &data)
          { coreData_.species().emplace_back(std::make_unique<Species>(name))->deserialise(data, coreData_); });

    toMap(node, "configurations",
          [this](const std::string &name, const SerialisedValue &data)
          {
              auto *cfg = coreData_.addConfiguration();
              cfg->setName(name);
              cfg->deserialise(data, coreData_);
          });

    toMap(node, "layers",
          [this](const std::string &name, const SerialisedValue &data)
          {
              auto *layer = coreData_.addProcessingLayer();
              layer->setName(name);
              layer->deserialise(data, coreData_);
          });
}

// Load input from supplied file
bool Dissolve::loadInput(std::string_view filename)
{
    // If the file name ends in TOML, insist on a TOML parse
    if (filename.find(".toml") == filename.size() - 5)
    {
        try
        {
            SerialisedValue contents = toml::parse(std::string(filename));
            deserialise(contents);
            return true;
        }
        catch (toml::syntax_error e)
        {
            Messenger::error("Syntax error in TOML file (are you sure you meant the .toml extension?).\n\n{}", e.what());
        }
        catch (toml::type_error e)
        {
            Messenger::error("Could not load TOML file\n\n{}", e.what());
        }
        return false;
    }

    // Fail if the file starts with restart header
    {
        std::ifstream infile{std::string(filename)};
        std::string firstLine;
        infile >> firstLine;
        infile.close();
        if (firstLine.find("# Restart file") == 0)
        {
            Messenger::error("File {} is a restart file and not an input file", filename);
            return false;
        }
    }

    try
    {
        SerialisedValue contents = toml::parse(std::string(filename));
        deserialise(contents);
        return true;
    }
    catch (toml::syntax_error e)
    {
        // The file didn't have TOML syntax, so try the original parser
        // Open file and check that we're OK to proceed reading from it
        LineParser parser(&worldPool());
        if (!parser.openInput(filename))
            return false;

        auto result = loadInput(parser);
        if (result)
        {
            Messenger::print("Finished reading input file.\n");
            setInputFilename(filename);
        }

        return result;
    }
    catch (toml::type_error e)
    {
        // The file *was* a TOML file, but it had problems loading
        Messenger::error("Could not load TOML file\n\n{}", e.what());
    }
    return false;
}

// Save input file
bool Dissolve::saveInput(std::string_view filename)
{
    // Open file
    LineParser parser;

    if (!parser.openOutput(filename, true) || (!parser.isFileGoodForWriting()))
    {
        Messenger::error("Couldn't open output file '{}' to save new input file.\n", filename);
        return false;
    }

    // Write title comment
    if (!parser.writeLineF("# Input file written by Dissolve v{} at {}.\n", Version::info(), DissolveSys::currentTimeAndDate()))
        return false;

    // Write master terms
    if (coreData_.nMasterBonds() || coreData_.nMasterAngles() || coreData_.nMasterTorsions() || coreData_.nMasterImpropers())
    {
        if (!parser.writeBannerComment("Master Terms"))
            return false;
        if (!parser.writeLineF("\n{}\n", BlockKeywords::keywords().keyword(BlockKeywords::MasterBlockKeyword)))
            return false;

        for (auto &b : coreData_.masterBonds())
            if (!parser.writeLineF("  {}  '{}'  {}  {}\n", MasterBlock::keywords().keyword(MasterBlock::BondKeyword), b->name(),
                                   BondFunctions::forms().keyword(b->interactionForm()),
                                   b->interactionPotential().parametersAsString()))
                return false;

        for (auto &a : coreData_.masterAngles())
            if (!parser.writeLineF("  {}  '{}'  {}  {}\n", MasterBlock::keywords().keyword(MasterBlock::AngleKeyword),
                                   a->name(), AngleFunctions::forms().keyword(a->interactionForm()),
                                   a->interactionPotential().parametersAsString()))
                return false;

        auto elec14Scaling = 0.5, vdw14Scaling = 0.5;
        for (auto &t : coreData_.masterTorsions())
        {
            // Write new 1-4 scale factor line if this torsion has different values
            if ((t->electrostatic14Scaling() != elec14Scaling || t->vanDerWaals14Scaling() != vdw14Scaling) &&
                !parser.writeLineF(fmt::format("  {}  {}  {}\n", MasterBlock::keywords().keyword(MasterBlock::Scaling14Keyword),
                                               t->electrostatic14Scaling(), t->vanDerWaals14Scaling())))
                return false;

            if (!parser.writeLineF("  {}  '{}'  {}  {}\n", MasterBlock::keywords().keyword(MasterBlock::TorsionKeyword),
                                   t->name(), TorsionFunctions::forms().keyword(t->interactionForm()),
                                   t->interactionPotential().parametersAsString()))
                return false;

            elec14Scaling = t->electrostatic14Scaling();
            vdw14Scaling = t->vanDerWaals14Scaling();
        }

        for (auto &imp : coreData_.masterImpropers())
            if (!parser.writeLineF("  {}  '{}'  {}  {}\n", MasterBlock::keywords().keyword(MasterBlock::ImproperKeyword),
                                   imp->name(), TorsionFunctions::forms().keyword(imp->interactionForm()),
                                   imp->interactionPotential().parametersAsString()))
                return false;

        // Done with the master terms
        if (!parser.writeLineF("{}\n", MasterBlock::keywords().keyword(MasterBlock::EndMasterKeyword)))
            return false;
    }

    // Write Species data
    parser.writeBannerComment("Species");
    for (auto &sp : coreData_.species())
    {
        if (!parser.writeLineF("\n"))
            return false;
        if (!sp->write(parser, ""))
            return false;
    }

    // Write PairPotentials block
    if (!parser.writeBannerComment("Pair Potentials"))
        return false;
    if (!parser.writeLineF("\n{}\n", BlockKeywords::keywords().keyword(BlockKeywords::PairPotentialsBlockKeyword)))
        return false;

    // Atom Type Parameters
    if (!parser.writeLineF("  # Atom Type Parameters\n"))
        return false;
    for (const auto &atomType : coreData_.atomTypes())
        if (!parser.writeLineF("  {}  {}  {}  {:12.6e}  {}  {}\n",
                               PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::ParametersKeyword),
                               atomType->name(), Elements::symbol(atomType->Z()), atomType->charge(),
                               ShortRangeFunctions::forms().keyword(atomType->interactionPotential().form()),
                               atomType->interactionPotential().parametersAsString()))
            return false;

    if (!parser.writeLineF("  {}  {}\n", PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::RangeKeyword),
                           pairPotentialRange_))
        return false;
    if (!parser.writeLineF("  {}  {}\n", PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::DeltaKeyword),
                           pairPotentialDelta_))
        return false;
    if (!automaticChargeSource_)
    {
        if (!parser.writeLineF("  {}  {}\n",
                               PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::ManualChargeSourceKeyword),
                               DissolveSys::btoa(true)))
            return false;
        if (!parser.writeLineF("  {}  {}\n",
                               PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::IncludeCoulombKeyword),
                               DissolveSys::btoa(atomTypeChargeSource_)))
            return false;
        if (forceChargeSource_ &&
            !parser.writeLineF("  {}  {}\n",
                               PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::ForceChargeSourceKeyword),
                               DissolveSys::btoa(true)))
            return false;
    }
    if (!parser.writeLineF("  {}  {}\n", PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::CoulombTruncationKeyword),
                           PairPotential::coulombTruncationSchemes().keyword(PairPotential::coulombTruncationScheme())))
        return false;
    if (!parser.writeLineF("  {}  {}\n",
                           PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::ShortRangeTruncationKeyword),
                           PairPotential::shortRangeTruncationSchemes().keyword(PairPotential::shortRangeTruncationScheme())))
        return false;
    if (!parser.writeLineF("{}\n", PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::EndPairPotentialsKeyword)))
        return false;

    // Write Configurations
    if (!parser.writeBannerComment("Configurations"))
        return false;
    for (auto &cfg : coreData_.configurations())
    {
        if (!parser.writeLineF("\n{}  '{}'\n", BlockKeywords::keywords().keyword(BlockKeywords::ConfigurationBlockKeyword),
                               cfg->name()))
            return false;

        // Generator
        if (!parser.writeLineF("\n  # Generator\n"))
            return false;
        if (!parser.writeLineF("  {}\n", ConfigurationBlock::keywords().keyword(ConfigurationBlock::GeneratorKeyword)))
            return false;
        if (!cfg->generator().serialise(parser, "    "))
            return false;
        if (!parser.writeLineF("  End{}\n", ConfigurationBlock::keywords().keyword(ConfigurationBlock::GeneratorKeyword)))
            return false;
        if (!parser.writeLineF("\n"))
            return false;
        if (!parser.writeLineF("  {}  {}\n", ConfigurationBlock::keywords().keyword(ConfigurationBlock::TemperatureKeyword),
                               cfg->temperature()))
            return false;

        if (!parser.writeLineF("\n"))
            return false;

        if (!parser.writeLineF("{}\n", ConfigurationBlock::keywords().keyword(ConfigurationBlock::EndConfigurationKeyword)))
            return false;
    }

    // Write processing layers
    if (!parser.writeBannerComment("Processing Layers"))
        return false;
    for (auto &layer : coreData_.processingLayers())
    {
        if (!parser.writeLineF("\n{}  '{}'\n", BlockKeywords::keywords().keyword(BlockKeywords::LayerBlockKeyword),
                               layer->name()))
            return false;

        // Write frequency and disabled lines
        if (!parser.writeLineF("  Frequency  {}\n", layer->frequency()))
            return false;
        if (layer->runControlFlags().isSet(ModuleLayer::RunControlFlag::Disabled) && (!parser.writeLineF("  Disabled\n")))
            return false;
        if (layer->runControlFlags().isSet(ModuleLayer::RunControlFlag::EnergyStability) &&
            (!parser.writeLineF("  {}\n", LayerBlock::keywords().keyword(LayerBlock::RequireEnergyStabilityKeyword))))
            return false;
        if (layer->runControlFlags().isSet(ModuleLayer::RunControlFlag::SizeFactors) &&
            (!parser.writeLineF("  {}\n", LayerBlock::keywords().keyword(LayerBlock::RequireNoSizeFactorsKeyword))))
            return false;

        for (auto &module : layer->modules())
        {
            if (!parser.writeLineF("\n  {}  {}  '{}'\n", BlockKeywords::keywords().keyword(BlockKeywords::ModuleBlockKeyword),
                                   ModuleTypes::moduleType(module->type()), module->name()))
                return false;

            // Write frequency and disabled keywords
            if (!parser.writeLineF("    Frequency  {}\n", module->frequency()))
                return false;
            if (module->isDisabled() && (!parser.writeLineF("    Disabled\n")))
                return false;

            // Write keyword options
            if (!module->keywords().serialise(parser, "    ", true))
                return false;

            if (!parser.writeLineF("  {}\n", ModuleBlock::keywords().keyword(ModuleBlock::EndModuleKeyword)))
                return false;
        }

        if (!parser.writeLineF("{}\n", LayerBlock::keywords().keyword(LayerBlock::EndLayerKeyword)))
            return false;
    }

    parser.closeFiles();

    return true;
}

// Load restart file
bool Dissolve::loadRestart(std::string_view filename)
{
    // Open file and check that we're OK to proceed reading from it
    LineParser parser(&worldPool());
    if (!parser.openInput(filename))
        return false;

    // Peek the first line and see if can determine a version
    if (parser.readNextLine(LineParser::KeepComments) != LineParser::Success)
        return false;
    GenericList::setBaseDataVersionFromString(parser.line());
    parser.rewind();

    // Variables
    Configuration *cfg;
    Module *module;
    auto error = false;

    while (!parser.eofOrBlank())
    {
        if (parser.getArgsDelim() != LineParser::Success)
            break;

        // First argument indicates the type of data
        if (DissolveSys::sameString(parser.argsv(0), "Keyword"))
        {
            // Let the user know what we are doing
            Messenger::print("Reading keyword '{}' into Module '{}'...\n", parser.argsv(2), parser.argsv(1));

            // Find the referenced Module
            auto *module = Module::find(parser.argsv(1));
            if (!module)
            {
                Messenger::error("No Module named '{}' exists.\n", parser.argsv(1));
                error = true;
                break;
            }

            // Does the Module have a keyword by this name?
            auto result = module->keywords().deserialise(parser, coreData_, 2);
            if (result == KeywordBase::ParseResult::Unrecognised)
            {
                Messenger::error("Module '{}' has no keyword '{}'.\n", parser.argsv(2));
                error = true;
                break;
            }
            else if (result == KeywordBase::ParseResult::Failed)
            {
                Messenger::error("Failed to read keyword data '{}' from restart file.\n", parser.argsv(2));
                error = true;
                break;
            }
        }
        else if (DissolveSys::sameString(parser.argsv(0), "Processing"))
        {
            // Let the user know what we are doing
            Messenger::print("Reading item '{}' ({}) into processing module data...\n", parser.argsv(1), parser.argsv(2));

            // Realise the item in the list
            processingModuleData_.deserialise(parser, coreData_, parser.args(1), parser.args(2), parser.argi(3),
                                              parser.hasArg(4) ? parser.argi(4) : 0);
        }
        else if (DissolveSys::startsWith(parser.argsv(0), "Configuration"))
        {
            // Let the user know what we are doing
            Messenger::print("Reading Configuration '{}'...\n", parser.argsv(1));

            // Find the named Configuration
            cfg = coreData_.findConfiguration(parser.argsv(1));
            if (!cfg)
            {
                Messenger::error("No Configuration named '{}' exists.\n", parser.argsv(1));
                error = true;
                break;
            }
            else if (!cfg->deserialise(parser, coreData_, pairPotentialRange_,
                                       DissolveSys::sameString(parser.argsv(0), "ConfigurationWithPotentials")))
                error = true;
        }
        else if (DissolveSys::sameString(parser.argsv(0), "Timing"))
        {
            // Let the user know what we are doing
            Messenger::print("Reading timing information for Module '{}'...\n", parser.argsv(1));

            module = Module::find(parser.argsv(1));
            if (!module)
            {
                Messenger::warn("Timing information for Module '{}' found, but no Module with this unique name "
                                "exists...\n",
                                parser.argsv(1));
                if (!SampledDouble().deserialise(parser))
                    error = true;
            }
            else if (!module->readProcessTimes(parser))
                error = true;
        }
        else
        {
            Messenger::error("Unrecognised '{}' entry in restart file.\n", parser.argsv(0));
            error = true;
        }

        // Error encountered?
        if (error)
            break;
    }

    if (!error)
        Messenger::print("Finished reading restart file.\n");

    // Set current iteration number
    iteration_ = processingModuleData_.valueOr<int>("Iteration", "Dissolve", 0);

    // Error encountered?
    if (error)
        Messenger::error("Errors encountered while loading restart file.\n");

    // Done
    if (worldPool().isWorldMaster())
        parser.closeFiles();

    return (!error);
}

// Load restart file as reference point
bool Dissolve::loadRestartAsReference(std::string_view filename, std::string_view dataSuffix)
{
    // Open file and check that we're OK to proceed reading from it (master only...)
    LineParser parser(&worldPool());
    if (!parser.openInput(filename))
        return false;

    // Peek the first line and see if can determine a version
    if (parser.readNextLine(LineParser::KeepComments) != LineParser::Success)
        return false;
    GenericList::setBaseDataVersionFromString(parser.line());
    parser.rewind();

    // Variables
    std::string newName;
    auto error = false, skipCurrentItem = false;

    while (!parser.eofOrBlank())
    {
        // Master will read the next line from the file
        if (parser.getArgsDelim() != 0)
            break;

        // First argument indicates the type of data
        if (DissolveSys::sameString(parser.argsv(0), "Keyword"))
        {
            // Let the user know what we are doing
            Messenger::print("Ignoring entry for keyword '{}' (module '{}')...\n", parser.argsv(2), parser.argsv(1));

            skipCurrentItem = true;
        }
        else if (DissolveSys::sameString(parser.argsv(0), "Processing"))
        {
            // Create new suffixed name
            newName = fmt::format("{}@{}", parser.argsv(1), dataSuffix);

            // Let the user know what we are doing
            Messenger::print("Reading item '{}' => '{}' ({}) into processing module data...\n", parser.argsv(1), newName,
                             parser.argsv(2));

            // Deserialise the item
            processingModuleData_.deserialise(parser, coreData_, newName, parser.args(2), parser.argi(3),
                                              GenericItem::IsReferencePointDataFlag);

            skipCurrentItem = false;
        }
        else if (DissolveSys::sameString(parser.argsv(0), "Configuration"))
        {
            // Let the user know what we are doing
            Messenger::print("Ignoring Configuration '{}'...\n", parser.argsv(1));

            skipCurrentItem = true;
        }
        else if (DissolveSys::sameString(parser.argsv(0), "Timing"))
        {
            // Let the user know what we are doing
            Messenger::print("Ignoring timing information for Module '{}'...\n", parser.argsv(1));

            skipCurrentItem = true;
        }
        else if (!skipCurrentItem)
        {
            Messenger::error("Unrecognised '{}' entry in restart file.\n", parser.argsv(0));
            error = true;
        }

        // Error encounterd?
        if (error)
            break;
    }

    if (!error)
        Messenger::print("Finished reading restart file.\n");

    // Error encountered?
    if (error)
        Messenger::error("Errors encountered while loading restart file.\n");

    // Done
    if (worldPool().isWorldMaster())
        parser.closeFiles();

    return (!error);
}

// Save restart file
bool Dissolve::saveRestart(std::string_view filename)
{
    // Open file
    LineParser parser;

    if (!parser.openOutput(filename, true) || (!parser.isFileGoodForWriting()))
    {
        Messenger::error("Couldn't open restart file '{}'.\n", filename);
        return false;
    }

    // Write title comment
    if (!parser.writeLineF("# Restart file written by Dissolve v{} at {}.\n", Version::info(),
                           DissolveSys::currentTimeAndDate()))
        return false;

    // Module Keyword Data
    for (const auto *module : Module::instances())
    {
        for (const auto &section : module->keywords().sections())
            for (const auto &group : section.groups())
                for (const auto &[keyword, keywordType] : group.keywords())
                    if (keywordType == KeywordBase::KeywordType::Restartable &&
                        !keyword->serialise(parser, fmt::format("Keyword  {}  {}  ", module->name(), keyword->name())))
                        return false;
    }

    // Processing Module Data
    if (!processingModuleData_.serialiseAll(parser, "Processing"))
        return false;

    // Configurations
    for (const auto &cfg : coreData_.configurations())
    {
        if (!parser.writeLineF("{}  '{}'\n",
                               (cfg->globalPotentials().empty() && cfg->targetedPotentials().empty())
                                   ? "Configuration"
                                   : "ConfigurationWithPotentials",
                               cfg->name()))
            return false;
        if (!cfg->serialise(parser))
            return false;
    }

    // Module timing information
    for (const auto *module : Module::instances())
    {
        if (!parser.writeLineF("Timing  {}\n", module->name()))
            return false;
        if (!module->processTimes().serialise(parser))
            return false;
    }

    parser.closeFiles();

    return true;
}

// Return whether an input filename has been set
bool Dissolve::hasInputFilename() const { return (!inputFilename_.empty()); }

// Set current input filename
void Dissolve::setInputFilename(std::string_view filename)
{
    inputFilename_ = filename;
    coreData_.setInputFilename(filename);

    restartFilename_ = fmt::format("{}.restart", inputFilename_);
}

// Return current input filename
std::string_view Dissolve::inputFilename() const { return inputFilename_; }

// Return restart filename
std::string_view Dissolve::restartFilename() const { return restartFilename_; }

// Return whether a restart filename has been set
bool Dissolve::hasRestartFilename() const { return (!restartFilename_.empty()); }
