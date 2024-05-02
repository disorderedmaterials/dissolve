// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/optionalRef.h"
#include <fmt/format.h>
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
    template <class... ContextObjects> ProcedureContext(const ProcedureContext &other, ContextObjects &&...contextObjects)
    {
        *this = other;
        ([&] { set(contextObjects); }(), ...);
    }
    template <class... ContextObjects> ProcedureContext(ContextObjects &&...contextObjects)
    {
        ([&] { set(contextObjects); }(), ...);
    }

    private:
    // Reference to Dissolve
    OptionalReferenceWrapper<Dissolve> dissolve_;
    // Available process pool
    OptionalReferenceWrapper<const ProcessPool> processPool_;
    // Target Configuration
    Configuration *configuration_{nullptr};
    // Prefix for generated data
    std::string processingDataPrefix_;

    private:
    // Set reference to Dissolve
    void set(Dissolve &dissolve);
    // Set available ProcessPool
    void set(const ProcessPool &procPool);
    // Set prefix for generated processing data
    void set(std::string_view prefix);
    // Set target Configuration
    void set(Configuration *cfg);
    // Catch any unrecognised object type
    template <class T> void set(T obj)
    {
        throw(std::runtime_error(
            fmt::format("Invalid object type ({}) passed to ProcedureContext set().\n", typeid(obj).name())));
    }

    public:
    // Return reference to Dissolve
    Dissolve &dissolve() const;
    // Return available process pool
    const ProcessPool &processPool() const;
    // Return target Configuration
    Configuration *configuration() const;
    // Return prefix to use for processing data
    std::string_view processingModuleDataPrefix() const;
    // Return processing data list
    GenericList &processingModuleData() const;
    // Return potential map
    const PotentialMap &potentialMap() const;
};
