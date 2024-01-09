// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
    addParameters("nc", "epsilon=0.711302 sigma=3.250");
    addParameters("ca", "epsilon=0.292919 sigma=3.550");
    addParameters("ha", "epsilon=0.125548 sigma=2.420");
    addParameters("ct", "epsilon=0.276040 sigma=3.50");
    addParameters("hc", "epsilon=0.125548 sigma=2.50");
    addParameters("cm", "epsilon=0.276040 sigma=3.50");
    addParameters("hm", "epsilon=0.125548 sigma=2.50");

    // Atom Types
    addAtomType(Elements::N, 1, "nc", "nbonds=3,ring(size=6,C(n=5),N)", "Nitrogen in pyridine ring", 0.0749, "nc");
    addAtomType(Elements::C, 2, "ca_o", "nbonds=3,-N,-C,-H,ring(size=6,C(n=5),N)", "Carbon in aromatic ring, ortho", 0.0725,
                "ca");
    addAtomType(Elements::C, 3, "ca_m", "nbonds=3,-C,-H,-C,ring(size=6,C(n=5),N)", "Carbon in aromatic ring, meta", -0.2245,
                "ca");
    addAtomType(Elements::C, 4, "ca_p", "nbonds=3,-C(-C(-N),n=2),ring(size=6,C(n=5),N)", "Carbon in aromatic ring, para",
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
    addBondTerm("ha", "ca", BondFunctions::Form::Harmonic, "k=3071. eq=1.080");
    addBondTerm("ca", "ca", BondFunctions::Form::Harmonic, "k=3925. eq=1.400");
    addBondTerm("nc", "ca", BondFunctions::Form::Harmonic, "k=4042. eq=1.339");
    addBondTerm("nc", "ct", BondFunctions::Form::Harmonic, "k=4042 eq=1.339");
    addBondTerm("ct", "ct", BondFunctions::Form::Harmonic, "k=2244.1 eq=1.529");
    addBondTerm("ct", "hc", BondFunctions::Form::Harmonic, "k=2847.0 eq=1.09");
    addBondTerm("ct", "cm", BondFunctions::Form::Harmonic, "k=2244.1 eq=1.529");
    addBondTerm("cm", "hm", BondFunctions::Form::Harmonic, "k=2847.0 eq=1.09");

    // Angle Terms
    addAngleTerm("ca", "ca", "ca", AngleFunctions::Form::Harmonic, "k=527.2 eq=120.0");
    addAngleTerm("ca", "ca", "nc", AngleFunctions::Form::Harmonic, "k=585.8 eq=124.0");
    addAngleTerm("ca", "nc", "ca", AngleFunctions::Form::Harmonic, "k=585.8 eq=117.0");
    addAngleTerm("ca", "ca", "ha", AngleFunctions::Form::Harmonic, "k=292.9 eq=120.0");
    addAngleTerm("nc", "ca", "ha", AngleFunctions::Form::Harmonic, "k=292.9 eq=116.0");
    addAngleTerm("ca", "nc", "ct", AngleFunctions::Form::Harmonic, "k=585.8 eq=121.5");
    addAngleTerm("nc", "ct", "ct", AngleFunctions::Form::Harmonic, "k=487.43 eq=112.7");
    addAngleTerm("hc", "ct", "nc", AngleFunctions::Form::Harmonic, "k=313.26 eq=110.7");
    addAngleTerm("hc", "ct", "hc", AngleFunctions::Form::Harmonic, "k=275.7 eq=107.8");
    addAngleTerm("hc", "ct", "ct", AngleFunctions::Form::Harmonic, "k=313.26 eq=110.7");
    addAngleTerm("hc", "ct", "cm", AngleFunctions::Form::Harmonic, "k=313.26 eq=110.7");
    addAngleTerm("hm", "cm", "ct", AngleFunctions::Form::Harmonic, "k=313.26 eq=110.7");
    addAngleTerm("hm", "cm", "hm", AngleFunctions::Form::Harmonic, "k=275.7 eq=107.8");
    addAngleTerm("ct", "ct", "ct", AngleFunctions::Form::Harmonic, "k=487.43 eq=112.7");
    addAngleTerm("ct", "ct", "cm", AngleFunctions::Form::Harmonic, "k=487.43 eq=112.7");

    // Torsion Terms
    addTorsionTerm("*", "ca", "ca", "*", TorsionFunctions::Form::Cosine, "k=15.178 n=2.0 eq=180.0 s=1");
    addTorsionTerm("*", "ca", "nc", "*", TorsionFunctions::Form::Cosine, "k=15.178 n=2.0 eq=180.0 s=1");
    addTorsionTerm("ca", "nc", "ct", "ct", TorsionFunctions::Form::CosN, "k1=0.0 k2=-0.4172 k3=0.0 k4=-0.4759");
    addTorsionTerm("nc", "ct", "ct", "ct", TorsionFunctions::Form::CosN, "k1=-2.9885 k2=0.0 k3=6.7221 k4=0.3547");
    addTorsionTerm("ct", "ct", "ct", "ct", TorsionFunctions::Form::CosN, "k1=-0.4882 k2=0.2620 k3=5.3908 k4=0.6635 k5=0.3339");
    addTorsionTerm("ct", "ct", "ct", "cm", TorsionFunctions::Form::CosN, "k1=0.0 k2=0.0 k3=6.0396 k4=0.5463");
    addTorsionTerm("ct", "ct", "cm", "hm", TorsionFunctions::Form::Cosine, "k=1.8773 n=3.0 eq=0.0 s=1");
    addTorsionTerm("hc", "ct", "ct", "hc", TorsionFunctions::Form::Cosine, "k=0.0000 n=3.0 eq=0.0 s=1");
    addTorsionTerm("hc", "ct", "cm", "hm", TorsionFunctions::Form::Cosine, "k=0.0000 n=3.0 eq=0.0 s=1");
    addTorsionTerm("hc", "ct", "ct", "ct", TorsionFunctions::Form::Cosine, "k=0.0000 n=3.0 eq=0.0 s=1");
    addTorsionTerm("hc", "ct", "ct", "cm", TorsionFunctions::Form::Cosine, "k=0.0000 n=3.0 eq=0.0 s=1");
    addTorsionTerm("hc", "ct", "ct", "nc", TorsionFunctions::Form::Cosine, "k=0.0000 n=3.0 eq=0.0 s=1");
    addTorsionTerm("hc", "ct", "nc", "ca", TorsionFunctions::Form::Cosine, "k=0.0000 n=3.0 eq=0.0 s=1");

    // Improper Terms
    addImproperTerm("ca", "ca", "ca", "ha", TorsionFunctions::Form::Cosine, "k=4.606 n=2.0 eq=180.0 s=1");
    addImproperTerm("ca", "nc", "ca", "ha", TorsionFunctions::Form::Cosine, "k=4.606 n=2.0 eq=180.0 s=1");
    addImproperTerm("nc", "ca", "ca", "ct", TorsionFunctions::Form::Cosine, "k=4.606 n=2.0 eq=180.0 s=1");

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
ShortRangeFunctions::Form Forcefield_Ludwig_Py5::shortRangeForm() const { return ShortRangeFunctions::Form::LennardJones; }
