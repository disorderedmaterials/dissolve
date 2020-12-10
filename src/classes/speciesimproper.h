// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "classes/speciesintra.h"

// Forward Declarations
class SpeciesAtom;
class Species;
class ProcessPool;

// SpeciesImproper Definition
class SpeciesImproper : public SpeciesIntra
{
    public:
    SpeciesImproper();
    SpeciesImproper(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l);
    ~SpeciesImproper() = default;
    SpeciesImproper(SpeciesImproper &&source);

    /*
     * DynamicArrayObject Virtuals
     */
    public:
    // Clear object, ready for re-use
    void clear();

    /*
     * Atom Information
     */
    private:
    // First SpeciesAtom in interaction
    SpeciesAtom *i_;
    // Second SpeciesAtom in interaction
    SpeciesAtom *j_;
    // Third SpeciesAtom in interaction
    SpeciesAtom *k_;
    // Fourth SpeciesAtom in interaction
    SpeciesAtom *l_;

    public:
    // Set Atoms involved in Improper
    void assign(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l);
    // Return first SpeciesAtom
    SpeciesAtom *i() const;
    // Return second SpeciesAtom
    SpeciesAtom *j() const;
    // Return third SpeciesAtom
    SpeciesAtom *k() const;
    // Return fourth SpeciesAtom
    SpeciesAtom *l() const;
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
    bool matches(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l) const;
    // Return whether all atoms in the interaction are currently selected
    bool isSelected() const;

    /*
     * Interaction Parameters
     */
    public:
    // Improper functional forms
    enum ImproperFunction
    {
        NoForm,
        CosineForm,
        Cos3Form,
        Cos3CForm,
        Cos4Form,
        CosNForm,
        CosNCForm,
        UFFCosineForm
    };
    // Return enum options for ImproperFunction
    static EnumOptions<ImproperFunction> improperFunctions();

    public:
    // Set up any necessary parameters
    void setUp();
    // Return fundamental frequency for the interaction
    double fundamentalFrequency(double reducedMass) const;
    // Return type of this interaction
    SpeciesIntra::InteractionType type() const;
    // Return energy for specified angle
    double energy(double angleInDegrees) const;
    // Return force multiplier for specified angle
    double force(double angleInDegrees) const;
};
