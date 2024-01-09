// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

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
    addParameters("nc", "epsilon=0.711302 sigma=3.250");
    addParameters("ca", "epsilon=0.292919 sigma=3.550");
    addParameters("ha", "epsilon=0.125548 sigma=2.420");
    addParameters("ct", "epsilon=0.276040 sigma=3.50");
    addParameters("hc", "epsilon=0.125548 sigma=2.50");
    addParameters("oh", "epsilon=0.711718 sigma=3.12");
    addParameters("ho", "epsilon=0.000000 sigma=0.0");

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
    addAtomType(Elements::C, 9, "ct_2", "nbonds=4,nh=2,-&8,-C", "Alkane Carbon of Tail, two away from N", 0.0160, "ct");
    addAtomType(Elements::C, 10, "ct_3", "nbonds=4,nh=2,-C(-H(n=2),-O)", "Alkane Carbon of Tail, adjacent to O", 0.0780, "ct");
    addAtomType(Elements::C, 11, "ct_4", "nbonds=4,nh=2,-C(-H(n=2),-C),-O", "Alkane Carbon of Tail, two away from O", 0.2911,
                "ct");
    addAtomType(Elements::H, 12, "hc_1", "nbonds=1,-&8", "Hydrogen of Tail, adjacent to N", 0.1015, "hc");
    addAtomType(Elements::H, 13, "hc_2", "nbonds=1,-&9", "Hydrogen of Tail, two away from N", 0.0043, "hc");
    addAtomType(Elements::H, 14, "hc_3", "nbonds=1,-&10", "Hydrogen of Tail, two away from O", 0.0316, "hc");
    addAtomType(Elements::H, 15, "hc_4", "nbonds=1,-&11", "Hydrogen of Tail, adjacent to O", -0.0205, "hc");
    addAtomType(Elements::O, 16, "oh", "nbonds=2,-H,-&11", "Oxygen of Hydroxyl Group", -0.6916, "oh");
    addAtomType(Elements::H, 17, "ho", "nbonds=1,-&16", "Hydrogen of Hydroxyl Group", 0.4370, "ho");

    // Bond Terms
    addBondTerm("ha", "ca", BondFunctions::Form::Harmonic, "k=3071. eq=1.080");
    addBondTerm("ca", "ca", BondFunctions::Form::Harmonic, "k=3925. eq=1.400");
    addBondTerm("nc", "ca", BondFunctions::Form::Harmonic, "k=4042. eq=1.339");
    addBondTerm("nc", "ct", BondFunctions::Form::Harmonic, "k=4042 eq=1.339");
    addBondTerm("ct", "ct", BondFunctions::Form::Harmonic, "k=2244.1 eq=1.529");
    addBondTerm("ct", "hc", BondFunctions::Form::Harmonic, "k=2847.0 eq=1.09");
    addBondTerm("ct", "oh", BondFunctions::Form::Harmonic, "k=2679.6 eq=1.41");
    addBondTerm("ho", "oh", BondFunctions::Form::Harmonic, "k=4630.6 eq=0.945");

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
    addAngleTerm("ct", "ct", "ct", AngleFunctions::Form::Harmonic, "k=487.43 eq=112.7");
    addAngleTerm("ho", "oh", "ct", AngleFunctions::Form::Harmonic, "k=460.55 eq=108.5");
    addAngleTerm("ct", "ct", "oh", AngleFunctions::Form::Harmonic, "k=418.68 eq=109.5");
    addAngleTerm("hc", "ct", "oh", AngleFunctions::Form::Harmonic, "k=293.08 eq=109.5");

    // Torsion Terms
    addTorsionTerm("*", "ca", "ca", "*", TorsionFunctions::Form::Cosine, "k=15.178 n=2.0 eq=180.0 s=1");
    addTorsionTerm("*", "ca", "nc", "*", TorsionFunctions::Form::Cosine, "k=15.178 n=2.0 eq=180.0 s=1");
    addTorsionTerm("ca", "nc", "ct", "ct", TorsionFunctions::Form::CosN, "k1=0.0 k2=-0.3579 k3=0.0 k4=-0.4037");
    addTorsionTerm("nc", "ct", "ct", "ct", TorsionFunctions::Form::CosN, "k1=-3.7014 k2=0.0 k3=6.9923");
    addTorsionTerm("ct", "ct", "ct", "ct", TorsionFunctions::Form::CosN,
                   "k1=-0.2825 k2=0.6065 k3=4.6858 k4=0.7018 k5=0.4468 k6=0.4564");
    addTorsionTerm("ct", "ct", "ct", "oh", TorsionFunctions::Form::CosN, "k1=-2.3748 k2=0.0 k3=6.8089 k4=0.9531");
    addTorsionTerm("ct", "ct", "oh", "ho", TorsionFunctions::Form::CosN, "k1=-3.5552 k2=0.5886 k3=2.5272 k4=0.1504");
    addTorsionTerm("hc", "ct", "ct", "hc", TorsionFunctions::Form::Cosine, "k=0.0000 n=3.0 eq=0.0 s=1");
    addTorsionTerm("hc", "ct", "ct", "ct", TorsionFunctions::Form::Cosine, "k=0.0000 n=3.0 eq=0.0 s=1");
    addTorsionTerm("hc", "ct", "nc", "ca", TorsionFunctions::Form::Cosine, "k=0.0000 n=3.0 eq=0.0 s=1");
    addTorsionTerm("hc", "ct", "ct", "nc", TorsionFunctions::Form::Cosine, "k=0.0000 n=3.0 eq=0.0 s=1");
    addTorsionTerm("hc", "ct", "ct", "oh", TorsionFunctions::Form::Cosine, "k=0.0000 n=3.0 eq=0.0 s=1");
    addTorsionTerm("hc", "ct", "oh", "ho", TorsionFunctions::Form::Cosine, "k=0.0000 n=3.0 eq=0.0 s=1");

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
std::string_view Forcefield_Ludwig_Py4OH::name() const { return "Ludwig/Py4OH"; }

// Return description for Forcefield
std::string_view Forcefield_Ludwig_Py4OH::description() const
{
    return "Implements of 1‐(4‐hydroxybutyl)pyridinium cation based on OPLS All Atom Forcefield for benzene and pyridine; "
           "W. L. Jorgensen,	D. S. Maxwell, and J. Tirado-Rives, <i>Journal of the "
           "American Chemical Society</i>, <b>118</b>, 11225 (1996).";
}

// Return short-range interaction style for AtomTypes
ShortRangeFunctions::Form Forcefield_Ludwig_Py4OH::shortRangeForm() const { return ShortRangeFunctions::Form::LennardJones; }
