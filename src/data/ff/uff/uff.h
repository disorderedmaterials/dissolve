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
    ~Forcefield_UFF() override = default;

    /*
     * Set Up
     */
    protected:
    // Set up / create all forcefield terms
    bool setUp() override;

    /*
     * Definition
     */
    public:
    // Return name of Forcefield
    std::string_view name() const override;
    // Return description for Forcefield
    std::string_view description() const override;
    // Return short-range interaction style for AtomTypes
    Forcefield::ShortRangeType shortRangeType() const override;

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
    OptionalReferenceWrapper<const UFFAtomType> determineUFFAtomType(const SpeciesAtom *i) const;

    protected:
    // Determine and return atom type for specified SpeciesAtom
    OptionalReferenceWrapper<const ForcefieldAtomType> determineAtomType(const SpeciesAtom &i) const override;

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

    protected:
    // Assign / generate bond term parameters
    bool assignBondTermParameters(SpeciesBond &bond, bool determineTypes) const override;
    // Assign / generate angle term parameters
    bool assignAngleTermParameters(SpeciesAngle &angle, bool determineTypes) const override;
    // Assign / generate torsion term parameters
    bool assignTorsionTermParameters(SpeciesTorsion &torsion, bool determineTypes) const override;
    // Assign / generate improper term parameters
    bool assignImproperTermParameters(ForcefieldImproperTerm &improper, SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k,
                                      SpeciesAtom *l, bool determineTypes) const override;
};
