// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/ff/ff.h"
#include "data/ff/uff/uffatomtype.h"
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
    Forcefield_UFF() = default;
    ~Forcefield_UFF() = default;

    /*
     * Set Up
     */
    protected:
    // Set up / create all forcefield terms
    bool setUp();

    /*
     * Definition
     */
    public:
    // Return name of Forcefield
    std::string_view name() const;
    // Return description for Forcefield
    std::string_view description() const;
    // Return short-range interaction style for AtomTypes
    Forcefield::ShortRangeType shortRangeType() const;

    /*
     * Atom Type Data
     */
    private:
    // UFF atom types
    std::vector<UFFAtomType> uffAtomTypes_;

    private:
    // Return UFF atom type with name specified
    OptionalReferenceWrapper<const UFFAtomType> uffAtomTypeByName(std::string_view name) const;
    // Return first UFF atom type for specified element
    OptionalReferenceWrapper<const UFFAtomType> uffAtomTypeForElement(int el) const;
    // Determine and return atom type for specified SpeciesAtom
    OptionalReferenceWrapper<const UFFAtomType> determineUFFAtomType(SpeciesAtom *i) const;

    protected:
    // Determine and return atom type for specified SpeciesAtom
    OptionalReferenceWrapper<const ForcefieldAtomType> determineAtomType(SpeciesAtom *i) const;

    /*
     * Term Assignment
     */
    private:
    // Return bond order for specified atom type pair
    double bondOrder(const UFFAtomType &i, const UFFAtomType &j) const;
    // Return bond order correction for specified atom type pair
    double bondOrderCorrection(const UFFAtomType &i, const UFFAtomType &j) const;
    // Return electronegativity correction for specified atom type pair
    double electronegativityCorrection(const UFFAtomType &i, const UFFAtomType &j) const;
    // Generate bond parameters for the supplied UFF atom types
    bool generateBondTerm(const Species *sp, SpeciesBond &bond, const UFFAtomType &i, const UFFAtomType &j) const;
    // Generate angle parameters for the supplied UFF atom types
    bool generateAngleTerm(const Species *sp, SpeciesAngle &angle, const UFFAtomType &i, const UFFAtomType &j,
                           const UFFAtomType &k) const;
    // Generate torsion parameters for the supplied UFF atom types
    bool generateTorsionTerm(const Species *sp, SpeciesTorsion &torsionTerm, const UFFAtomType &i, const UFFAtomType &j,
                             const UFFAtomType &k, const UFFAtomType &l) const;

    public:
    // Assign intramolecular parameters to the supplied Species
    bool assignIntramolecular(Species *sp, int flags = 0) const;
    // Perform some test calculations
    void test() const;
};
