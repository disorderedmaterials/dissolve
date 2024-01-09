// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "classes/isotopologueWeight.h"
#include <vector>

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
    std::vector<IsotopologueWeight> mix_;

    public:
    // Set associated Species and population
    void setSpecies(const Species *sp, double population);
    // Return associated Species
    const Species *species() const;
    // Return associated Species population
    double speciesPopulation() const;
    // Add specific Isotopologue to list
    void add(const Isotopologue *iso, double relativeWeight);
    // Set Isotopologue component in list
    void set(const Isotopologue *iso, double relativeWeight);
    // Remove references to the specified Isotopologue
    void remove(const Isotopologue *iso);
    // Remove the specified IsotopologueWeight
    void remove(IsotopologueWeight *isoWeight);
    // Return whether the mix contains the specified Isotopologue
    bool contains(const Isotopologue *iso) const;
    // Return Isotopologue/weight mix
    std::vector<IsotopologueWeight> &mix();
    const std::vector<IsotopologueWeight> &mix() const;
    // Return number of Isotopologues in mix
    int nIsotopologues() const;
    // Return summed weight over all isotopologues
    double summedWeight() const;
    // Normalise total relative population to 1.0
    void normalise();

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
    SerialisedValue serialise() const override;
};
