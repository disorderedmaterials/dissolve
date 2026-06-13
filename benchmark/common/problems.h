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
enum class SpeciesType
{
    Atomic,
    SmallMolecule,
    MediumMolecule
};

enum class SpeciesPopulation
{
    Single,
    Small,
    Medium,
    Large
};

enum class SystemType
{
    ArgonCubic,
    ArgonMonoclinic,
    ArgonTriclinic,
    Water1000,
    Hexane200
};

template <SpeciesType speciesType, SpeciesPopulation population> class Problem
{
    public:
    Problem() : dissolve_(coreData_)
    {
        // Species Types
        static std::map<SpeciesType, std::string> speciesTypes = {{SpeciesType::Atomic, "Ar"},
                                                                  {SpeciesType::SmallMolecule, "species/water.toml"},
                                                                  {SpeciesType::MediumMolecule, "species/hexane.toml"}};

        // Population Sizes
        static std::map<SpeciesPopulation, int> populationSizes = {{SpeciesPopulation::Single, 1},
                                                                   {SpeciesPopulation::Small, 1000},
                                                                   {SpeciesPopulation::Medium, 5000},
                                                                   {SpeciesPopulation::Large, 10000}};
        Messenger::setQuiet(true);

        // Construct input filename and load it
        std::stringstream fileName;
        fileName << benchmarkInputFilePath << speciesTypes[speciesType] << ".txt";
        dissolve_.loadInput(fileName.str());

        // Grab the configuration
        auto *cfg = coreData_.configurations().front().get();

        // Set the population of the test species in a predefined Parameters node in the generator
        auto populationParameter = cfg->generator().rootSequence().parameterExists("Population");
        populationParameter->setValue(populationSizes[population]);

        // Generate the configuration and set the pair potential range accordingly
        cfg->generate({dissolve_});
        PairPotential::setRange(std::min(cfg->box().inscribedSphereRadius(), 15.0));

        // Prepare the simulation
        dissolve_.prepare();
    }

    private:
    CoreData coreData_;
    Dissolve dissolve_;

    public:
    CoreData &coreData() { return coreData_; }
    Dissolve &dissolve() { return dissolve_; }
    Configuration *configuration() { return coreData_.configurations().front().get(); }
};

template <SystemType systemType> class GraphProblem
{
    public:
    GraphProblem()
    {
        // System Types
        static std::map<SystemType, std::tuple<std::string, int, std::string>> systemTypes = {
            {SystemType::ArgonCubic, {"Ar|epsilon=0.774040 sigma=3.445996", 6755, "dlpoly/argon/cubic/big_argon.CONFIG"}},
            {SystemType::ArgonMonoclinic,
             {"Ar|epsilon=0.774040 sigma=3.445996", 6802, "dlpoly/argon/monoclinic/big_argon.CONFIG"}},
            {SystemType::ArgonTriclinic,
             {"Ar|epsilon=0.774040 sigma=3.445996", 6528, "dlpoly/argon/triclinic/big_argon.CONFIG"}},
            {SystemType::Water1000, {"species/water.toml", 1000, "dlpoly/water1000/full.CONFIG"}},
            {SystemType::Hexane200, {"species/hexane.toml", 200, "dlpoly/hexane200/full.REVCON"}}};

        auto &[speciesString, speciePopulation, referenceCoordinates] = systemTypes[systemType];

        Messenger::setQuiet(true);

        // Create the species node
        auto *speciesNode = dynamic_cast<SpeciesNode *>(graph_.createNode("Species"));
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
        auto *insertNode = dynamic_cast<InsertNode *>(graph_.createNode("Insert"));
        insertNode->setInput<Number>("Population", speciePopulation);
        insertNode->setInput<Number>("Density", 0.1);
        graph_.addEdge({"Species", "Species", "Insert", "Species"});

        // Create a configuration node
        auto *configurationNode = dynamic_cast<ConfigurationNode *>(graph_.createNode("Configuration"));
        configuration_ = &configurationNode->configuration();
        graph_.addEdge({"Configuration", "Configuration", "Insert", "Configuration"});

        // Add SetCoordinatesand ImportDLPOLYStructure
        auto *setCoordinatesnNode = dynamic_cast<SetCoordinatesNode *>(graph_.createNode("SetCoordinates"));
        auto *importStructureNode = dynamic_cast<ImportDLPOLYStructureNode *>(graph_.createNode("ImportDLPOLYStructure"));
        importStructureNode->setOption<std::string>("FilePath", referenceCoordinates);
        graph_.addEdge({"ImportDLPOLYStructure", "Structure", "SetCoordinates", "Structure"});
        graph_.addEdge({"Configuration", "Configuration", "SetCoordinates", "Configuration"});

        // Adjust pair potential properties
        PairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme::NoShortRangeTruncation);
        PairPotential::setRange(std::min(configuration_->box()->inscribedSphereRadius(), 15.0));
    }

    private:
    DissolveGraph graph_;
    Configuration *configuration_{nullptr};
    Species *species_{nullptr};

    public:
};
} // namespace Benchmarks
