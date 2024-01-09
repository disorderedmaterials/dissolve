// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/ff/ff.h"
#include "templates/optionalRef.h"

// Forward Declarations
class SpeciesAngle;
class SpeciesAtom;
class SpeciesBond;
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
    // Create basic UFF atom types
    void createAtomTypes();
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
    ShortRangeFunctions::Form shortRangeForm() const override;

    /*
     * Atom Type Data
     */
    protected:
    // UFF atom type data
    enum UFFAtomTypeData
    {
        Epsilon, /* LJ Epsilon, calculated from UFF data */
        Sigma,   /* LJ Sigma, calculated from UFF data */
        R,       /* single bond radius (Angstroms) */
        Theta,   /* equilibrium angle geometry (degrees) */
        X,       /* nonbond distance (Angstroms) */
        D,       /* nonbond dissociation energy (kcal/mol) */
        Zeta,    /* nonbond scale parameter */
        Z,       /* effective charge */
        Chi,     /* GMP electronegativity */
        V,       /* sp3 torsional barrier parameter (kcal/mol) */
        U        /* sp2 torsional barrier parameter (kcal/mol) */
    };

    protected:
    // Add new atom type with its own parameters
    void addUFFAtomType(Elements::Element Z, int index, std::string_view name, std::string_view netaDefinition,
                        std::string_view description, double r, double theta, double x, double D, double zeta,
                        double effectiveCharge, double chi, double V, double U);

    /*
     * Term Assignment
     */
    private:
    // Return bond order for specified atom type pair
    double bondOrder(const ForcefieldAtomType &i, const ForcefieldAtomType &j) const;
    // Return bond order correction for specified atom type pair
    double bondOrderCorrection(const ForcefieldAtomType &i, const ForcefieldAtomType &j) const;
    // Return electronegativity correction for specified atom type pair
    double electronegativityCorrection(const ForcefieldAtomType &i, const ForcefieldAtomType &j) const;

    protected:
    // Assign / generate bond term parameters
    bool assignBondTermParameters(const Species *parent, SpeciesBond &bond, bool determineTypes) const override;
    // Assign / generate angle term parameters
    bool assignAngleTermParameters(const Species *parent, SpeciesAngle &angle, bool determineTypes) const override;
    // Assign / generate torsion term parameters
    bool assignTorsionTermParameters(const Species *parent, SpeciesTorsion &torsion, bool determineTypes) const override;
    // Assign / generate improper term parameters
    bool assignImproperTermParameters(ForcefieldImproperTerm &improper, SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k,
                                      SpeciesAtom *l, bool determineTypes) const override;
};
