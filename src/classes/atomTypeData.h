// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "data/isotopes.h"
#include <map>

// AtomTypeData Definition
class AtomTypeData
{
    public:
    AtomTypeData() = default;

    /*
     * Properties
     */
    private:
    // Isotope population
    std::map<Sears91::Isotope, double> isotopes_;
    // Calculated bound coherent scattering over all Isotopes
    double boundCoherent_{0.0};

    public:
    // Add to population
    void add(double nAdd);
    // Add to population of Isotope
    void add(Sears91::Isotope isotope, double population);
    // Finalise, calculating fractional populations etc.
    void finalise();
    // Return isotopes
    const std::map<Sears91::Isotope, double> &isotopes() const;
    // Set calculated bond coherent scattering over all isotopes
    void setBoundCoherent(double d);
    // Return calculated bound coherent scattering over all Isotopes
    double boundCoherent() const;
};
