// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/ff/ff.h"

// NTf2 Forcefield by Ludwig Group
class Forcefield_Ludwig_NTf2 : public Forcefield
{
    public:
    Forcefield_Ludwig_NTf2() = default;
    ~Forcefield_Ludwig_NTf2() override = default;

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
