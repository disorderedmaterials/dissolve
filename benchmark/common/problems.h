// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
#pragma once

#include "benchmark_config.h"
#include "classes/box.h"
#include "classes/coredata.h"
#include "main/dissolve.h"
#include "modules/energy/energy.h"
#include "modules/rdf/rdf.h"

enum ProblemType
{
    Argon1k,
    Argon5k,
    Argon10k,
    Hexane1k,
};

template <ProblemType problem> std::string benchmarkFilePath()
{
    if constexpr (problem == ProblemType::Argon1k)
    {
        std::string filename = "argon1k.txt";
        std::string fullPath = benchmark_path + filename;
        return fullPath;
    }
    else if constexpr (problem == ProblemType::Argon5k)
    {
        std::string filename = "argon5k.txt";
        std::string fullPath = benchmark_path + filename;
        return fullPath;
    }
    else if constexpr (problem == ProblemType::Argon10k)
    {
        std::string filename = "argon10k.txt";
        std::string fullPath = benchmark_path + filename;
        return fullPath;
    }
    else if constexpr (problem == ProblemType::Hexane1k)
    {
        std::string filename = "hexane1k.txt";
        std::string fullPath = benchmark_path + filename;
        return fullPath;
    }
    else
        return {};
}

namespace Method
{
constexpr auto CellsMethod = RDFModule::PartialsMethod::CellsMethod;
constexpr auto SimpleMethod = RDFModule::PartialsMethod::SimpleMethod;
} // namespace Method

template <ProblemType problem> struct Problem
{
    Problem() : dissolve_(coredata_)
    {
        Messenger::setQuiet(true);
        dissolve_.registerMasterModules();
        auto file = benchmarkFilePath<problem>();
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

    void setUpEnergy()
    {
        energymodule_ = std::make_unique<RDFModule>();
        energymodule_->addTargetConfiguration(cfg_);
    }

    template <RDFModule::PartialsMethod method> void iterateGR()
    {
        double rdfRange = cfg_->box()->inscribedSphereRadius();
        bool upToDate = false;
        rdfmodule_->calculateGR(dissolve_.processingModuleData(), dissolve_.worldPool(), cfg_, method, rdfRange, 0.05,
                                upToDate);
        dissolve_.processingModuleData().clearAll();
    }

    void iterateEnergy()
    {
        int a = 3;
        int b = 4;
    }

    CoreData coredata_;
    Dissolve dissolve_;
    std::unique_ptr<RDFModule> rdfmodule_;
    std::unique_ptr<EnergyModule> energymodule_;

    Configuration *cfg_;
};
