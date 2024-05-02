// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/ff/zhao2010/zhao2010.h"
#include "classes/species.h"

/*
 * Implements forcefield found in "A force field for dynamic Cu-BTC metal-organic framework", L. Zhao & Q. Yang, Q. Ma, C.
 * Zhong, J. Mi, and D. Liu, J. Mol. Model. *17*, 227 (2011), 10.1007/s00894-010-0720-x.
 *
 * Notes:
 * Any inconsistencies between the forcefield as implemented here and the original work are the sole responsibility of TGAY.
 * All energy values are convert from kcal/mol to kJ/mol.
 *
 * In order to cater for the differentiation between O(1) and O(2) atoms in angles, we override the term assignment and do this
 * by hand since there is no way to determine the correct parameters by atom type / connectivity alone.
 *
 * Torsion O-Cu-O-C1 is present in the model but not in Table 4. In the original reference (page 230, RHS) the authors state
 * that "In addition, two torsion terms Cu-O-C(1)-C(2) and Cu- O-C(1)-O were also included for interactions between the cluster
 * and organic part..." which suggests that the O-Cu-O-C1 term is explicitly not set.
 *
 * Improper terms are given with
 * the central atom C at position 2 (i-C-j-k) and a reordered here to put the central atom at position 1 (C-i-j-k). Improper
 * H-[C3]-C2-C2 is defined but this is another invalid arrangement, and is assumed to mean H-[C3]-C2-C3.
 */

/*
 * Set Up
 */

// Set up / create all forcefield terms
bool Forcefield_Zhao2010::setUp()
{
    // Atom types
    addAtomType(Elements::Cu, 1, "Cu", "-O(n=4)", "Copper", 1.098, "epsilon=0.02092 sigma=3.144");
    addAtomType(Elements::O, 2, "O", "-&1,-C(n=1)", "Oxygen in BTC linker", -0.665, "epsilon=0.401664 sigma=3.033");
    addAtomType(Elements::C, 3, "C1", "-&2(n=2),-C,nbonds=3", "Carboxylate carbon in BTC linker", 0.778,
                "epsilon=0.39748 sigma=3.473");
    addAtomType(Elements::C, 4, "C2", "ring(size=6),-&3,-C(-H,n=2)", "Carbon in benzene ring attached to carboxylate carbon",
                -0.092, "epsilon=0.39748 sigma=3.473");
    addAtomType(Elements::C, 5, "C3", "ring(size=6),-H", "Carbon in benzene ring", -0.014, "epsilon=0.39748 sigma=3.473");
    addAtomType(Elements::H, 6, "H", "-&5", "BTC linker hydrogen", 0.109, "epsilon=0.06276 sigma=2.846");

    // Intramolecular terms
    addBondTerm("Cu", "O", BondFunctions::Form::Morse, "D=358.8575 alpha=2.85 eq=1.969");
    addBondTerm("O", "C1", BondFunctions::Form::Morse, "D=564.84 alpha=2.0 eq=1.260");
    addBondTerm("C1", "C2", BondFunctions::Form::Morse, "D=367.5226 alpha=2.0 eq=1.456");
    addBondTerm("C2", "C3", BondFunctions::Form::Morse, "D=502.08 alpha=2.0 eq=1.355");
    addBondTerm("C3", "H", BondFunctions::Form::Morse, "D=485.344 alpha=1.77 eq=0.931");

    // -- O-Cu-O angles handled separately in our local assignAngleTermParameters()
    addAngleTerm("O(1)", "Cu", "O(1)", AngleFunctions::Form::Harmonic, "k=419.7389 eq=170.2");
    addAngleTerm("O(2)", "Cu", "O(2)", AngleFunctions::Form::Harmonic, "k=419.7389 eq=170.2");
    addAngleTerm("O(1)", "Cu", "O(2)", AngleFunctions::Form::Harmonic, "k=100.416 eq=90.0");
    addAngleTerm("Cu", "O", "C1", AngleFunctions::Form::Harmonic, "k=338.6530 eq=127.5");
    addAngleTerm("O", "C1", "O", AngleFunctions::Form::Harmonic, "k=1213.36 eq=128.5");
    addAngleTerm("O", "C1", "C2", AngleFunctions::Form::Harmonic, "k=456.0142 eq=116.2");
    addAngleTerm("C1", "C2", "C3", AngleFunctions::Form::Harmonic, "k=290.2022 eq=119.9");
    addAngleTerm("C3", "C2", "C3", AngleFunctions::Form::Harmonic, "k=753.12 eq=120.1");
    addAngleTerm("C2", "C3", "C2", AngleFunctions::Form::Harmonic, "k=753.12 eq=119.9");
    addAngleTerm("C2", "C3", "H", AngleFunctions::Form::Harmonic, "k=309.616 eq=120.0");

    addTorsionTerm("Cu", "O", "C1", "C2", TorsionFunctions::Form::Cosine, "k=12.552 n=2 eq=0.0 s=-1.0");
    addTorsionTerm("Cu", "O", "C1", "O", TorsionFunctions::Form::Cosine, "k=12.552 n=2 eq=0.0 s=-1.0");
    addTorsionTerm("C2", "C3", "C2", "C3", TorsionFunctions::Form::Cosine, "k=12.552 n=2 eq=0.0 s=-1.0");
    addTorsionTerm("C1", "C2", "C3", "C2", TorsionFunctions::Form::Cosine, "k=12.552 n=2 eq=0.0 s=-1.0");
    addTorsionTerm("C1", "C2", "C3", "H", TorsionFunctions::Form::Cosine, "k=12.552 n=2 eq=0.0 s=-1.0");
    addTorsionTerm("C3", "C2", "C3", "H", TorsionFunctions::Form::Cosine, "k=12.552 n=2 eq=0.0 s=-1.0");
    addTorsionTerm("O", "C1", "C2", "C3", TorsionFunctions::Form::Cosine, "k=10.46 n=2 eq=0.0 s=-1.0");
    // -- Missing in original
    addTorsionTerm("H", "C2", "C3", "C2", TorsionFunctions::Form::Cosine, "k=12.552 n=2 eq=0.0 s=-1.0");
    // -- Required in model, but assumed to be explicitly not applied (see notes above)
    addTorsionTerm("O", "Cu", "O", "C1", TorsionFunctions::Form::None, {});

    addImproperTerm("C2", "C1", "C3", "C3", TorsionFunctions::Form::Cosine, "k=41.84 n=2 eq=0.0 s=-1.0");
    addImproperTerm("C1", "C2", "O", "O", TorsionFunctions::Form::Cosine, "k=41.84 n=2 eq=0.0 s=-1.0");
    addImproperTerm("C3", "H", "C2", "C2", TorsionFunctions::Form::Cosine, "k=1.5481 n=2 eq=0.0 s=-1.0");

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_Zhao2010::name() const { return "Zhao2010"; }

// Return description for Forcefield
std::string_view Forcefield_Zhao2010::description() const
{
    return "Implements Cu-BTC forcefield found in 'A force field for dynamic Cu-BTC metal-organic framework', L. Zhao & Q. "
           "Yang, Q. Ma, C. Zhong, J. Mi, and D. Liu, <i>J. Mol. Model.</i> <b>17</b>, 227 (2011), <a "
           "href='https://dx.doi.org/10.1007/s00894-010-0720-x'></a>.";
}

// Return short-range interaction style for AtomTypes
ShortRangeFunctions::Form Forcefield_Zhao2010::shortRangeForm() const { return ShortRangeFunctions::Form::LennardJones; }

/*
 * Term Assignment
 */

// Assign / generate angle term parameters
bool Forcefield_Zhao2010::assignAngleTermParameters(const Species *parent, SpeciesAngle &angle, bool determineTypes) const
{
    // We need an override on the angle term assignment so that we can deal with the fact that the O-Cu-O angles are described
    // as harmonic angles and we have a mix of ~180 and ~90 angles.

    // Is this an O-Cu-O angle?
    if (angle.i()->Z() == Elements::O && angle.j()->Z() == Elements::Cu && angle.k()->Z() == Elements::O)
    {
        // Determine the geometry
        auto theta = parent->box()->angleInDegrees(angle.i()->r(), angle.j()->r(), angle.k()->r());
        if (theta > 135.0)
            angle.setInteractionFormAndParameters(AngleFunctions::Form::Harmonic, std::vector<double>{419.7389, 170.2});
        else
            angle.setInteractionFormAndParameters(AngleFunctions::Form::Harmonic, std::vector<double>{100.416, 90.0});

        return true;
    }
    else
        return Forcefield::assignAngleTermParameters(parent, angle, determineTypes);
}
