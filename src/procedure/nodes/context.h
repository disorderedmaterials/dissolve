// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "templates/optionalRef.h"
#include <stdexcept>
#include <string>

// Forward Declarations
class Configuration;
class Dissolve;
class GenericList;
class PotentialMap;
class ProcessPool;

// Node Operating Data
class ProcedureContext
{
    public:
    explicit ProcedureContext(const ProcessPool &procPool);
    ProcedureContext(const ProcessPool &procPool, Configuration *cfg);
    ProcedureContext(const ProcessPool &procPool, Dissolve &dissolve);
    template <class... ContextObjects>
    ProcedureContext(Dissolve &dissolve, const ProcessPool &procPool, ContextObjects &&...contextObjects)
        : dissolve_(dissolve), processPool_(procPool)
    {
        (
            [&]
            {
                set(contextObjects);
            }(),
            ...);
    }

    private:
    // Available process pool
    const ProcessPool &processPool_;
    // Target Configuration
    Configuration *configuration_{nullptr};
    // Prefix for generated data
    std::string processingDataPrefix_;
    // Dissolve
    OptionalReferenceWrapper<Dissolve> dissolve_;

    private:
    template <class T> void set(T) { throw(std::runtime_error("Invalid object type passed to ProcedureContext set().\n")); }
    template <class T> void set(Configuration *cfg) { printf("It's a config!\n"); };

    public:
    // Return available process pool
    const ProcessPool &processPool() const;
    // Set target Configuration
    void setConfiguration(Configuration *cfg);
    // Return target Configuration
    Configuration *configuration() const;
    // Set prefix for generated processing data
    void setProcessingDataPrefix(std::string_view prefix);
    // Return prefix for generated data
    std::string_view processingDataPrefix() const;
    // Return target list for generated data
    GenericList &dataList() const;
    // Set reference to Dissolve
    void setDissolve(Dissolve &dissolve);
    // Return reference to Dissolve
    Dissolve &dissolve() const;
    // Return potential map
    const PotentialMap &potentialMap() const;
};
