// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "templates/optionalref.h"
#include <string>

// Forward Declarations
class Configuration;
class GenericList;
class PotentialMap;
class ProcessPool;

// Node Operating Data
class ProcedureContext
{
    public:
    explicit ProcedureContext(const ProcessPool &procPool);
    ProcedureContext(const ProcessPool &procPool, Configuration *cfg);
    ProcedureContext(const ProcessPool &procPool, const PotentialMap &potentialMap);

    private:
    // Available process pool
    const ProcessPool &processPool_;
    // Target Configuration
    Configuration *configuration_{nullptr};
    // Prefix for generated data
    std::string dataPrefix_;
    // Target list for generated data
    OptionalReferenceWrapper<GenericList> dataList_;
    // Potential map
    OptionalReferenceWrapper<const PotentialMap> potentialMap_;

    public:
    // Return available process pool
    const ProcessPool &processPool() const;
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
    // Set potential map
    void setPotentialMap(const PotentialMap &potentialMap);
    // Return potential map
    const PotentialMap &potentialMap() const;
};
