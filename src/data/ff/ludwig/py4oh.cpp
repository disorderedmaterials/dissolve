// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/ff/ludwig/py4oh.h"

/*
 * Implements "1‐(4‐hydroxybutyl)pyridinium cation based on OPLS All Atom Forcefield for benzene and pyridine"
 * W. L. Jorgensen, D. S. Maxwell, and J. Tirado-Rives
 * J. Am. Chem. Soc. 118, 11225-11236 (1996).
 * W. L. Jorgensen and N. A. McDonald, Theochem 424, 145-155 (1998).
 * W. L. Jorgensen and N. A. McDonald, J. Phys. Chem. B 102, 8049-8059 (1998).
 * Notes:
 * Any inconsistencies between the forcefield as implemented here and the original work are the sole responsibility of JB.
 * All energy values are in kJ/mol.
 */

/*
 * Set Up
 */

// Set up / create all forcefield terms
bool Forcefield_Ludwig_Py4OH::setUp()
{
    // Short-Range Parameters
    addParameters("nc", {0.711302, 3.250});
    addParameters("ca", {0.292919, 3.550});
    addParameters("ha", {0.125548, 2.420});
    addParameters("ct", {0.276040, 3.50});
    addParameters("hc", {0.125548, 2.50});
    addParameters("oh", {0.711718, 3.12});
    addParameters("ho", {0.000000, 0.0});

    // Atom Types
    addAtomType(Elements::N, 1, "nc", "nbonds=3,ring(size=6,C(n=5),N)", "Nitrogen in pyridine ring", 0.1014, "nc");
    addAtomType(Elements::C, 2, "ca_o", "nbonds=3,ring(size=6,C(n=5),N),-N,-C,-H", "Carbon in aromatic ring, ortho", 0.0568,
                "ca");
    addAtomType(Elements::C, 3, "ca_m", "nbonds=3,ring(size=6,C(n=5),N),-C(n=2),-H", "Carbon in aromatic ring, meta", -0.2214,
                "ca");
    addAtomType(Elements::C, 4, "ca_p", "nbonds=3,ring(size=6,C(n=5),N),-C(n=2,-C(-N))", "Carbon in aromatic ring, para",
                0.1747, "ca");
    addAtomType(Elements::H, 5, "ha_o", "nbonds=1,-&2", "Hydrogen bound to carbon in aromatic ring, ortho", 0.1802, "ha");
    addAtomType(Elements::H, 6, "ha_m", "nbonds=1,-&3", "Hydrogen bound to carbon in aromatic ring, meta", 0.1759, "ha");
    addAtomType(Elements::H, 7, "ha_p", "nbonds=1,-&4", "Hydrogen bound to carbon in aromatic ring, para", 0.1275, "ha");
    addAtomType(Elements::C, 8, "ct_1", "nbonds=4,nh=2,-&1,-C", "Alkane Carbon of Tail, adjacent to N", -0.1509, "ct");
    addAtomType(Elements::C, 9, "ct_2", "nbonds=4,nh=2,-&8,-C", "Alkane Carbon of Tail,nextnext=N", 0.0160, "ct");
    addAtomType(Elements::C, 10, "ct_3", "nbonds=4,nh=2,-C(-H(n=2),-O)", "Alkane Carbon of Tail,nextnext=O", 0.0780, "ct");
    addAtomType(Elements::C, 11, "ct_4", "nbonds=4,nh=2,-C(-H(n=2),-C),-O", "Alkane Carbon of Tail, adjacent to O", 0.2911,
                "ct");
    addAtomType(Elements::H, 12, "hc_1", "nbonds=1,-&8", "Hydrogen of Tail, adjacent to N", 0.1015, "hc");
    addAtomType(Elements::H, 13, "hc_2", "nbonds=1,-&9", "Hydrogen of Tail, nextnext=N", 0.0043, "hc");
    addAtomType(Elements::H, 14, "hc_3", "nbonds=1,-&10", "Hydrogen of Tail, nextnext=O", 0.0316, "hc");
    addAtomType(Elements::H, 15, "hc_4", "nbonds=1,-&11", "Hydrogen of Tail, adjacent to O", -0.0205, "hc");
    addAtomType(Elements::O, 16, "oh", "nbonds=2,-H,-&11", "Oxygen of Hydroxyl Group", -0.6916, "oh");
    addAtomType(Elements::H, 17, "ho", "nbonds=1,-&16", "Hydrogen of Hydroxyl Group", 0.4370, "ho");

    // Bond Terms
    addBondTerm("ha", "ca", SpeciesBond::HarmonicForm, {3071., 1.080});
    addBondTerm("ca", "ca", SpeciesBond::HarmonicForm, {3925., 1.400});
    addBondTerm("nc", "ca", SpeciesBond::HarmonicForm, {4042., 1.339});
    addBondTerm("nc", "ct", SpeciesBond::HarmonicForm, {4042, 1.339});
    addBondTerm("ct", "ct", SpeciesBond::HarmonicForm, {2244.1, 1.529});
    addBondTerm("ct", "hc", SpeciesBond::HarmonicForm, {2847.0, 1.09});
    addBondTerm("ct", "oh", SpeciesBond::HarmonicForm, {2679.6, 1.41});
    addBondTerm("ho", "oh", SpeciesBond::HarmonicForm, {4630.6, 0.945});

    // Angle Terms
    addAngleTerm("ca", "ca", "ca", SpeciesAngle::HarmonicForm, {527.2, 120.0});
    addAngleTerm("ca", "ca", "nc", SpeciesAngle::HarmonicForm, {585.8, 124.0});
    addAngleTerm("ca", "nc", "ca", SpeciesAngle::HarmonicForm, {585.8, 117.0});
    addAngleTerm("ca", "ca", "ha", SpeciesAngle::HarmonicForm, {292.9, 120.0});
    addAngleTerm("nc", "ca", "ha", SpeciesAngle::HarmonicForm, {292.9, 116.0});
    addAngleTerm("ca", "nc", "ct", SpeciesAngle::HarmonicForm, {585.8, 121.5});
    addAngleTerm("nc", "ct", "ct", SpeciesAngle::HarmonicForm, {487.43, 112.7});
    addAngleTerm("hc", "ct", "nc", SpeciesAngle::HarmonicForm, {313.26, 110.7});
    addAngleTerm("hc", "ct", "hc", SpeciesAngle::HarmonicForm, {275.7, 107.8});
    addAngleTerm("hc", "ct", "ct", SpeciesAngle::HarmonicForm, {313.26, 110.7});
    addAngleTerm("ct", "ct", "ct", SpeciesAngle::HarmonicForm, {487.43, 112.7});
    addAngleTerm("ho", "oh", "ct", SpeciesAngle::HarmonicForm, {460.55, 108.5});
    addAngleTerm("ct", "ct", "oh", SpeciesAngle::HarmonicForm, {418.68, 109.5});
    addAngleTerm("hc", "ct", "oh", SpeciesAngle::HarmonicForm, {293.08, 109.5});

    // Torsion Terms
    addTorsionTerm("*", "ca", "ca", "*", SpeciesTorsion::CosineForm, {15.178, 2.0, 180.0, 1});
    addTorsionTerm("*", "ca", "nc", "*", SpeciesTorsion::CosineForm, {15.178, 2.0, 180.0, 1});
    addTorsionTerm("ca", "nc", "ct", "ct", SpeciesTorsion::CosNForm, {0.0, -0.3579, 0.0, -0.4037});
    addTorsionTerm("nc", "ct", "ct", "ct", SpeciesTorsion::CosNForm, {-3.7014, 0.0, 6.9923});
    addTorsionTerm("ct", "ct", "ct", "ct", SpeciesTorsion::CosNForm, {-0.2825, 0.6065, 4.6858, 0.7018, 0.4468, 0.4564});
    addTorsionTerm("ct", "ct", "ct", "oh", SpeciesTorsion::CosNForm, {-2.3748, 0.0, 6.8089, 0.9531});
    addTorsionTerm("ct", "ct", "oh", "ho", SpeciesTorsion::CosNForm, {-3.5552, 0.5886, 2.5272, 0.1504});
    addTorsionTerm("hc", "ct", "ct", "hc", SpeciesTorsion::CosineForm, {0.0000, 3.0, 0.0, 1});
    addTorsionTerm("hc", "ct", "ct", "ct", SpeciesTorsion::CosineForm, {0.0000, 3.0, 0.0, 1});
    addTorsionTerm("hc", "ct", "nc", "ca", SpeciesTorsion::CosineForm, {0.0000, 3.0, 0.0, 1});
    addTorsionTerm("hc", "ct", "ct", "nc", SpeciesTorsion::CosineForm, {0.0000, 3.0, 0.0, 1});
    addTorsionTerm("hc", "ct", "ct", "oh", SpeciesTorsion::CosineForm, {0.0000, 3.0, 0.0, 1});
    addTorsionTerm("hc", "ct", "oh", "ho", SpeciesTorsion::CosineForm, {0.0000, 3.0, 0.0, 1});

    // Improper Terms
    addImproperTerm("ca", "ca", "ca", "ha", SpeciesTorsion::CosineForm, {4.606, 2.0, 180.0, 1});
    addImproperTerm("ca", "nc", "ca", "ha", SpeciesTorsion::CosineForm, {4.606, 2.0, 180.0, 1});
    addImproperTerm("nc", "ca", "ca", "ct", SpeciesTorsion::CosineForm, {4.606, 2.0, 180.0, 1});

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_Ludwig_Py4OH::name() const { return "Ludwig/Py4OH"; }

// Return description for Forcefield
std::string_view Forcefield_Ludwig_Py4OH::description() const
{
    return "Implements of 1‐(4‐hydroxybutyl)pyridinium cation based on OPLS All Atom Forcefield for benzene and pyridine; "
           "W. L. Jorgensen,	D. S. Maxwell, and J. Tirado-Rives, <i>Journal of the "
           "American Chemical Society</i>, <b>118</b>, 11225 (1996).";
}

// Return short-range interaction style for AtomTypes
Forcefield::ShortRangeType Forcefield_Ludwig_Py4OH::shortRangeType() const { return Forcefield::LennardJonesType; }
