// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "data/ff/ludwig/py5.h"

/*
 * Implements "1-pentylpyridinium cation based on OPLS All Atom Forcefield for benzene and pyridine"
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
bool Forcefield_Ludwig_Py5::setUp()
{
    // Short-Range Parameters
    addParameters("nc", {0.711302, 3.250});
    addParameters("ca", {0.292919, 3.550});
    addParameters("ha", {0.125548, 2.420});
    addParameters("ct", {0.276040, 3.50});
    addParameters("hc", {0.125548, 2.50});
    addParameters("cm", {0.276040, 3.50});
    addParameters("hm", {0.125548, 2.50});

    // Atom Types
    addAtomType(Elements::N, 1, "nc", "nbonds=3,ring(size=6,C(n=5),N)", "Nitrogen in pyridine ring", 0.0749, "nc");
    addAtomType(Elements::C, 2, "ca_o", "nbonds=3,ring(size=6,C(n=5),N),-N,-C,-H", "Carbon in aromatic ring, ortho", 0.0725,
                "ca");
    addAtomType(Elements::C, 3, "ca_m", "nbonds=3,ring(size=6,C(n=5),N),-C,-H,-C", "Carbon in aromatic ring, meta", -0.2245,
                "ca");
    addAtomType(Elements::C, 4, "ca_p", "nbonds=3,ring(size=6,C(n=5),N),-C(-C(-N),n=2)", "Carbon in aromatic ring, para",
                0.1706, "ca");
    addAtomType(Elements::H, 5, "ha_o", "nbonds=1,-&2", "Hydrogen bound to carbon in aromatic ring, ortho", 0.1751, "ha");
    addAtomType(Elements::H, 6, "ha_m", "nbonds=1,-&3", "Hydrogen bound to carbon in aromatic ring, meta", 0.1760, "ha");
    addAtomType(Elements::H, 7, "ha_p", "nbonds=1,-&4", "Hydrogen bound to carbon in aromatic ring, para", 0.1293, "ha");
    addAtomType(Elements::C, 8, "ct_1", "nbonds=4,nh=2,-C,-&1", "Alkane Carbon of Tail, adjacent to N", -0.1745, "ct");
    addAtomType(Elements::C, 9, "ct_2", "nbonds=4,nh=2,-C(-N)", "Alkane Carbon of Tail, 2nd", 0.1349, "ct");
    addAtomType(Elements::C, 10, "ct_3", "nbonds=4,nh=2,-C(-C(-N))", "Alkane Carbon of Tail, 3rd", 0.0170, "ct");
    addAtomType(Elements::C, 11, "ct_4", "nbonds=4,nh=2,-C(-C(-C(-N)))", "Alkane Carbon of Tail, 4th", 0.1023, "ct");
    addAtomType(Elements::H, 12, "hc_1", "nbonds=1,-&8", "Hydrogen of Tail, next to N", 0.1070, "hc");
    addAtomType(Elements::H, 13, "hc_2", "nbonds=1,-&9", "Hydrogen of Tail, 2nd", -0.0208, "hc");
    addAtomType(Elements::H, 14, "hc_3", "nbonds=1,-&10", "Hydrogen of Tail, 3rd", 0.0097, "hc");
    addAtomType(Elements::H, 15, "hc_4", "nbonds=1,-&11", "Hydrogen of Tail, 4th", -0.0107, "hc");
    addAtomType(Elements::C, 16, "cm", "nbonds=4,nh=3,-&11", "Carbon of Tail End", -0.1671, "cm");
    addAtomType(Elements::H, 17, "hm", "nbonds=1,-&16", "Hydrogen of Tail End", 0.0480, "hm");

    // Bond Terms
    addBondTerm("ha", "ca", BondFunctions::Form::Harmonic, {3071., 1.080});
    addBondTerm("ca", "ca", BondFunctions::Form::Harmonic, {3925., 1.400});
    addBondTerm("nc", "ca", BondFunctions::Form::Harmonic, {4042., 1.339});
    addBondTerm("nc", "ct", BondFunctions::Form::Harmonic, {4042, 1.339});
    addBondTerm("ct", "ct", BondFunctions::Form::Harmonic, {2244.1, 1.529});
    addBondTerm("ct", "hc", BondFunctions::Form::Harmonic, {2847.0, 1.09});
    addBondTerm("ct", "cm", BondFunctions::Form::Harmonic, {2244.1, 1.529});
    addBondTerm("cm", "hm", BondFunctions::Form::Harmonic, {2847.0, 1.09});

    // Angle Terms
    addAngleTerm("ca", "ca", "ca", AngleFunctions::Form::Harmonic, {527.2, 120.0});
    addAngleTerm("ca", "ca", "nc", AngleFunctions::Form::Harmonic, {585.8, 124.0});
    addAngleTerm("ca", "nc", "ca", AngleFunctions::Form::Harmonic, {585.8, 117.0});
    addAngleTerm("ca", "ca", "ha", AngleFunctions::Form::Harmonic, {292.9, 120.0});
    addAngleTerm("nc", "ca", "ha", AngleFunctions::Form::Harmonic, {292.9, 116.0});
    addAngleTerm("ca", "nc", "ct", AngleFunctions::Form::Harmonic, {585.8, 121.5});
    addAngleTerm("nc", "ct", "ct", AngleFunctions::Form::Harmonic, {487.43, 112.7});
    addAngleTerm("hc", "ct", "nc", AngleFunctions::Form::Harmonic, {313.26, 110.7});
    addAngleTerm("hc", "ct", "hc", AngleFunctions::Form::Harmonic, {275.7, 107.8});
    addAngleTerm("hc", "ct", "ct", AngleFunctions::Form::Harmonic, {313.26, 110.7});
    addAngleTerm("hc", "ct", "cm", AngleFunctions::Form::Harmonic, {313.26, 110.7});
    addAngleTerm("hm", "cm", "ct", AngleFunctions::Form::Harmonic, {313.26, 110.7});
    addAngleTerm("hm", "cm", "hm", AngleFunctions::Form::Harmonic, {275.7, 107.8});
    addAngleTerm("ct", "ct", "ct", AngleFunctions::Form::Harmonic, {487.43, 112.7});
    addAngleTerm("ct", "ct", "cm", AngleFunctions::Form::Harmonic, {487.43, 112.7});

    // Torsion Terms
    addTorsionTerm("*", "ca", "ca", "*", TorsionFunctions::Form::Cosine, {15.178, 2.0, 180.0, 1});
    addTorsionTerm("*", "ca", "nc", "*", TorsionFunctions::Form::Cosine, {15.178, 2.0, 180.0, 1});
    addTorsionTerm("ca", "nc", "ct", "ct", TorsionFunctions::Form::CosN, {0.0, -0.4172, 0.0, -0.4759});
    addTorsionTerm("nc", "ct", "ct", "ct", TorsionFunctions::Form::CosN, {-2.9885, 0.0, 6.7221, 0.3547});
    addTorsionTerm("ct", "ct", "ct", "ct", TorsionFunctions::Form::CosN, {-0.4882, 0.2620, 5.3908, 0.6635, 0.3339});
    addTorsionTerm("ct", "ct", "ct", "cm", TorsionFunctions::Form::CosN, {0.0, 0.0, 6.0396, 0.5463});
    addTorsionTerm("ct", "ct", "cm", "hm", TorsionFunctions::Form::Cosine, {1.8773, 3.0, 0.0, 1});
    addTorsionTerm("hc", "ct", "ct", "hc", TorsionFunctions::Form::Cosine, {0.0000, 3.0, 0.0, 1});
    addTorsionTerm("hc", "ct", "cm", "hm", TorsionFunctions::Form::Cosine, {0.0000, 3.0, 0.0, 1});
    addTorsionTerm("hc", "ct", "ct", "ct", TorsionFunctions::Form::Cosine, {0.0000, 3.0, 0.0, 1});
    addTorsionTerm("hc", "ct", "ct", "cm", TorsionFunctions::Form::Cosine, {0.0000, 3.0, 0.0, 1});
    addTorsionTerm("hc", "ct", "ct", "nc", TorsionFunctions::Form::Cosine, {0.0000, 3.0, 0.0, 1});
    addTorsionTerm("hc", "ct", "nc", "ca", TorsionFunctions::Form::Cosine, {0.0000, 3.0, 0.0, 1});

    // Improper Terms
    addImproperTerm("ca", "ca", "ca", "ha", TorsionFunctions::Form::Cosine, {4.606, 2.0, 180.0, 1});
    addImproperTerm("ca", "nc", "ca", "ha", TorsionFunctions::Form::Cosine, {4.606, 2.0, 180.0, 1});
    addImproperTerm("nc", "ca", "ca", "ct", TorsionFunctions::Form::Cosine, {4.606, 2.0, 180.0, 1});

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_Ludwig_Py5::name() const { return "Ludwig/Py5"; }

// Return description for Forcefield
std::string_view Forcefield_Ludwig_Py5::description() const
{
    return "Implements of 1‐pentylpyridinium cation based on OPLS All Atom Forcefield for benzene and pyridine; W. L. "
           "Jorgensen, D. S. Maxwell, and J. Tirado-Rives, <i>Journal of the American "
           "Chemical "
           "Society</i>, <b>118</b>, 11225 (1996).";
}

// Return short-range interaction style for AtomTypes
Forcefield::ShortRangeType Forcefield_Ludwig_Py5::shortRangeType() const { return Forcefield::ShortRangeType::LennardJones; }
