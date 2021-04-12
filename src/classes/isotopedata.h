// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "templates/listitem.h"

// Forward Declarations
class CoreData;
class Isotope;
class LineParser;

/*
 * IsotopeData Definition
 */
class IsotopeData : public ListItem<IsotopeData>
{
    public:
    IsotopeData();
    IsotopeData(const IsotopeData &source);
    void operator=(const IsotopeData &source);

    /*
     * Properties
     */
    private:
    // Reference Isotope
    Isotope *isotope_;
    // Population of Isotope
    double population_;
    // Local fractional population (e.g. within an AtomTypeData)
    double fraction_;

    public:
    // Initialise
    bool initialise(Isotope *isotope);
    // Add to population of Isotope
    void add(double nAdd);
    // Finalise, calculating local fractional population (e.g. within an AtomTypeData)
    void finalise(double totalAtoms);
    // Zero population and fraction
    void zeroPopulation();
    // Return reference Isotope
    Isotope *isotope() const;
    // Return total population
    double population() const;
    // Return local fractional population (e.g. within an AtomTypeData)
    double fraction() const;

    /*
     * I/O
     */
    public:
    // Write data through specified LineParser
    bool serialise(LineParser &parser) const;
    // Read data through specified LineParser
    bool deserialise(LineParser &parser);
};
