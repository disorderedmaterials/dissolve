// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "benchmarkConfig.h"
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
    fileName << benchmarkInputFilePath << systemName;
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

template <ProblemType problem, Population population> class Problem
{
    public:
    Problem() : dissolve_(coreData_)
    {
        Messenger::setQuiet(true);
        auto file = benchmarkFilePath<problem, population>();
        dissolve_.loadInput(file);
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
