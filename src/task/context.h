// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "templates/optionalRef.h"


// Forward Declarations
class Configuration;
class Dissolve;
class ProcessPool;
class TaskContext {

    public:
    template <class... ContextObjects> TaskContext(const TaskContext &other, ContextObjects &&...contextObjects)
    {
        *this = other;
        ([&] { set(contextObjects); }(), ...);
    }
    template <class... ContextObjects> TaskContext(ContextObjects &&...contextObjects)
    {
        ([&] { set(contextObjects); }(), ...);
    }

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
            fmt::format("Invalid object type ({}) passed to TaskContext set().\n", typeid(obj).name())));
    }
};