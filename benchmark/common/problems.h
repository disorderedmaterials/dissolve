// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "benchmark_config.h"
#include "classes/box.h"
#include "classes/coreData.h"
#include "main/dissolve.h"
#include "modules/energy/energy.h"
#include "modules/gr/gr.h"
#include <string>

namespace
{
const char *FILEEXTENSION = ".txt";
}
enum ProblemType
{
    atomic,
    smallMolecule,
    mediumMolecule,
    frameworkMolecule,
};

enum Population
{
    single,
    small,
    medium,
    large
};

template <Population population> std::string getFileName(const std::string &systemName)
{
    std::stringstream fileName;
    fileName << benchmark_path << systemName;
    if constexpr (population == Population::single)
    {
        fileName << "single";
    }
    else if constexpr (population == Population::small)
    {
        fileName << "1k";
    }
    else if constexpr (population == Population::medium)
    {
        fileName << "5k";
    }
    else if constexpr (population == Population::large)
    {
        fileName << "10k";
    }
    fileName << FILEEXTENSION;
    return fileName.str();
}

template <ProblemType problem, Population population> std::string benchmarkFilePath()
{
    if constexpr (problem == ProblemType::atomic)
    {
        return getFileName<population>("argon");
    }
    else if constexpr (problem == ProblemType::smallMolecule)
    {
        return getFileName<population>("water");
    }
    else if constexpr (problem == ProblemType::mediumMolecule)
    {
        return getFileName<population>("hexane");
    }
    else if constexpr (problem == ProblemType::frameworkMolecule)
    {
        return getFileName<population>("mof5-3x3x3");
    }
    else
        return {};
}

namespace Method
{
constexpr auto CellsMethod = GRModule::PartialsMethod::CellsMethod;
constexpr auto SimpleMethod = GRModule::PartialsMethod::SimpleMethod;
} // namespace Method

template <ProblemType problem, Population population> struct Problem
{
    Problem() : dissolve_(coreData_)
    {
        Messenger::setQuiet(true);
        auto file = benchmarkFilePath<problem, population>();
        dissolve_.loadInput(file);
        dissolve_.prepare();
        cfg_ = coreData_.configurations().front().get();
        setUpRDF();
    }

    void setUpRDF()
    {
        rdfmodule_ = std::make_unique<GRModule>();
        rdfmodule_->keywords().set("Configurations", std::vector<Configuration *>{cfg_});
    }

    template <GRModule::PartialsMethod method> void iterateGR()
    {
        double rdfRange = cfg_->box()->inscribedSphereRadius();
        bool upToDate = false;
        rdfmodule_->calculateGR(dissolve_.processingModuleData(), dissolve_.worldPool(), cfg_, method, rdfRange, 0.05,
                                upToDate);
        dissolve_.processingModuleData().clearAll();
    }

    CoreData coreData_;
    Dissolve dissolve_;
    std::unique_ptr<GRModule> rdfmodule_;
    std::unique_ptr<EnergyModule> energymodule_;

    Configuration *cfg_;
};
