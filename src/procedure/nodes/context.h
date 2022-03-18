// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include <string_view>

// Forward Declarations
class Configuration;
class GenericList;
class ProcessPool;

// Node Operating Data
class ProcedureContext
{
    public:
    ProcedureContext(ProcessPool &procPool, Configuration *cfg, std::string_view prefix, GenericList &targetList);

    private:
    // Available process pool
    ProcessPool &processPool_;
    // Target Configuration
    Configuration *configuration_{nullptr};
    // Prefix for generated data
    std::string_view dataPrefix_;
    // Target list for generated data
    GenericList &dataList_;

    public:
    // Return available process pool
    ProcessPool &processPool() const;
    // Return target Configuration
    Configuration *configuration() const;
    // Return prefix for generated data
    std::string_view dataPrefix() const;
    // Return target list for generated data
    GenericList &dataList() const;
};
