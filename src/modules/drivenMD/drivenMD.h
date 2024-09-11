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

// Forward Declarations
class AtomType;
class PartialSet;

// SAXSDrivenMD Module
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
    // Frequency at which to apply generated perturbations to interatomic potentials
    std::optional<int> modifyPotential_{1};
    // Whether to apply this module's generated potentials to the global pair potentials
    bool applyPotentials_{true};
    // Target Modules containing data to refine against
    std::vector<Module *> targets_;
    // Weightings for targets (if not 1.0)
    std::vector<std::pair<Module *, double>> targetWeights_;
    // Ranges to calculate rFactor over
    std::vector<Range> ranges_;
    // Scattering matrix
    ScatteringMatrix scatteringMatrix_;

    public:
    // Return list of target Modules / data for refinement
    const std::vector<Module *> &targets() const;
    // Return current scattering matrix
    const ScatteringMatrix &scatteringMatrix() const;
    // Set whether to apply this module's generated potentials to the global pair potentials
    void setApplyPotentials(bool b);

    /*
     * Functions
     */
    private:
    // Target Configuration (determined from target modules)
    Configuration *targetConfiguration_{nullptr};

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;

    public:
    // Run set-up stage
    bool setUp(ModuleContext &moduleContext, Flags<KeywordBase::KeywordSignal> actionSignals) override;
};
