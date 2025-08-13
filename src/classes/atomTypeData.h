// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/isotopeData.h"
#include <memory>

// Forward Declarations
class AtomType;

// AtomTypeData Definition
class AtomTypeData
{
    public:
    AtomTypeData(const AtomType *atomType, double population = 0, double fraction = 0, double boundCoherent = 0, int nIso = 0);

    /*
     * Properties
     */
    private:
    // Referenced AtomType
    const AtomType *atomType_;
    // Whether the AtomType has been marked as exchangeable
    bool exchangeable_{false};
    // Isotopes information (if any)
    std::vector<IsotopeData> isotopes_;
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
    void add(Sears91::Isotope tope, double nAdd);
    // Return referenced AtomType
    const AtomType *atomType() const;
    // Set exchangeable flag
    void setAsExchangeable();
    // Return whether the associated AtomType is exchangeable
    bool exchangeable() const;
    // Finalise, calculating fractional populations etc.
    void finalise(double nWorldAtoms);
    // Return the number of defined Isotopes
    int nIsotopes() const;
    // Return IsotopeData vector
    const std::vector<IsotopeData> &isotopeData() const;
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
