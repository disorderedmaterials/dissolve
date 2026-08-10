// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "base/messenger.h"
#include "base/serialiser.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/species.h"
#include "main/compatibility.h"
#include "main/dissolve.h"
#include "main/version.h"
#include "nodes/dissolve.h"
#include <fstream>

// Serialise pair potential
SerialisedValue Dissolve::serialisePairPotentials() const
{
    SerialisedValue pairPotentials = Serialisable::table{
        {"range", PairPotential::range()},
        {"delta", PairPotential::delta()},
        {"chargeSource", PairPotential::chargeSources().serialise(PairPotential::chargeSource())},
        {"coulombTruncation", PairPotential::coulombTruncationSchemes().serialise(PairPotential::coulombTruncationScheme())},
        {"shortRangeTruncation",
         PairPotential::shortRangeTruncationSchemes().serialise(PairPotential::shortRangeTruncationScheme())}};
    if (!useCombinationRules_)
    {
        pairPotentials["useCombinationRules"] = false;
        Serialisable::vector(pairPotentials_, "potentials", pairPotentials,
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

    root["pairPotentials"] = serialisePairPotentials();

    graphNode_->serialise("graph", root);
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

    Deserialisable::optionalOn(node, "graph", [this](const auto node) { graphNode_->deserialise(node); });

    Deserialisable::optionalOn(node, "pairPotentials", [this](const auto node) { deserialisePairPotentials(node); });
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
        Messenger::error("Not a valid TOML file?\n\n{}", e.what());
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
    outfile << Serialisable::ser(*this).as_string() << std::endl;
    outfile.close();
    return true;
}

// Return whether an input filename has been set
bool Dissolve::hasInputFilename() const { return (!inputFilename_.empty()); }

// Set current input filename
void Dissolve::setInputFilename(std::string_view filename)
{
    inputFilename_ = filename;

    restartFilename_ = std::format("{}.restart", inputFilename_);
}

// Return current input filename
std::string_view Dissolve::inputFilename() const { return inputFilename_; }

// Return restart filename
std::string_view Dissolve::restartFilename() const { return restartFilename_; }

// Return whether a restart filename has been set
bool Dissolve::hasRestartFilename() const { return (!restartFilename_.empty()); }
