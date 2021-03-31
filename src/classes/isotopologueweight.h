// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "templates/listitem.h"
#include "templates/refdatalist.h"

// Forward Declarations
class Species;
class Isotopologue;
class LineParser;

// Isotopologue Weight
class IsotopologueWeight
{
    public:
    IsotopologueWeight(const Isotopologue *iso = nullptr, double weight = 1.0);
    ~IsotopologueWeight();

    /*
     * Isotopologue and Weight
     */
    private:
    // Associated Isotopologue
    const Isotopologue *isotopologue_;
    // Relative weight
    double weight_;

    public:
    // Set associated Isotopologue and weight
    void set(const Isotopologue *iso, double weight);
    // Set associated Isotopologue
    void setIsotopologue(const Isotopologue *iso);
    // Return associated Isotopologue
    const Isotopologue *isotopologue() const;
    // Set relative weight
    void setWeight(double weight);
    // Return relative weight
    double weight() const;
};
