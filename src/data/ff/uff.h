/*
    *** Universal Forcefield
    *** src/data/ff/uff.h
    Copyright T. Youngs 2019-2020

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

#include "data/ff.h"
#include "data/ff/uffatomtype.h"
#include "templates/optionalref.h"

// Forward Declarations
class CoreData;
class Species;
class SpeciesAngle;
class SpeciesAtom;
class SpeciesBond;
class SpeciesIntra;
class SpeciesTorsion;

// Universal Forcefield
class Forcefield_UFF : public Forcefield
{
    public:
    Forcefield_UFF();
    ~Forcefield_UFF();

    /*
     * Set Up
     */
    public:
    // Set up / create all forcefield data ready for use
    bool setUp();

    /*
     * Definition
     */
    public:
    // Return name of Forcefield
    const char *name() const;
    // Return description for Forcefield
    const char *description() const;
    // Return short-range interaction style for AtomTypes
    Forcefield::ShortRangeType shortRangeType() const;

    /*
     * Atom Type Data
     */
    private:
    // Return UFF atom types
    const std::vector<UFFAtomType> &uffAtomTypes() const;
    // Return UFF atom type with name specified
    OptionalReferenceWrapper<const UFFAtomType> uffAtomTypeByName(const char *name) const;
    // Return first UFF atom type for specified element
    OptionalReferenceWrapper<const UFFAtomType> uffAtomTypeForElement(int el) const;
    // Determine and return atom type for specified SpeciesAtom
    OptionalReferenceWrapper<const UFFAtomType> determineUFFAtomType(SpeciesAtom *i) const;

    /*
     * Term Assignment
     */
    private:
    // Generate bond parameters for the supplied UFF atom types
    bool generateBondTerm(const Species *sp, SpeciesBond *bondTerm, const UFFAtomType &i, const UFFAtomType &j) const;
    // Generate angle parameters for the supplied UFF atom types
    bool generateAngleTerm(const Species *sp, SpeciesAngle *angleTerm, const UFFAtomType &i, const UFFAtomType &j, const UFFAtomType &k) const;
    // Generate torsion parameters for the supplied UFF atom types
    bool generateTorsionTerm(const Species *sp, SpeciesTorsion *torsionTerm, const UFFAtomType &i, const UFFAtomType &j, const UFFAtomType &k, const UFFAtomType &l) const;

    public:
    // Assign suitable AtomTypes to the supplied Species
    bool assignAtomTypes(Species *sp, CoreData &coreData, bool keepExisting = false) const;
    // Assign intramolecular parameters to the supplied Species
    bool assignIntramolecular(Species *sp, int flags = 0) const;
    // Perform some test calculations
    void test() const;
};
