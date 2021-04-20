// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors
#pragma once

#include "benchmark_config.h"
#include "classes/box.h"
#include "classes/coredata.h"
#include "main/dissolve.h"
#include "modules/rdf/rdf.h"

enum ProblemSize
{
    Small,
    Medium,
    Large
};

template <ProblemSize problem> std::string benchmarkFilePath()
{
    if constexpr (problem == ProblemSize::Small)
    {
        std::string filename = "small_benchmark.txt";
        std::string fullPath = benchmark_path + filename;
        return fullPath;
    }
    else if constexpr (problem == ProblemSize::Medium)
    {
        std::string filename = "medium_benchmark.txt";
        std::string fullPath = benchmark_path + filename;
        return fullPath;
    }
    else if constexpr (problem == ProblemSize::Large)
    {
        std::string filename = "large_benchmark.txt";
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

template <ProblemSize problem> struct Problem
{
    Problem() : dissolve_(coredata_)
    {
        Messenger::setQuiet(true);
        dissolve_.registerMasterModules();
        auto file = benchmarkFilePath<problem>();
        dissolve_.loadInput(file);
        dissolve_.prepare();
        setUp();
    }

    void setUp()
    {
        cfg_ = dissolve_.configurations().first();
        auto &procPool = dissolve_.worldPool();
        procPool.assignProcessesToGroups(cfg_->processPool());
        // create the rdf module
        rdfmodule_ = std::make_unique<RDFModule>();
        rdfmodule_->addTargetConfiguration(cfg_);
        cfg_->incrementContentsVersion();
    }

    template <RDFModule::PartialsMethod method> void iterateCells()
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
    Configuration *cfg_;
};