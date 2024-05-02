// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/serialiser.h"
#include "classes/speciesIntra.h"
#include "classes/speciesTorsion.h"

#include <map>
#include <vector>

// Forward Declarations
class SpeciesAtom;
class Species;

// SpeciesImproper Definition
class SpeciesImproper : public SpeciesIntra<SpeciesImproper, TorsionFunctions>
{
    public:
    SpeciesImproper();
    SpeciesImproper(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l);
    ~SpeciesImproper() override;
    SpeciesImproper(SpeciesImproper &source);
    SpeciesImproper(SpeciesImproper &&source) noexcept;
    SpeciesImproper &operator=(const SpeciesImproper &source);
    SpeciesImproper &operator=(SpeciesImproper &&source) noexcept;

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

    private:
    // Detach from current atoms
    void detach();

    public:
    // Set Atoms involved in Improper
    void assign(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l);
    // Rewrite SpeciesAtom pointer
    void switchAtom(const SpeciesAtom *oldPtr, SpeciesAtom *newPtr);
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
    // Return whether the improper uses the specified SpeciesAtom
    bool uses(SpeciesAtom *spAtom) const;
    // Return index (in parent Species) of first SpeciesAtom
    int indexI() const;
    // Return index (in parent Species) of second SpeciesAtom
    int indexJ() const;
    // Return index (in parent Species) of third SpeciesAtom
    int indexK() const;
    // Return index (in parent Species) of fourth SpeciesAtom
    int indexL() const;
    // Return index (in parent Species) of nth SpeciesAtom in interaction
    int index(int n) const;
    // Return whether SpeciesAtoms match those specified
    bool matches(const SpeciesAtom *i, const SpeciesAtom *j, const SpeciesAtom *k, const SpeciesAtom *l) const;
    // Return whether all atoms in the interaction are currently selected
    bool isSelected() const;

    /*
     * Interaction Parameters
     */
    public:
    // Return fundamental frequency for the interaction
    double fundamentalFrequency(double reducedMass) const override;
    // Return energy for specified angle
    double energy(double angleInDegrees) const;
    // Return force multiplier for specified angle
    double force(double angleInDegrees) const;

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
