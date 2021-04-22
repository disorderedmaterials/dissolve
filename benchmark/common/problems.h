// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
#pragma once

#include "benchmark_config.h"
#include "classes/box.h"
#include "classes/coredata.h"
#include "main/dissolve.h"
#include "modules/energy/energy.h"
#include "modules/rdf/rdf.h"
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
};

enum Population
{
    small,
    medium,
    large,
};

template <Population population> std::string getFileName(std::string systemName)
{
    std::stringstream fileName;
    fileName << benchmark_path << systemName;
    if constexpr (population == Population::small)
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
    else
        return {};
}

namespace Method
{
constexpr auto CellsMethod = RDFModule::PartialsMethod::CellsMethod;
constexpr auto SimpleMethod = RDFModule::PartialsMethod::SimpleMethod;
} // namespace Method

template <ProblemType problem, Population population> struct Problem
{
    Problem() : dissolve_(coredata_)
    {
        Messenger::setQuiet(true);
        dissolve_.registerMasterModules();
        auto file = benchmarkFilePath<problem, population>();
        dissolve_.loadInput(file);
        dissolve_.prepare();
        cfg_ = dissolve_.configurations().first();
        auto &procPool = dissolve_.worldPool();
        procPool.assignProcessesToGroups(cfg_->processPool());
        setUpRDF();
    }

    void setUpRDF()
    {
        rdfmodule_ = std::make_unique<RDFModule>();
        rdfmodule_->addTargetConfiguration(cfg_);
    }

    template <RDFModule::PartialsMethod method> void iterateGR()
    {
        double rdfRange = cfg_->box()->inscribedSphereRadius();
        bool upToDate = false;
        rdfmodule_->calculateGR(dissolve_.processingModuleData(), dissolve_.worldPool(), cfg_, method, rdfRange, 0.05,
                                upToDate);
        dissolve_.processingModuleData().clearAll();
    }

    CoreData coredata_;
    Dissolve dissolve_;
    std::unique_ptr<RDFModule> rdfmodule_;
    std::unique_ptr<EnergyModule> energymodule_;

    Configuration *cfg_;
};
