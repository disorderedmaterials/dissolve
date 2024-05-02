// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/ff/ff.h"

// Zhao 2010 Cu-BTC Forcefield
class Forcefield_Zhao2010 : public Forcefield
{
    public:
    Forcefield_Zhao2010() = default;
    ~Forcefield_Zhao2010() override = default;

    /*
     * Set Up
     */
    public:
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

    /*
     * Term Assignment
     */
    public:
    // Assign / generate angle term parameters
    bool assignAngleTermParameters(const Species *parent, SpeciesAngle &angle, bool determineTypes) const override;
};
