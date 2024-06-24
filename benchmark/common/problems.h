// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "benchmarkConfig.h"
#include "classes/box.h"
#include "classes/coreData.h"
#include "main/dissolve.h"
#include <string>

namespace DissolveBenchmarks
{
const char *FILEEXTENSION = ".txt";
enum class SpeciesType
{
    Atomic,
    SmallMolecule,
    MediumMolecule,
    FrameworkMolecule,
};

enum class Population
{
    Single,
    Small,
    Medium,
    Large
};

template <SpeciesType speciesType, Population moleculePopulation> std::string benchmarkFilePath()
{
    static std::map<SpeciesType, std::string> speciesTypes = {{SpeciesType::Atomic, "argon"},
                                                              {SpeciesType::SmallMolecule, "water"},
                                                              {SpeciesType::MediumMolecule, "hexane"},
                                                              {SpeciesType::FrameworkMolecule, "mof5-3x3x3"}};
    static std::map<Population, std::string> populationSizes = {
        {Population::Single, "single"}, {Population::Small, "1k"}, {Population::Medium, "5k"}, {Population::Large, "10k"}};

    std::stringstream fileName;
    fileName << benchmarkInputFilePath << speciesTypes[speciesType] << populationSizes[moleculePopulation] << FILEEXTENSION;
    return fileName.str();
}

template <SpeciesType speciesType, Population population> class Problem
{
    public:
    Problem() : dissolve_(coreData_)
    {
        Messenger::setQuiet(true);
        dissolve_.loadInput(benchmarkFilePath<speciesType, population>());
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

} // namespace DissolveBenchmarks
