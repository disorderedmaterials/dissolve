// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/isotopologues.h"
#include "templates/optionalref.h"
#include <vector>

// Forward Declarations
class Species;
class Isotopologue;
class LineParser;

// IsotopologueSet - Isotopologues for one or more Species
class IsotopologueSet
{
    public:
    IsotopologueSet() = default;
    ~IsotopologueSet() = default;

    /*
     * Data
     */
    private:
    // Isotopologue mixtures for individual Species
    std::vector<Isotopologues> isotopologues_;

    public:
    // Clear all existing data
    void clear();
    // Add Isotopologue with the specified relative weight
    void add(const Isotopologue *iso, double relativeWeight);
    // Remove specified Species from the list (if it exists)
    void remove(const Species *sp);
    // Remove any occurrences of the specified Isotopologue
    void remove(const Isotopologue *iso);
    // Remove the specified IsotopologueWeight
    void remove(IsotopologueWeight *isoWeight);
    // Return whether Isotopologues for the specified Species exists
    bool contains(const Species *sp) const;
    // Return Isotopologues for the specified Species
    OptionalReferenceWrapper<const Isotopologues> getIsotopologues(const Species *sp) const;
    // Return number of Isotopologues defined
    int nIsotopologues() const;
    // Return vector of all Isotopologues
    std::vector<Isotopologues> &isotopologues();
    const std::vector<Isotopologues> &isotopologues() const;

    /*
     * Serialisation
     */
    public:
    // Read data through specified LineParser
    bool deserialise(LineParser &parser, const CoreData &coreData);
    // Write data through specified LineParser
    bool write(LineParser &parser);
};
