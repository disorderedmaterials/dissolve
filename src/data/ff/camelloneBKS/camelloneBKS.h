// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "data/ff/ff.h"

// CamelloneBKS Forcefield
class Forcefield_CamelloneBKS : public Forcefield
{
    public:
    Forcefield_CamelloneBKS() = default;
    ~Forcefield_CamelloneBKS() override = default;

    /*
     * Set Up
     */
    protected:
    // Set up / create all forcefield terms
    bool setUp() override;

    /*
     * Definition
     */
    public:
    // Return name of Forcefield
    std::string_view name() const override;
    // Return description for Forcefield
    std::string_view description() const override;
    // Return short-range interaction style for AtomTypes
    ShortRangeFunctions::Form shortRangeForm() const override;
    // Return defined overrides
    std::vector<PairPotentialOverride> pairPotentialOverrides() override;
};
