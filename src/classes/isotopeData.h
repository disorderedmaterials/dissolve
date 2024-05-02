// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/isotopes.h"

// IsotopeData Definition
class IsotopeData
{
    public:
    IsotopeData(Sears91::Isotope isotope = Sears91::Isotope::Unknown, double population = 0.0, double fraction = 0.0);
    IsotopeData(const IsotopeData &source) = default;
    IsotopeData &operator=(const IsotopeData &source) = default;

    /*
     * Properties
     */
    private:
    // Reference Isotope
    Sears91::Isotope isotope_;
    // Population of Isotope
    double population_;
    // Local fractional population (e.g. within an AtomTypeData)
    double fraction_;

    public:
    // Initialise
    void initialise(Sears91::Isotope isotope);
    // Add to population of Isotope
    void add(double nAdd);
    // Finalise, calculating local fractional population (e.g. within an AtomTypeData)
    void finalise(double totalAtoms);
    // Zero population and fraction
    void zeroPopulation();
    // Return referenced Isotope
    Sears91::Isotope isotope() const;
    // Return total population
    double population() const;
    // Return local fractional population (e.g. within an AtomTypeData)
    double fraction() const;
};
