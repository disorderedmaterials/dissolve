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

    // PP override
    std::vector<PairPotentialOverride> pairPotentialOverrides() override
    {
        InteractionPotential<Functions1D> oOPotential(Functions1D::Form::Buckingham128,
                                                      {133996.2240, 2.76, 16884.9331, 17367.3598, 2315.6480});
        InteractionPotential<Functions1D> siOPotential(Functions1D::Form::Buckingham128,
                                                       {1737098.491, 4.8732, 12884.4679, 1929.7066, 578.912});

        std::vector<PairPotentialOverride> overrides{
            {"O", "O", PairPotentialOverride::PairPotentialOverrideType::Add, oOPotential},
            {"Si", "O", PairPotentialOverride::PairPotentialOverrideType::Add, siOPotential}};

        return overrides;
    }

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
};
