// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "classes/speciesintra.h"

// Forward Declarations
class SpeciesAtom;
class Species;

// SpeciesBond Definition
class SpeciesBond : public SpeciesIntra
{
    public:
    SpeciesBond(SpeciesAtom *i = nullptr, SpeciesAtom *j = nullptr);
    ~SpeciesBond() = default;
    SpeciesBond(SpeciesBond &source);
    SpeciesBond(SpeciesBond &&source);
    SpeciesBond &operator=(const SpeciesBond &source);
    SpeciesBond &operator=(SpeciesBond &&source);

    /*
     * SpeciesAtom Information
     */
    private:
    // First SpeciesAtom in interaction
    SpeciesAtom *i_;
    // Second SpeciesAtom in interaction
    SpeciesAtom *j_;

    private:
    // Assign the two atoms in the bond
    void assign(SpeciesAtom *i, SpeciesAtom *j);

    public:
    // Return first SpeciesAtom
    SpeciesAtom *i() const;
    // Return second SpeciesAtom
    SpeciesAtom *j() const;
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
    BondType bondType_;

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
    // Bond functional forms
    enum BondFunction
    {
        NoForm,
        HarmonicForm,
        EPSRForm
    };
    // Return enum options for BondFunction
    static EnumOptions<BondFunction> bondFunctions();

    public:
    // Set up any necessary parameters
    void setUp();
    // Return fundamental frequency for the interaction
    double fundamentalFrequency(double reducedMass) const;
    // Return type of this interaction
    SpeciesIntra::InteractionType type() const;
    // Return energy for specified distance
    double energy(double distance) const;
    // Return force multiplier for specified distance
    double force(double distance) const;
};
