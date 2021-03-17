// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/ff/ludwig/ntf2.h"

/*
 * Implements "Revisiting imidazolium based ionic liquids: Effect of the conformation bias of the [NTf2] anion studied by
 * molecular dynamics simulations" J. Neumann, B. Golub, LM. Odebrecht, R. Ludwig, D. Paschek Journal of Chemical Physics 148
 * 193828 (2018) http://doi.org/10.1063/1.5013096
 *
 * Notes:
 * Any inconsistencies between the forcefield as implemented here and the original work are the sole responsibility of JB.
 * All energy values are in kJ/mol.
 */

/*
 * Set Up
 */

// Set up / create all forcefield terms
bool Forcefield_Ludwig_NTf2::setUp()
{
    // Short-Range Parameters
    addParameters("F", {0.066516, 2.655});
    addParameters("C", {0.082812, 3.150});
    addParameters("S", {0.313705, 4.0825});
    addParameters("O", {0.263568, 3.4632});
    addParameters("N", {0.213349, 3.2500});

    // Atom Types
    addAtomType(Elements::F, 1, "F", "-C(-F(n=2))", "Fluorine", -0.189, "F");
    addAtomType(Elements::C, 2, "C", "-F(n=3),-S(-O(n=2))", "Carbon", 0.494, "C");
    addAtomType(Elements::S, 3, "S", "-O(n=2),-C,-N", "Sulfur", 1.076, "S");
    addAtomType(Elements::O, 4, "O", "-S(-O(n=2,root),-C,-N)", "Oxygen", -0.579, "O");
    addAtomType(Elements::N, 5, "N", "-S(-O(n=2),-C)", "Nitrogen", -0.69, "N");

    // Bond Terms
    addBondTerm("C", "F", SpeciesBond::HarmonicForm, {3697.0, 1.323});
    addBondTerm("C", "S", SpeciesBond::HarmonicForm, {1979.0, 1.818});
    addBondTerm("S", "O", SpeciesBond::HarmonicForm, {5331.0, 1.442});
    addBondTerm("N", "S", SpeciesBond::HarmonicForm, {3113.0, 1.570});

    // Angle Terms
    addAngleTerm("F", "C", "F", SpeciesAngle::HarmonicForm, {781.0, 107.1});
    addAngleTerm("S", "C", "F", SpeciesAngle::HarmonicForm, {694.0, 111.8});
    addAngleTerm("C", "S", "O", SpeciesAngle::HarmonicForm, {870.0, 102.6});
    addAngleTerm("O", "S", "O", SpeciesAngle::HarmonicForm, {969.0, 118.5});
    addAngleTerm("O", "S", "N", SpeciesAngle::HarmonicForm, {789.0, 113.6});
    addAngleTerm("C", "S", "N", SpeciesAngle::HarmonicForm, {816.0, 100.2});
    addAngleTerm("S", "N", "S", SpeciesAngle::HarmonicForm, {671.0, 125.6});

    // Torsion Terms
    addTorsionTerm("F", "C", "S", "N", SpeciesTorsion::CosineForm, {2.0401, 3.0, 0.0, 1});
    addTorsionTerm("F", "C", "S", "O", SpeciesTorsion::NoForm);
    addTorsionTerm("O", "S", "N", "S", SpeciesTorsion::NoForm);
    addTorsionTerm("S", "N", "S", "C", SpeciesTorsion::CosNForm, {23.7647, 6.2081, -2.3684, -0.0298, 0.6905, 1.0165});

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_Ludwig_NTf2::name() const { return "Ludwig/NTf2"; }

// Return description for Forcefield
std::string_view Forcefield_Ludwig_NTf2::description() const
{
    return "Implements 'Revisiting imidazolium based ionic liquids: Effect of the conformation bias of the [NTf2] anion "
           "studied by molecular dynamics simulations', J. Neumann, B. Golub, LM. Odebrecht, R. Ludwig, D. Paschek, "
           "<em>Journal of Chemical Physics</em>, <b>148</b>, 193828 (2018).";
}

// Return short-range interaction style for AtomTypes
Forcefield::ShortRangeType Forcefield_Ludwig_NTf2::shortRangeType() const { return Forcefield::LennardJonesType; }
