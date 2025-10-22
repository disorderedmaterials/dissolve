// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "templates/keyedVector.h"
#include <map>

// Forward Declarations
class CoreData;
class Isotopologue;
class LineParser;
class Species;

// Isotopologues
class Isotopologues : public Serialisable<const CoreData &>
{
    public:
    Isotopologues(const Species *species = nullptr, int speciesPopulation = 0);
    ~Isotopologues() = default;

    /*
     * Isotopologue Mixture Data
     */
    private:
    // Associated Species
    const Species *species_;
    // Population of associated Species
    double speciesPopulation_;
    // Weighted Isotopologue mixture
    KeyedVector<const Isotopologue *, double> mix_;

    public:
    // Set associated Species and population
    void setSpecies(const Species *sp, double population);
    // Return associated Species
    const Species *species() const;
    // Return associated Species population
    double speciesPopulation() const;
    // Return Isotopologue/weight mix
    KeyedVector<const Isotopologue *, double> &mix();
    const KeyedVector<const Isotopologue *, double> &mix() const;
    // Return summed weight over all isotopologues
    double summedWeight() const;
    // Return the normalised populations
    KeyedVector<const Isotopologue *, double> normalised() const;

    /*
     * Serialisation
     */
    public:
    // Read data through specified LineParser
    bool deserialise(LineParser &parser, const CoreData &coreData);
    // Write data through specified LineParser
    bool serialise(LineParser &parser) const;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
    // Express as a serialisable value
    void serialize(std::string tag, SerialisedValue &target) const override;
};
