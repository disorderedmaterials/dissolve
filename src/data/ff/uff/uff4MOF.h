// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/ff/uff/uff.h"

// Universal Forcefield for MOFs
class Forcefield_UFF4MOF : public Forcefield_UFF
{
    public:
    Forcefield_UFF4MOF() = default;
    ~Forcefield_UFF4MOF() override = default;

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
};
