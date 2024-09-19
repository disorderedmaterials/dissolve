// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "classes/scatteringMatrix.h"
#include "math/data1D.h"
#include "math/range.h"
#include "module/groups.h"
#include "module/module.h"
#include "templates/array3D.h"
#include <tuple>

// DrivenMD Module
class DrivenMDModule : public Module
{
    public:
    DrivenMDModule();
    ~DrivenMDModule() override = default;

    /*
     * Definition
     */
    public:
    private:
    // Vector storing atom pairs and associated potentials
    std::vector<std::tuple<std::shared_ptr<AtomType>, std::shared_ptr<AtomType>, Data1D>> empiricalPotentials_;
    // Target Modules containing data to refine against
    std::vector<Module *> targets_;

    public:
    // Return list of target Modules / data for refinement
    const std::vector<Module *> &targets() const;
    // Set whether to apply this module's generated potentials to the global pair potentials
    void setApplyPotentials(bool b);

    /*
     * Functions
     */
    private:
    // Target Configuration (determined from target modules)
    Configuration *targetConfiguration_{nullptr};
    // Calculate partial g(r) in serial with simple double-loop
    PartialSet calculateGRTestSerial(Configuration *cfg, PartialSet &partialSet);
    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
