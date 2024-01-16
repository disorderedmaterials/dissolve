// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/ff/strader2002/dmso.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/speciesAtom.h"

/*
 * Implements "A Flexible All-Atom Model of Dimethyl Sulfoxide"
 * Matthew L. Strader and Scott E. Feller
 * J. Phys. Chem A 106 1074-1080 (2002)
 * http://dx.doi.org/10.1021/jp013658n
 *
 * Notes:
 * Any inconsistencies between the forcefield as implemented here and the original work are the sole responsibility of JB.
 * All energy values are in kJ/mol.
 */

/*
 * Set Up
 */

// Set up / create all forcefield terms
bool Forcefield_Strader2002::setUp()
{
    // Atom types
    addAtomType(Elements::C, 1, "CD", "-S,-H(n=3)", "DMSO carbon", -0.148, "epsilon=0.3265 sigma=3.6349");
    addAtomType(Elements::S, 2, "SD", "nbonds=3,-O,-C(n=2)", "DMSO sulfur", 0.312, "epsilon=1.4651 sigma=3.5636");
    addAtomType(Elements::O, 3, "OD", "-&2", "DMSO oxygen", -0.556, "epsilon=0.50232 sigma=3.0291");
    addAtomType(Elements::H, 4, "HD", "-C", "DMSO hydrogen", 0.090, "epsilon=0.10046 sigma=2.3876");

    // Bond terms
    addBondTerm("CD", "SD", BondFunctions::Form::Harmonic, "k=2009.28 eq=1.80");
    addBondTerm("SD", "OD", BondFunctions::Form::Harmonic, "k=4520.88 eq=1.53");
    addBondTerm("CD", "HD", BondFunctions::Form::Harmonic, "k=2695.78 eq=1.11");

    // Angle terms
    addAngleTerm("CD", "SD", "CD", AngleFunctions::Form::Harmonic, "k=284.65 eq=95.0");
    addAngleTerm("CD", "SD", "OD", AngleFunctions::Form::Harmonic, "k=661.39 eq=106.75");
    addAngleTerm("HD", "CD", "SD", AngleFunctions::Form::Harmonic, "k=385.94 eq=111.30");
    addAngleTerm("HD", "CD", "HD", AngleFunctions::Form::Harmonic, "k=297.21 eq=108.40");

    // Dihedral terms
    addTorsionTerm("HD", "CD", "SD", "OD", TorsionFunctions::Form::Cosine, "k=0.8372 n=3.0 eq=0.0 s=1");
    addTorsionTerm("HD", "CD", "SD", "CD", TorsionFunctions::Form::Cosine, "k=0.8372 n=3.0 eq=0.0 s=1");

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_Strader2002::name() const { return "Strader2002"; }

// Return description for Forcefield
std::string_view Forcefield_Strader2002::description() const
{
    return "Implements Matthew L. Strader and Scott E. Feller, 'A Flexible All-Atom Model of Dimethyl Sulfoxide for Molecular "
           "Dynamics Simulations'"
           ", <i>J. Phys. Chem A</i> "
           "<b>106</b> "
           "1074-1080 (2002), http://dx.doi.org/10.1021/jp013658n";
}

// Return short-range interaction style for AtomTypes
ShortRangeFunctions::Form Forcefield_Strader2002::shortRangeForm() const { return ShortRangeFunctions::Form::LennardJones; }
