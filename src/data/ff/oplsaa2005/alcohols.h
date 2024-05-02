// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/ff/oplsaa2005/alkanes.h"

// OPLS-AA/2005 Alcohols Forcefield
class Forcefield_OPLSAA2005_Alcohols : public Forcefield_OPLSAA2005_Alkanes
{
    public:
    Forcefield_OPLSAA2005_Alcohols() = default;
    ~Forcefield_OPLSAA2005_Alcohols() override = default;

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
