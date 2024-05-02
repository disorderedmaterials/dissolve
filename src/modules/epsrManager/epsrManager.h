// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/scatteringMatrix.h"
#include "module/groups.h"
#include "module/module.h"
#include "procedure/procedure.h"
#include <tuple>

// EPSR Manager Module
class EPSRManagerModule : public Module
{
    public:
    EPSRManagerModule();
    ~EPSRManagerModule() override = default;

    /*
     * Definition
     */

    private:
    // Target Module containing data to refine against
    std::vector<Module *> target_;
    // Frequency at which to apply generated perturbations to interatomic potentials
    std::optional<int> modifyPotential_{1};
    // Vector storing atom pairs and associated potentials
    std::vector<std::tuple<std::shared_ptr<AtomType>, std::shared_ptr<AtomType>, Data1D>> potentials_;
    struct EPData
    {
        Data1D ep;
        double count{0};
        std::shared_ptr<AtomType> at1, at2;
    };
    // Potential scalings
    std::string potentialScalings_;

    /*
     * Functions
     */
    private:
    // Return key for supplied atom type pair
    static std::string pairKey(const std::shared_ptr<AtomType> &at1, const std::shared_ptr<AtomType> &at2);

    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;

    public:
    // Run set-up stage
    bool setUp(ModuleContext &moduleContext, Flags<KeywordBase::KeywordSignal> actionSignals) override;
};
