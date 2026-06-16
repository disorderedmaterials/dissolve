// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "benchmarkConfig.h"
#include "classes/box.h"
#include "classes/coreData.h"
#include "expression/variable.h"
#include "main/dissolve.h"
#include "nodes/configuration.h"
#include "nodes/dissolve.h"
#include "nodes/importDLPOLYStructure.h"
#include "nodes/insert.h"
#include "nodes/setCoordinates.h"
#include "nodes/species.h"
#include <string>

namespace Benchmarks
{
enum class SystemType
{
    ArgonCubic,
    ArgonMonoclinic,
    ArgonTriclinic,
    Water1000,
    Hexane200
};

template <SystemType systemType> class GraphProblem : public DissolveGraph
{
    public:
    GraphProblem()
    {
        // System Types
        static std::map<SystemType, std::tuple<std::string, int, std::string>> systemTypes = {
            {SystemType::ArgonCubic,
             {"Ar|epsilon=0.774040 sigma=3.445996", 6755, "tests/data/dlpoly/argon/cubic/big_argon.CONFIG"}},
            {SystemType::ArgonMonoclinic,
             {"Ar|epsilon=0.774040 sigma=3.445996", 6802, "tests/data/dlpoly/argon/monoclinic/big_argon.CONFIG"}},
            {SystemType::ArgonTriclinic,
             {"Ar|epsilon=0.774040 sigma=3.445996", 6528, "tests/data/dlpoly/argon/triclinic/big_argon.CONFIG"}},
            {SystemType::Water1000, {"tests/data/species/water.toml", 1000, "tests/data/dlpoly/water1000/full.REVCON"}},
            {SystemType::Hexane200, {"tests/data/species/hexane.toml", 200, "tests/data/dlpoly/hexane200/full.REVCON"}}};

        auto &[speciesString, speciesPopulation, referenceCoordinates] = systemTypes[systemType];

        // Create the species node
        auto *speciesNode = dynamic_cast<SpeciesNode *>(createNode("Species"));
        species_ = &speciesNode->species();

        if (speciesString.ends_with(".toml"))
            species_->load(speciesString);
        else
        {
            if (speciesString.find('|') == std::string::npos)
                species_->createAtomic(Elements::element(speciesString), {ShortRangeFunctions::Form::Undefined});
            else
                species_->createAtomic(Elements::element(DissolveSys::beforeChar(speciesString, '|')),
                                       {ShortRangeFunctions::Form::LennardJones, DissolveSys::afterChar(speciesString, '|')});
        }

        // Add an Insert node for the Species
        auto *insertNode = dynamic_cast<InsertNode *>(createNode("Insert"));
        insertNode->setInput<Number>("Population", speciesPopulation);
        insertNode->setInput<Number>("Density", 0.1);
        addEdge({"Species", "Species", "Insert", "Species"});

        // Create a configuration node
        auto *configurationNode = dynamic_cast<ConfigurationNode *>(createNode("Configuration"));
        configuration_ = &configurationNode->configuration();
        addEdge({"Configuration", "Configuration", "Insert", "Configuration"});

        // Add SetCoordinates and ImportDLPOLYStructure
        auto *setCoordinatesNode = dynamic_cast<SetCoordinatesNode *>(createNode("SetCoordinates"));
        auto *importStructureNode = dynamic_cast<ImportDLPOLYStructureNode *>(createNode("ImportDLPOLYStructure"));
        importStructureNode->setOption<std::string>("FilePath", referenceCoordinates);
        addEdge({"ImportDLPOLYStructure", "Structure", "SetCoordinates", "Structure"});
        addEdge({"Insert", "Configuration", "SetCoordinates", "Configuration"});

        // Adjust pair potential properties
        PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::NoShortRangeTruncation);
        PairPotential::setRange(std::min(configuration_->box()->inscribedSphereRadius(), 15.0));

        // Set echo for all nodes
        Messenger::setQuiet(true);
        setEcho(false);

        // Run the graph from the SetCoordinates node
        if (setCoordinatesNode->run() != NodeConstants::ProcessResult::Success)
            throw(std::runtime_error("Failed to construct problem graph.\n"));
    }

    private:
    Configuration *configuration_{nullptr};
    Species *species_{nullptr};

    public:
    Configuration *configuration() { return configuration_; }
    Species *species() { return species_; };
};
} // namespace Benchmarks
