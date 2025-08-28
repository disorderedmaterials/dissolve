// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/serialiser.h"
#include "classes/bondFunctions.h"
#include "classes/speciesIntra.h"
#include <map>
#include <vector>

// Forward Declarations
class SpeciesParticle;
class Species;
class CoreData;

// SpeciesBond Definition
class SpeciesBond : public SpeciesIntra<SpeciesBond, BondFunctions>
{
    public:
    SpeciesBond();
    SpeciesBond(SpeciesParticle *i, SpeciesParticle *j);
    ~SpeciesBond() override = default;
    SpeciesBond(SpeciesBond &source);
    SpeciesBond(SpeciesBond &&source) noexcept;
    SpeciesBond &operator=(const SpeciesBond &source);
    SpeciesBond &operator=(SpeciesBond &&source) noexcept;

    /*
     * SpeciesParticle Information
     */
    private:
    // First SpeciesParticle in interaction
    SpeciesParticle *i_{nullptr};
    // Second SpeciesParticle in interaction
    SpeciesParticle *j_{nullptr};

    public:
    // Assign the two atoms in the bond
    void assign(SpeciesParticle *i, SpeciesParticle *j);
    // Set scaled intramolecular interactions on the involved atoms
    void addScaledInteractions();
    // Rewrite SpeciesParticle pointer
    void switchAtom(const SpeciesParticle *oldPtr, SpeciesParticle *newPtr);
    // Return first SpeciesParticle
    SpeciesParticle *i() const;
    // Return second SpeciesParticle
    SpeciesParticle *j() const;
    // Return vector of involved atoms
    std::vector<const SpeciesParticle *> atoms() const override;
    // Return the 'other' SpeciesParticle
    SpeciesParticle *partner(const SpeciesParticle *i) const;
    // Return index (in parent Species) of first SpeciesParticle
    int indexI() const;
    // Return index (in parent Species) of second SpeciesParticle
    int indexJ() const;
    // Return index (in parent Species) of nth SpeciesParticle
    int index(int n) const;
    // Return whether SpeciesParticles match those specified
    bool matches(const SpeciesParticle *i, const SpeciesParticle *j) const;
    // Return whether all atoms in the interaction are currently selected
    bool isSelected() const;
    // Detach from current atoms
    void detach();

    /*
     * Bond Type
     */
    public:
    // Bond Type enum
    enum BondType
    {
        SingleBond,
        DoubleBond,
        TripleBond,
        QuadrupleBond,
        AromaticBond,
        nBondTypes
    };
    // Convert bond type string to functional form
    static BondType bondType(std::string_view s);
    // Return bond type functional form text
    static std::string_view bondType(BondType bt);
    // Return bond order for specified bond type
    static double bondOrder(BondType bt);

    private:
    // Bond type
    BondType bondType_{SpeciesBond::SingleBond};

    public:
    // Set bond type
    void setBondType(BondType type);
    // Return bond type
    BondType bondType() const;
    // Return bond order for current bond type
    double bondOrder() const;

    /*
     * Interaction Parameters
     */
    public:
    // Return energy for specified distance
    double energy(double distance) const;
    // Return force multiplier for specified distance
    double force(double distance) const;

    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, CoreData &coreData);
};

// MasterBond Definition
class MasterBond : public SpeciesBond
{
    public:
    explicit MasterBond(std::string_view name) : SpeciesBond(), name_{name} {};
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
