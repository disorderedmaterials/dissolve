// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/serialiser.h"
#include "classes/speciesIntra.h"
#include "classes/speciesTorsion.h"

#include <map>
#include <vector>

// Forward Declarations
class SpeciesParticle;
class Species;

// SpeciesImproper Definition
class SpeciesImproper : public SpeciesIntra<SpeciesImproper, TorsionFunctions>
{
    public:
    SpeciesImproper();
    SpeciesImproper(SpeciesParticle *i, SpeciesParticle *j, SpeciesParticle *k, SpeciesParticle *l);
    ~SpeciesImproper() override;
    SpeciesImproper(SpeciesImproper &source);
    SpeciesImproper(SpeciesImproper &&source) noexcept;
    SpeciesImproper &operator=(const SpeciesImproper &source);
    SpeciesImproper &operator=(SpeciesImproper &&source) noexcept;

    /*
     * Atom Information
     */
    private:
    // First SpeciesParticle in interaction
    SpeciesParticle *i_{nullptr};
    // Second SpeciesParticle in interaction
    SpeciesParticle *j_{nullptr};
    // Third SpeciesParticle in interaction
    SpeciesParticle *k_{nullptr};
    // Fourth SpeciesParticle in interaction
    SpeciesParticle *l_{nullptr};

    private:
    // Detach from current atoms
    void detach();

    public:
    // Set Atoms involved in Improper
    void assign(SpeciesParticle *i, SpeciesParticle *j, SpeciesParticle *k, SpeciesParticle *l);
    // Rewrite SpeciesParticle pointer
    void switchAtom(const SpeciesParticle *oldPtr, SpeciesParticle *newPtr);
    // Return first SpeciesParticle
    SpeciesParticle *i() const;
    // Return second SpeciesParticle
    SpeciesParticle *j() const;
    // Return third SpeciesParticle
    SpeciesParticle *k() const;
    // Return fourth SpeciesParticle
    SpeciesParticle *l() const;
    // Return vector of involved atoms
    std::vector<const SpeciesParticle *> atoms() const override;
    // Return whether the improper uses the specified SpeciesParticle
    bool uses(SpeciesParticle *spAtom) const;
    // Return index (in parent Species) of first SpeciesParticle
    int indexI() const;
    // Return index (in parent Species) of second SpeciesParticle
    int indexJ() const;
    // Return index (in parent Species) of third SpeciesParticle
    int indexK() const;
    // Return index (in parent Species) of fourth SpeciesParticle
    int indexL() const;
    // Return index (in parent Species) of nth SpeciesParticle in interaction
    int index(int n) const;
    // Return whether SpeciesParticles match those specified
    bool matches(const SpeciesParticle *i, const SpeciesParticle *j, const SpeciesParticle *k, const SpeciesParticle *l) const;
    // Return whether all atoms in the interaction are currently selected
    bool isSelected() const;

    /*
     * Interaction Parameters
     */
    public:
    // Return energy for specified angle phi (in radians)
    double energy(double phi) const;
    // Return force multiplier for specified angle phi (in radians)
    double force(double phi) const;

    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, CoreData &coreData);
};

// MasterImproper Definition
class MasterImproper : public SpeciesImproper
{
    public:
    explicit MasterImproper(std::string_view name) : SpeciesImproper(), name_{name} {};
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
