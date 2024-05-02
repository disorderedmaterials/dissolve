// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/ff/pcl2019/base.h"

// Padua & Canongia Lopes Ionic Liquids (2019) Cations Forcefield
class Forcefield_PCL2019_Cations : public PCL2019BaseForcefield
{
    public:
    Forcefield_PCL2019_Cations() = default;
    ~Forcefield_PCL2019_Cations() override = default;

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
