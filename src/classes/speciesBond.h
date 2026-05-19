// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "classes/bond.h"
#include "classes/bondFunctions.h"
#include "classes/speciesIntra.h"
#include <vector>

// Forward Declarations
class SpeciesAtom;
class Species;

// SpeciesBond Definition
class SpeciesBond : public Bond<SpeciesAtom>, public SpeciesIntra<SpeciesBond, BondFunctions>
{
    public:
    SpeciesBond();
    SpeciesBond(Species *parent, SpeciesAtom *i, SpeciesAtom *j);
    ~SpeciesBond() override;

    /*
     * SpeciesAtom Information
     */
    public:
    // Assign the two atoms in the bond
    void assign(SpeciesAtom *i, SpeciesAtom *j);
    // Set scaled intramolecular interactions on the involved atoms
    void addScaledInteractions();
    // Rewrite SpeciesAtom pointer
    void switchAtom(const SpeciesAtom *oldPtr, SpeciesAtom *newPtr);
    // Return first SpeciesAtom
    // Return vector of involved atoms
    std::vector<const SpeciesAtom *> atoms() const override;
    // Return index (in parent Species) of first SpeciesAtom
    int indexI() const;
    // Return index (in parent Species) of second SpeciesAtom
    int indexJ() const;
    // Return index (in parent Species) of nth SpeciesAtom
    int index(int n) const;
    // Return whether SpeciesAtoms match those specified
    bool matches(const SpeciesAtom *i, const SpeciesAtom *j) const;
    // Detach from current atoms
    void detach();

    /*
     * Interaction Parameters
     */
    public:
    // Return energy for specified distance
    double energy(double distance) const;
    // Return force multiplier for specified distance
    double force(double distance) const;

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};

// CommonBond Definition
class CommonBond : public SpeciesBond
{
    public:
    explicit CommonBond(std::string_view name) : SpeciesBond(), name_{name} {};
    using SpeciesIntra<SpeciesBond, BondFunctions>::deserialise;
    /*
     * Identifying Name
     */
    private:
    // Identifying name
    std::string name_;

    public:
    // Set identifying name
    void setName(std::string_view name) override { name_ = name; }
    // Return identifying name
    std::string_view name() const override { return name_; };
};
