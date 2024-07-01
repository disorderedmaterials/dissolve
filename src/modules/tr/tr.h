// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/partialSet.h"
#include "math/averaging.h"
#include "math/function1D.h"
#include "module/module.h"

// Forward Declarations
class Dissolve;
class ModuleGroup;
class PartialSet;
class NeutronSQModule;

// RDF Module
class TRModule : public Module
{
    public:
    TRModule();
    ~TRModule() override = default;

    /*
     * Definition
     */
    public:
    private:
    // Source module for calculation
    const NeutronSQModule *sourceNeutronSQ_{nullptr};
    PartialSet unweightedtr;
    PartialSet weightedtr;

    /*
     * Functions
     */
    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
