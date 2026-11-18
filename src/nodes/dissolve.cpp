// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "nodes/dissolve.h"
#include "classes/configuration.h"
#include "classes/potentialMap.h"
#include "kernels/energy.h"
#include "kernels/force.h"
#include "main/compatibility.h"
#include "main/version.h"

DissolveGraph::DissolveGraph() : Graph(nullptr) {}

/*
 * Definitions
 */

// Return node name
std::string_view DissolveGraph::name() const { return "Root"; }

// Return type of the node
std::string_view DissolveGraph::type() const { return "Dissolve"; }

// Return short summary of the node's purpose
std::string_view DissolveGraph::summary() const { return "Parent node of all simulations"; }

/*
 * Data
 */

// Return the DissolveGraph reference
DissolveGraph *DissolveGraph::dissolveGraph() { return this; }

/*
 * Files
 */

// Return current input file
std::filesystem::path DissolveGraph::inputFile() const { return inputFile_; }

// Load TOML input file
std::optional<std::string> DissolveGraph::loadTOML(std::filesystem::path filePath)
{
    auto contents = toml::parse(filePath);
    try
    {
        deserialise(contents);
        inputFile_ = filePath;
    }
    catch (toml::type_error err)
    {
        inputFile_.clear();
        return std::format("{} in {} at {} through {} ", err.what(), err.location().file_name(), err.location().first_line(),
                           err.location().last_line());
    }
    return {};
}

// Save TOML file
bool DissolveGraph::saveTOML(std::string_view filename) const
{
    std::ofstream outfile;
    outfile.open(std::string(filename));
    outfile << Serialisable::ser(*this) << std::endl;
    outfile.close();
    return true;
}

// Set frequency with which to write various iteration data
void DissolveGraph::setRestartDirectoryFrequency(int n) { restartDirectoryFrequency_ = n; }

// Return frequency with which to write restart file
int DissolveGraph::restartDirectoryFrequency() const { return restartDirectoryFrequency_; }

/*
 * Pair Potentials, Energy and Forces
 */

// Apply relevant overrides to the specified pair potential
void DissolveGraph::applyPairPotentialOverrides(PairPotential &pot)
{
    for (const auto &override : pairPotentialOverrides_)
    {
        // Is the override enabled?
        if (override->type() == PairPotentialOverride::PairPotentialOverrideType::Off)
            continue;

        // Is this override a match for the atom types in the potential?
        if ((DissolveSys::sameWildString(override->matchI(), pot.nameI()) &&
             DissolveSys::sameWildString(override->matchJ(), pot.nameJ())) ||
            (DissolveSys::sameWildString(override->matchJ(), pot.nameI()) &&
             DissolveSys::sameWildString(override->matchI(), pot.nameJ())))
        {
            // Create a function wrapper for the potential
            Function1DWrapper overridePotential(override->interactionPotential().form(),
                                                override->interactionPotential().parameters());
            // Apply the potential
            pot.addToReferenceShortRangePotential(
                overridePotential, override->type() == PairPotentialOverride::PairPotentialOverrideType::Replace);
        }
    }
}

// Update specified pair potential
void DissolveGraph::updatePairPotential(const AtomType &i, const AtomType &j)
{
    auto nameI = i.name(), nameJ = j.name();

    // Create the potential if it doesn't already exist
    if (!pairPotentials_.contains(nameI, nameJ))
    {
        std::optional<InteractionPotential<Functions1D>> interactionPotential;

        if (useCombinationRules_)
        {
            ShortRangeFunctions::combine(i.interactionPotential(), j.interactionPotential());

            if (interactionPotential.has_value())
                pairPotentials_.set(nameI, nameJ, {nameI, nameJ, *interactionPotential});
            else
                pairPotentials_.set(nameI, nameJ, {nameI, nameJ});
        }

        auto &pot = pairPotentials_.get({nameI, nameJ});
        pot.setLocalChargeProduct(i.charge() * j.charge());
    }

    // Update the tabulated potential
    auto &pot = pairPotentials_.get(nameI, nameJ);
    pot.tabulate();

    // Apply any relevant overrides
    applyPairPotentialOverrides(pot);

    // Add additional potential
    // TODO DISSOLVE2
    //     auto addPotName = std::format("Potential_{}-{}_Additional", at1->name(), at2->name());
    //     if (processingModuleData_.contains(addPotName, "Dissolve"))
    //         pp->setAdditionalPotential(processingModuleData_.retrieve<Data1D>(addPotName, "Dissolve"));
}

// Set whether pair potentials are updated automatically through combination rules
void DissolveGraph::setUseCombinationRules(bool b) { useCombinationRules_ = b; }

// Return whether pair potentials are updated automatically through combination rules
bool DissolveGraph::useCombinationRules() const { return useCombinationRules_; }

// Clear all pair potentials
void DissolveGraph::clearPairPotentials() { pairPotentials_.clear(); }

// Access a pair potential
PairPotential &DissolveGraph::pairPotential(const AtomType *at1, const AtomType *at2)
{
    return pairPotentials_.get(at1->name(), at2->name());
}

// Ensure that the specified Configuration has updated type indexing, cells etc.
void DissolveGraph::updateIndexingAndCells(Configuration *cfg) const
{
    // Update atom type indexing
    cfg->updateTypeIndexing();

    // Regenerate cells in the configuration if necessary
    cfg->updateCells();
}

// Create new pair potential override
PairPotentialOverride *DissolveGraph::addPairPotentialOverride(std::string_view matchI, std::string_view matchJ,
                                                               PairPotentialOverride::PairPotentialOverrideType overrideType,
                                                               const InteractionPotential<Functions1D> &potential)
{
    pairPotentialOverrides_.emplace_back(std::make_unique<PairPotentialOverride>(matchI, matchJ, overrideType, potential));
    return pairPotentialOverrides_.back().get();
}

// Return defined pair potential overrides
std::vector<std::unique_ptr<PairPotentialOverride>> &DissolveGraph::pairPotentialOverrides() { return pairPotentialOverrides_; }
const std::vector<std::unique_ptr<PairPotentialOverride>> &DissolveGraph::pairPotentialOverrides() const
{
    return pairPotentialOverrides_;
}

// Create an energy kernel suitable for the supplied Configuration
std::unique_ptr<EnergyKernel> DissolveGraph::createEnergyKernel(Configuration *cfg)
{
    // Update types and cells in Configuration
    updateIndexingAndCells(cfg);
    auto atomTypes = cfg->atomTypeVector();

    // Update pair potentials
    dissolve::for_each_pair(ParallelPolicies::seq, atomTypes,
                            [&](int i, const auto &atI, int j, const auto &atJ) { updatePairPotential(*atI, *atJ); });

    // Generate and return kernel
    return std::make_unique<EnergyKernel>(cfg, PotentialMap(atomTypes, pairPotentials_));
}

// Create a force kernel suitable for the supplied Configuration
std::unique_ptr<ForceKernel> DissolveGraph::createForceKernel(Configuration *cfg)
{
    // Update types and cells in Configuration
    updateIndexingAndCells(cfg);
    auto atomTypes = cfg->atomTypeVector();

    // Update pair potentials
    dissolve::for_each_pair(ParallelPolicies::seq, atomTypes,
                            [&](int i, const auto &atI, int j, const auto &atJ) { updatePairPotential(*atI, *atJ); });

    // Generate and return kernel
    return std::make_unique<ForceKernel>(cfg, PotentialMap(atomTypes, pairPotentials_));
}

/*
 * Serialisation
 */

// Serialise pair potential
SerialisedValue DissolveGraph::serialisePairPotentials() const
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
        // TODO DISSOLVE2 - Need a serialiser for DoubleKeyedMap
        // pairPotentials["useCombinationRules"] = false;
        // Serialisable::vector(pairPotentials_, "potentials", pairPotentials,
        //                      [](const auto &term)
        //                      {
        //                          const auto &[at1, at2, pot] = term;
        //                          SerialisedValue target;
        //                          pot->serialise("inner", target);
        //                          auto &value = target["inner"];
        //                          value["atomTypeI"] = at1->name();
        //                          value["atomTypeJ"] = at2->name();
        //                          return value;
        //                      });
    }
    return pairPotentials;
}

// Read pair potentials from a serialisable value
void DissolveGraph::deserialisePairPotentials(const SerialisedValue &node)
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

// Express as a serialisable value
void DissolveGraph::serialise(std::string tag, SerialisedValue &target) const
{
    auto &root = target[tag];

    root["version"] = Version::semantic();

    root["pairPotentials"] = serialisePairPotentials();

    Graph::serialise("graph", root);

    Serialisable::vector<>(pairPotentialOverrides_, "pairPotentialOverrides", root);
}

// Read values from a serialisable value
void DissolveGraph::deserialise(const SerialisedValue &node)
{
    // Default to current version if no version info is given.
    auto hasVersion = node.contains("version");
    if (!hasVersion)
        Messenger::warn("File does not contain version information.  Assuming the current version: {}", Version::semantic());
    const SerialisedValue versionNode = hasVersion ? dissolve::backwardsUpgrade(node) : node;

    Deserialisable::optionalOn(node, "graph", [this](const auto node) { Graph::deserialise(node); });

    Deserialisable::optionalOn(node, "pairPotentials", [this](const auto node) { deserialisePairPotentials(node); });

    Graph::deserialise(node.at("graph"));
    Deserialisable::vector(node, "pairPotentialOverrides",
                           [this](const auto ppOverrideNode) { addPairPotentialOverride()->deserialise(ppOverrideNode); });

    resolve();
}
