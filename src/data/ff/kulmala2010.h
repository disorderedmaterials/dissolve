// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "data/ff.h"

// Forward Declarations
/* none */

// Kulmala 2010 Forcefield
class Forcefield_Kulmala2010 : public Forcefield
{
    public:
    Forcefield_Kulmala2010() = default;
    ~Forcefield_Kulmala2010() = default;

    /*
     * Set Up
     */
    public:
    // Set up / create all forcefield terms
    bool setUp();

    /*
     * Definition
     */
    public:
    // Return name of Forcefield
    std::string_view name() const;
    // Return description for Forcefield
    std::string_view description() const;
    // Return short-range interaction style for AtomTypes
    Forcefield::ShortRangeType shortRangeType() const;
};
