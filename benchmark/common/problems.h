// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "benchmarkConfig.h"
#include "classes/box.h"
#include "classes/coreData.h"
#include "expression/variable.h"
#include "main/dissolve.h"
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

template <SpeciesType speciesType, SpeciesPopulation population> class Problem
{
    public:
    Problem() : dissolve_(coreData_)
    {
        // Species Types
        static std::map<SpeciesType, std::string> speciesTypes = {{SpeciesType::Atomic, "argon"},
                                                                  {SpeciesType::SmallMolecule, "water"},
                                                                  {SpeciesType::MediumMolecule, "hexane"}};

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
        dissolve_.setPairPotentialRange(std::min(cfg->box()->inscribedSphereRadius(), 15.0));

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
} // namespace Benchmarks
