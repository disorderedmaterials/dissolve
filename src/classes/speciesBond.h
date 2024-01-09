// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/serialiser.h"
#include "classes/speciesIntra.h"

#include <map>
#include <vector>

// Forward Declarations
class SpeciesAtom;
class Species;
class CoreData;

// Bond functional forms
class BondFunctions
{
    public:
    enum class Form
    {
        None,
        Harmonic,
        EPSR,
        Morse
    };
    // Return enum options for form
    static EnumOptions<Form> forms();
    // Return parameters for specified form
    static const std::vector<std::string> &parameters(Form form);
    // Return nth parameter for the given form
    static std::string parameter(Form form, int n);
    // Return index of parameter in the given form
    static std::optional<int> parameterIndex(Form form, std::string_view name);
};

// SpeciesBond Definition
class SpeciesBond : public SpeciesIntra<SpeciesBond, BondFunctions>
{
    public:
    SpeciesBond();
    SpeciesBond(SpeciesAtom *i, SpeciesAtom *j);
    ~SpeciesBond() override = default;
    SpeciesBond(SpeciesBond &source);
    SpeciesBond(SpeciesBond &&source) noexcept;
    SpeciesBond &operator=(const SpeciesBond &source);
    SpeciesBond &operator=(SpeciesBond &&source) noexcept;

    /*
     * SpeciesAtom Information
     */
    private:
    // First SpeciesAtom in interaction
    SpeciesAtom *i_{nullptr};
    // Second SpeciesAtom in interaction
    SpeciesAtom *j_{nullptr};

    public:
    // Assign the two atoms in the bond
    void assign(SpeciesAtom *i, SpeciesAtom *j);
    // Set scaled intramolecular interactions on the involved atoms
    void addScaledInteractions();
    // Rewrite SpeciesAtom pointer
    void switchAtom(const SpeciesAtom *oldPtr, SpeciesAtom *newPtr);
    // Return first SpeciesAtom
    SpeciesAtom *i() const;
    // Return second SpeciesAtom
    SpeciesAtom *j() const;
    // Return vector of involved atoms
    std::vector<const SpeciesAtom *> atoms() const override;
    // Return the 'other' SpeciesAtom
    SpeciesAtom *partner(const SpeciesAtom *i) const;
    // Return index (in parent Species) of first SpeciesAtom
    int indexI() const;
    // Return index (in parent Species) of second SpeciesAtom
    int indexJ() const;
    // Return index (in parent Species) of nth SpeciesAtom
    int index(int n) const;
    // Return whether SpeciesAtoms match those specified
    bool matches(const SpeciesAtom *i, const SpeciesAtom *j) const;
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
    // Return fundamental frequency for the interaction
    double fundamentalFrequency(double reducedMass) const override;
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
