// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "templates/optionalref.h"
#include <string>

// Forward Declarations
class Configuration;
class GenericList;
class ProcessPool;

// Node Operating Data
class ProcedureContext
{
    public:
    ProcedureContext(ProcessPool &procPool, Configuration *cfg = nullptr);

    private:
    // Available process pool
    ProcessPool &processPool_;
    // Target Configuration
    Configuration *configuration_{nullptr};
    // Prefix for generated data
    std::string dataPrefix_;
    // Target list for generated data
    OptionalReferenceWrapper<GenericList> dataList_;

    public:
    // Return available process pool
    ProcessPool &processPool() const;
    // Set target Configuration
    void setConfiguration(Configuration *cfg);
    // Return target Configuration
    Configuration *configuration() const;
    // Set target data list and prefix
    void setDataListAndPrefix(GenericList &list, std::string_view prefix);
    // Return prefix for generated data
    std::string_view dataPrefix() const;
    // Return target list for generated data
    GenericList &dataList() const;
};
