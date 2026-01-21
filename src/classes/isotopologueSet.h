// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "classes/coreData.h"
#include "templates/optionalRef.h"

// Forward Declarations
class Species;
class Isotopologue;
class LineParser;

// IsotopologueSet - Isotopologues for one or more Species
class IsotopologueSet : public Serialisable<const CoreData &>
{
    public:
    IsotopologueSet() = default;
    ~IsotopologueSet() = default;
    // TODO: Improve equality operator
    bool operator==(const IsotopologueSet &other) { return this == &other; }

    /*
     * Data
     */
    private:
    // Isotopologue mixtures for individual Species
    ResolvableKeyedVector<const Species*, KeyedVector<const Isotopologue *, double>> isotopologues_;

    public:
    // Clear all existing data
    void clear();
    // Add Isotopologue with the specified relative weight
    void add(const Isotopologue *iso, double relativeWeight);
    // Remove specified Species from the list (if it exists)
    void remove(const Species *sp);
    // Remove any occurrences of the specified Isotopologue
    void remove(const Isotopologue *iso);
    // Return whether Isotopologues for the specified Species exists
    bool contains(const Species *sp) const;
    // Return Isotopologues with normalised populations for the specified Species
    const KeyedVector<const Isotopologue *, double> normalisedIsotopologues(const Species *sp) const;
    // Return number of species covered by set
    int nSpecies() const;
    // Return vector of all Isotopologues
    ResolvableKeyedVector<const Species*, KeyedVector<const Isotopologue *, double>>  &isotopologues();
    const ResolvableKeyedVector<const Species*, KeyedVector<const Isotopologue *, double>>  &isotopologues() const;

    /*
     * Serialisation
     */
    public:
    // Read data through specified LineParser
    bool deserialise(LineParser &parser, const CoreData &coreData);
    // Write data through specified LineParser
    bool write(LineParser &parser);
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};
