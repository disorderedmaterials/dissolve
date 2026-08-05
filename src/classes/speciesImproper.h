// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "classes/speciesIntra.h"
#include "classes/speciesTorsion.h"
#include <vector>

// Forward Declarations
class SpeciesAtom;
class Species;

// SpeciesImproper Definition
class SpeciesImproper : public SpeciesIntra<SpeciesImproper, TorsionFunctions>
{
    public:
    SpeciesImproper();
    SpeciesImproper(Species *parent, SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l);
    ~SpeciesImproper() override;

    /*
     * Atom Information
     */
    private:
    // First SpeciesAtom in interaction
    SpeciesAtom *i_{nullptr};
    // Second SpeciesAtom in interaction
    SpeciesAtom *j_{nullptr};
    // Third SpeciesAtom in interaction
    SpeciesAtom *k_{nullptr};
    // Fourth SpeciesAtom in interaction
    SpeciesAtom *l_{nullptr};

    public:
    // Return first SpeciesAtom
    SpeciesAtom *i() const;
    // Return second SpeciesAtom
    SpeciesAtom *j() const;
    // Return third SpeciesAtom
    SpeciesAtom *k() const;
    // Return fourth SpeciesAtom
    SpeciesAtom *l() const;
    // Return vector of involved atoms
    std::vector<const SpeciesAtom *> atoms() const override;
    // Return whether SpeciesAtoms match those specified
    bool matches(const SpeciesAtom *i, const SpeciesAtom *j, const SpeciesAtom *k, const SpeciesAtom *l) const;

    /*
     * Interaction Parameters
     */
    public:
    // Return energy for specified angle phi (in radians)
    double energy(double phi) const;
    // Return force multiplier for specified angle phi (in radians)
    double force(double phi) const;

    /*
     * Serialisation
     */
    public:
    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node);
};

// CommonImproper Definition
class CommonImproper : public SpeciesImproper
{
    public:
    explicit CommonImproper(std::string_view name) : SpeciesImproper(), name_{name} {};
    using SpeciesIntra<SpeciesImproper, TorsionFunctions>::deserialise;

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
