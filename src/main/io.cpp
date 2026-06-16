// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

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
#include "nodes/dissolve.h"
#include <cstring>
#include <fstream>
#include <functional>
#include <map>
#include <toml/parser.hpp>

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
                if (coreData_.findSpecies(DissolveSys::niceName(parser.argsv(1))))
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
    LineParser parser;
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
        {"range", PairPotential::range()},
        {"delta", PairPotential::delta()},
        {"chargeSource", PairPotential::chargeSources().serialise(PairPotential::chargeSource())},
        {"coulombTruncation", PairPotential::coulombTruncationSchemes().serialise(PairPotential::coulombTruncationScheme())},
        {"shortRangeTruncation",
         PairPotential::shortRangeTruncationSchemes().serialise(PairPotential::shortRangeTruncationScheme())}};
    if (!useCombinationRules_)
    {
        pairPotentials["useCombinationRules"] = false;
        Serialisable::fromVector(pairPotentials_, "potentials", pairPotentials,
                                 [](const auto &term)
                                 {
                                     const auto &[at1, at2, pot] = term;
                                     SerialisedValue target;
                                     pot->serialise("inner", target);
                                     auto &value = target["inner"];
                                     value["atomTypeI"] = at1->name();
                                     value["atomTypeJ"] = at2->name();
                                     return value;
                                 });
    }
    return pairPotentials;
}

// Express as a serialisable value
void Dissolve::serialise(std::string tag, SerialisedValue &target) const
{
    auto &root = target[tag];

    root["version"] = Version::semantic();

    Serialisable::fromVectorToTable<>(coreData_.species(), "species", root);

    root["pairPotentials"] = serialisePairPotentials();

    graphNode_->serialise("graph", root);

    Serialisable::fromVectorToTable(coreData_.configurations(), "configurations", root);
}

// Read pair potentials from a serialisable value
void Dissolve::deserialisePairPotentials(const SerialisedValue &node)
{
    PairPotential::setRange(toml::find_or<double>(node, "range", 15.0), toml::find_or<double>(node, "delta", 0.005));
    PairPotential::setChargeSource(
        PairPotential::chargeSources().deserialise(toml::find_or<std::string>(node, "chargeSource", "Automatic")));
    PairPotential::setCoulombTruncationScheme(PairPotential::coulombTruncationSchemes().deserialise(
        toml::find_or<std::string>(node, "coulombTruncation", "Shifted")));
    PairPotential::setShortRangeTruncationScheme(PairPotential::shortRangeTruncationSchemes().deserialise(
        toml::find_or<std::string>(node, "shortRangeTruncation", "Shifted")));

    useCombinationRules_ = toml::find_or<bool>(node, "useCombinationRules", true);
    // if (!useCombinationRules_)
    // {
    //     Serialisable::toVector(
    //         node, "potentials",
    //         [&](const SerialisedValue &potData)
    //         {
    //             // Get atom types
    //             auto at1 = coreData_.findAtomType(toml::find<std::string>(potData, "atomTypeI"));
    //             auto at2 = coreData_.findAtomType(toml::find<std::string>(potData, "atomTypeJ"));
    //             if (!at1 || !at2)
    //                 throw(toml::type_error("Non-existent atom type(s) used in pair potential.", potData.location()));
    //             auto *pot = addPairPotential(at1, at2);
    //             pot->deserialise(potData);
    //         });
    // }
    // TODO DISSOLVE2
}

// Read values from a serialisable value
void Dissolve::deserialise(const SerialisedValue &originalNode)
{
    // Default to current version if no version info is given.
    auto hasVersion = originalNode.contains("version");
    if (!hasVersion)
        Messenger::warn("File does not contain version information.  Assuming the current version: {}", Version::semantic());
    const SerialisedValue node = hasVersion ? dissolve::backwardsUpgrade(originalNode) : originalNode;

    Serialisable::optionalOn(node, "graph", [this](const auto node) { graphNode_->deserialise(node); });

    Serialisable::optionalOn(node, "pairPotentials", [this](const auto node) { deserialisePairPotentials(node); });

    toMap(node, "species", [this](const std::string &name, const SerialisedValue &data)
          { coreData_.species().emplace_back(std::make_unique<Species>(name))->deserialise(data); });

    toMap(node, "configurations",
          [this](const std::string &name, const SerialisedValue &data)
          {
              auto *cfg = coreData_.addConfiguration();
              cfg->setName(name);
              cfg->deserialise(data, coreData_);
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
        catch (toml::syntax_error &e)
        {
            Messenger::error("Syntax error in TOML file (are you sure you meant the .toml extension?).\n\n{}", e.what());
        }
        catch (toml::type_error &e)
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
    catch (toml::syntax_error &e)
    {
        // The file didn't have TOML syntax, so try the original parser
        // Open file and check that we're OK to proceed reading from it
        LineParser parser;
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
    catch (toml::type_error &e)
    {
        // The file *was* a TOML file, but it had problems loading
        Messenger::error("Could not load TOML file\n\n{}", e.what());
    }
    return false;
}

// Save TOML file
bool Dissolve::saveToml(std::string_view filename) const
{
    std::ofstream outfile;
    outfile.open(std::string(filename));
    outfile << into_toml() << std::endl;
    outfile.close();
    return true;
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
    // for (const auto &atomType : coreData_.atomTypes())
    //     if (!parser.writeLineF("  {}  {}  {}  {:12.6e}  {}  {}\n",
    //                            PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::ParametersKeyword),
    //                            atomType->name(), Elements::symbol(atomType->Z()), atomType->charge(),
    //                            ShortRangeFunctions::forms().keyword(atomType->interactionPotential().form()),
    //                            atomType->interactionPotential().parametersAsString()))
    //         return false;

    // Pair potentials (if we are not using combination rules)
    if (!useCombinationRules_)
    {
        if (!parser.writeLineF("  # Pair Potentials\n"))
            return false;
        if (!parser.writeLineF("  {}  {}\n", PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::UseCombinationRules),
                               DissolveSys::btoa(false)))
            return false;
        for (const auto &[at1, at2, pot] : pairPotentials_)
            if (!parser.writeLineF("  {}  '{}'  '{}'  {}  {}\n",
                                   PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::PairPotentialKeyword),
                                   at1->name(), at2->name(), Functions1D::forms().keyword(pot->interactionPotential().form()),
                                   pot->interactionPotential().parametersAsString()))
                return false;
    }

    if (!parser.writeLineF("  {}  {}\n", PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::RangeKeyword),
                           PairPotential::range()))
        return false;
    if (!parser.writeLineF("  {}  {}\n", PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::DeltaKeyword),
                           PairPotential::delta()))
        return false;
    if (!parser.writeLineF("  {}  {}\n",
                           PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::ManualChargeSourceKeyword),
                           DissolveSys::btoa(PairPotential::chargeSource() != PairPotential::ChargeSource::Automatic)))
        return false;
    if (!parser.writeLineF("  {}  {}\n", PairPotentialsBlock::keywords().keyword(PairPotentialsBlock::IncludeCoulombKeyword),
                           DissolveSys::btoa(PairPotential::chargeSource() == PairPotential::ChargeSource::AtomTypes)))
        return false;
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

    restartFilename_ = std::format("{}.restart", inputFilename_);
}

// Return current input filename
std::string_view Dissolve::inputFilename() const { return inputFilename_; }

// Return restart filename
std::string_view Dissolve::restartFilename() const { return restartFilename_; }

// Return whether a restart filename has been set
bool Dissolve::hasRestartFilename() const { return (!restartFilename_.empty()); }
