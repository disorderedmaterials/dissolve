// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "data/isotopes.h"
#include <map>

// AtomTypeData Definition
class AtomTypeData
{
    public:
    AtomTypeData(double population = 0, double fraction = 0, double boundCoherent = 0);

    /*
     * Properties
     */
    private:
    // Whether the AtomType has been marked as exchangeable
    bool exchangeable_{false};
    // Isotope population
    std::map<Sears91::Isotope, double> isotopes_;
    // Total population
    double population_{0.0};
    // World fractional population over all Isotopes
    double fraction_{0.0};
    // Calculated bound coherent scattering over all Isotopes
    double boundCoherent_{0.0};

    public:
    // Add to population
    void add(double nAdd);
    // Add to population of Isotope
    void add(Sears91::Isotope isotope, double population);
    // Set exchangeable flag
    void setAsExchangeable();
    // Return whether the associated AtomType is exchangeable
    bool exchangeable() const;
    // Finalise, calculating fractional populations etc.
    void finalise(double nWorldAtoms);
    // Return isotopes
    const std::map<Sears91::Isotope, double> &isotopes() const;
    // Return total population over all isotopes
    int population() const;
    // Return total fractional population including all isotopes
    double fraction() const;
    // Set calculated bond coherent scattering over all isotopes
    void setBoundCoherent(double d);
    // Return calculated bound coherent scattering over all Isotopes
    double boundCoherent() const;
    // Return referenced AtomType name
    std::string_view atomTypeName() const;
};
