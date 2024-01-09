// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/optionalRef.h"

// Forward Declarations
class Dissolve;
class ProcessPool;

class ModuleContext
{
    public:
    explicit ModuleContext(const ProcessPool &procPool);
    ModuleContext(const ProcessPool &procPool, Dissolve &dissolve);

    private:
    // Available process pool
    const ProcessPool &processPool_;
    // Reference to dissolve
    OptionalReferenceWrapper<Dissolve> dissolve_;

    public:
    // Return available process pool
    const ProcessPool &processPool() const;
    // Return reference to dissolve
    Dissolve &dissolve();
    const Dissolve &dissolve() const;
};