// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/potentialSet.h"
#include "classes/scatteringMatrix.h"
#include "generator/generator.h"
#include "math/averaging.h"
#include "math/history.h"
#include "module/module.h"

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
    // History of potentials
    History<PotentialSet> potentials_;
    // Potential scalings
    std::string potentialScalings_;
    // Number of historical potentials sets to combine into final potentials
    std::optional<int> averagingLength_{};
    // Weighting scheme to use when averaging potentials
    Averaging::AveragingScheme averagingScheme_{Averaging::LinearAveraging};

    /*
     * Functions
     */
    private:
    // Return key for supplied atom type pair
    static std::string pairKey(const AtomType *at1, const AtomType *at2);

    private:
    // Run main processing
    Module::ExecutionResult process(Dissolve &dissolve) override;

    public:
    // Run set-up stage
    bool setUp(Dissolve &dissolve, Flags<KeywordBase::KeywordSignal> actionSignals) override;
};
