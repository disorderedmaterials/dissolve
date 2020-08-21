/*
    *** SpeciesTorsion Definition
    *** src/classes/speciestorsion.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "base/enumoptions.h"
#include "classes/speciesintra.h"
#include "templates/dynamicarrayobject.h"

// Forward Declarations
class SpeciesAtom;
class Species;
class ProcessPool;

// SpeciesTorsion Definition
class SpeciesTorsion : public SpeciesIntra
{
    public:
    SpeciesTorsion(const SpeciesTorsion &source);
    SpeciesTorsion(const SpeciesTorsion &&source);
    SpeciesTorsion(SpeciesAtom *i = nullptr, SpeciesAtom *j = nullptr, SpeciesAtom *k = nullptr, SpeciesAtom *l = nullptr);
    ~SpeciesTorsion();
    SpeciesTorsion &operator=(const SpeciesTorsion &source);
    SpeciesTorsion &operator=(SpeciesTorsion &&source);

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
    // Detach from current atoms
    void detach();

    public:
    // Set Atoms involved in Torsion
    void assign(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l);
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
        UFFCosineForm
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
    // Return energy for specified angle
    double energy(double angleInDegrees) const;
    // Return force multiplier for specified angle
    double force(double angleInDegrees) const;
};
