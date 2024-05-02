// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/ff/ff.h"

// Zhang 2013 Forcefield
class Forcefield_Zhang2013 : public Forcefield
{
    public:
    Forcefield_Zhang2013() = default;
    ~Forcefield_Zhang2013() override = default;

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
};
