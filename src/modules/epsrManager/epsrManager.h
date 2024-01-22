// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/scatteringMatrix.h"
#include "module/groups.h"
#include "module/module.h"
#include "procedure/procedure.h"

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
    // Scattering matrix
    ScatteringMatrix scatteringMatrix_;
    // Vector storing atom pairs and associated potentials
    std::vector<std::pair<std::pair<std::shared_ptr<AtomType>, std::shared_ptr<AtomType>>, Data1D>> potentials_;
    struct EPData
    {
        Data1D ep;
        double count{0};
        std::shared_ptr<AtomType> at1, at2;
    };

    /*
     * Functions
     */
    public:
    std::string pairKey(std::pair<std::shared_ptr<AtomType>, std::shared_ptr<AtomType>> atomTypes);

    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext);
};
