// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"

// Forward Declarations
class Species;
class Isotopologue;

// IsotopologueSet - Isotopologues for one or more Species
class IsotopologueSet : ResolvableContext
{
    public:
    IsotopologueSet() = default;
    IsotopologueSet(const std::vector<std::pair<const Isotopologue *, double>> &topes);
    ~IsotopologueSet() = default;
    // TODO: Improve equality operator
    bool operator==(const IsotopologueSet &other) { return this == &other; }

    /*
     * Data
     */
    private:
    // Isotopologue mixtures for individual Species
    ResolvableKeyedVector<const Species *, ResolvableKeyedVector<const Isotopologue *, double>> isotopologues_;

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
    std::map<const Isotopologue *, double> normalisedIsotopologues(const Species *sp) const;
    // Return number of species covered by set
    int nSpecies() const;
    // Return vector of all Isotopologues
    ResolvableKeyedVector<const Species *, ResolvableKeyedVector<const Isotopologue *, double>> &isotopologues();
    const ResolvableKeyedVector<const Species *, ResolvableKeyedVector<const Isotopologue *, double>> &isotopologues() const;

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node);
    // Resolve internal resolvable name references with supplied data
    void resolve(const std::map<std::string, const Species *> &speciesInScope) override;
};
