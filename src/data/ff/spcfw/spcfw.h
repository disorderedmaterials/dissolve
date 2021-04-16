// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/ff/ff.h"

// Forward Declarations
/* none */

// SPC/Fw Forcefield
class Forcefield_SPCFw : public Forcefield
{
    public:
    Forcefield_SPCFw() = default;
    ~Forcefield_SPCFw() override = default;

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
    Forcefield::ShortRangeType shortRangeType() const override;
};
