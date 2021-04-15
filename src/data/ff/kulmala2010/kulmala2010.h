// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/ff/ff.h"

// Forward Declarations
/* none */

// Kulmala 2010 Forcefield
class Forcefield_Kulmala2010 : public Forcefield
{
    public:
    Forcefield_Kulmala2010() = default;
    ~Forcefield_Kulmala2010() override = default;

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
    Forcefield::ShortRangeType shortRangeType() const override;
};
