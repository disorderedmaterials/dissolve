// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/partialSet.h"
#include "module/module.h"

// Forward Declarations
class Dissolve;
class PartialSet;
class NeutronSQModule;

// TR Module
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
    bool saveTR_{false};
    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
