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

// Generator Operating Context
class GeneratorContext
{
    public:
    template <class... ContextObjects> GeneratorContext(const GeneratorContext &other, ContextObjects &&...contextObjects)
    {
        *this = other;
        ([&] { set(contextObjects); }(), ...);
    }
    template <class... ContextObjects> GeneratorContext(ContextObjects &&...contextObjects)
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

    private:
    // Set reference to Dissolve
    void set(Dissolve &dissolve);
    // Set available ProcessPool
    void set(const ProcessPool &procPool);
    // Set target Configuration
    void set(Configuration *cfg);
    // Catch any unrecognised object type
    template <class T> void set(T obj)
    {
        throw(std::runtime_error(
            fmt::format("Invalid object type ({}) passed to GeneratorContext set().\n", typeid(obj).name())));
    }

    public:
    // Return available process pool
    const ProcessPool &processPool() const;
    // Return target Configuration
    Configuration *configuration() const;
    // Return potential map
    const PotentialMap &potentialMap() const;
};
