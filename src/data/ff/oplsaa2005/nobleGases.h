// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/ff/oplsaa2005/base.h"

// OPLS-AA (2005) Noble Gases Forcefield
class Forcefield_OPLSAA2005_NobleGases : public OPLSAA2005BaseForcefield
{
    public:
    Forcefield_OPLSAA2005_NobleGases() = default;
    ~Forcefield_OPLSAA2005_NobleGases() override = default;

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
