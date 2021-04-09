// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptions.h"
#include "classes/speciesintra.h"
#include "templates/dynamicarrayobject.h"

// Forward Declarations
class SpeciesAtom;
class Species;

// SpeciesTorsion Definition
class SpeciesTorsion : public SpeciesIntra
{
    public:
    SpeciesTorsion(SpeciesTorsion &source);
    SpeciesTorsion(SpeciesTorsion &&source);
    SpeciesTorsion(SpeciesAtom *i = nullptr, SpeciesAtom *j = nullptr, SpeciesAtom *k = nullptr, SpeciesAtom *l = nullptr);
    ~SpeciesTorsion();
    SpeciesTorsion &operator=(const SpeciesTorsion &source);
    SpeciesTorsion &operator=(SpeciesTorsion &&source);

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

    private:
    // Set Atoms involved in Torsion
    void assign(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l);
    // Detach from current atoms
    void detach();

    public:
    // Return first SpeciesAtom
    SpeciesAtom *i() const;
    // Return second SpeciesAtom
    SpeciesAtom *j() const;
    // Return third SpeciesAtom
    SpeciesAtom *k() const;
    // Return fourth SpeciesAtom
    SpeciesAtom *l() const;
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
    // Torsion functional forms
    enum TorsionFunction
    {
        NoForm,
        CosineForm,
        Cos3Form,
        Cos3CForm,
        Cos4Form,
        CosNForm,
        CosNCForm,
        UFFCosineForm,
        FourierNForm
    };
    // Return enum options for TorsionFunction
    static EnumOptions<TorsionFunction> torsionFunctions();

    public:
    // Set up any necessary parameters
    void setUp();
    // Return fundamental frequency for the interaction
    double fundamentalFrequency(double reducedMass) const;
    // Return type of this interaction
    SpeciesIntra::InteractionType type() const;
    // Return energy for specified angle and functional form, given supplied parameters
    static double energy(double angleInDegrees, int form, const std::vector<double> &params);
    // Return energy for specified angle
    double energy(double angleInDegrees) const;
    // Return force multiplier for specified angle and functional form, given supplied parameters
    static double force(double angleInDegrees, int form, const std::vector<double> &params);
    // Return force multiplier for specified angle
    double force(double angleInDegrees) const;
};
