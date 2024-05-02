// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/ff/kulmala2010/kulmala2010.h"

/*
 * Implements forcefield found in "Enhancing effect of dimethylamine in sulfuric acid nucleation in the presence
 * of water – a computational study", V. Loukonen, T. Kurten, I. K. Ortega, H. Vehkamaki, A. A. H. Padua, K. Sellegri, and M.
 * Kulmala, Atmos. Chem. Phys., 10, 4961 (2010), doi:10.5194/acp-10-4961-2010
 *
 * Forcefield covers water (SPC/E), the hydronium ion, ammonia, the ammonium ion, sulfuric acid, the hydrogensulfate ion, and
 * the dimethylammonium ion. Based on OPLS-AA parameters, and augmented by QM-derived values.
 *
 * Notes:
 * Any inconsistencies between the forcefield as implemented here and the original work are the sole responsibility of TGAY.
 * All energy values are in kJ/mol.
 */

/*
 * Set Up
 */

// Set up / create all forcefield terms
bool Forcefield_Kulmala2010::setUp()
{
    // Atom types
    // -- Water (SPC/E)
    addAtomType(Elements::H, 1, "HW", "-O(nh=2)", "Water hydrogen", 0.4238, "epsilon=0.0 sigma=0.0");
    addAtomType(Elements::O, 2, "OW", "-H(n=2)", "Water oxygen", -0.8476, "epsilon=0.6502 sigma=3.166");
    // -- Hydronium Ion
    addAtomType(Elements::H, 3, "HW+", "-O(nh=3)", "Hydronium hydrogen", 0.4722, "epsilon=0.0 sigma=0.0");
    addAtomType(Elements::O, 4, "OW+", "-H(n=3)", "Hydronium oxygen", -0.4166, "epsilon=0.62 sigma=3.234");
    // -- Ammonia
    addAtomType(Elements::H, 5, "HN", "-N(nh=3)", "Ammonia hydrogen", 0.3, "epsilon=0.12552 sigma=2.5");
    addAtomType(Elements::N, 6, "NH", "-H(n=3)", "Ammonia nitrogen", -0.9, "epsilon=0.71128 sigma=3.25");
    // -- Ammonium Ion
    addAtomType(Elements::H, 7, "HN+", "-N(nh=4)", "Ammonium ion hydrogen", 0.44, "epsilon=0.12552 sigma=2.5");
    addAtomType(Elements::N, 8, "NH+", "-H(n=4)", "Ammonium ion nitrogen", -0.76, "epsilon=0.71128 sigma=3.25");
    // -- Sulphuric Acid
    addAtomType(Elements::S, 9, "SA", "-O(nh=1,n=2)", "Sulphuric acid sulfur", 1.1, "epsilon=1.046 sigma=3.55");
    addAtomType(Elements::O, 10, "OHA", "nh=1,-&9", "Sulphuric acid hydroxyl oxygen", -0.53, "epsilon=0.71176 sigma=3.0");
    addAtomType(Elements::O, 11, "OA", "nh=0,-&9", "Sulphuric acid oxygen", -0.46, "epsilon=0.71176 sigma=3.0");
    addAtomType(Elements::H, 12, "HSA", "-O(-&9)", "Sulphuric acid hydroxyl hydrogen", 0.44, "epsilon=0.0 sigma=0.0");
    // -- Hydrogen Sulfate Ion
    addAtomType(Elements::S, 13, "SA-", "-O(nh=1,n=1),-O(nh=0,n=3)", "Hydrogen sulphate sulfur", 1.27,
                "epsilon=1.046 sigma=3.55");
    addAtomType(Elements::O, 14, "OHA-", "nh=1,-&13", "Hydrogen sulphate hydroxyl oxygen", -0.64, "epsilon=0.71176 sigma=3.0");
    addAtomType(Elements::O, 15, "OA-", "nh=0,-&13", "Hydrogen sulphate oxygen", -0.67, "epsilon=0.71176 sigma=3.0");
    addAtomType(Elements::H, 16, "HSA-", "-O(-&13)", "Hydrogen sulphate hydroxyl hydrogen", 0.38, "epsilon=0.0 sigma=0.0");
    // -- Dimethylammonium Ion
    addAtomType(Elements::N, 17, "NDM", "nh=2,-C(nh=3,n=2)", "Dimethylammonium ion nitrogen", -0.12,
                "epsilon=0.71128 sigma=3.25");
    addAtomType(Elements::H, 18, "HDM", "-&17", "Dimethylammonium ion hydrogen", 0.31, "epsilon=0.12550 sigma=2.5");
    addAtomType(Elements::C, 19, "CDM", "nh=3,-&17", "Dimethylammonium ion methyl carbon", -0.2, "epsilon=0.27614 sigma=3.5");
    addAtomType(Elements::H, 20, "HCD", "-&19", "Dimethylammonium ion methyl hydrogen", 0.15, "epsilon=0.12550 sigma=2.5");

    // Intramolecular terms
    // -- Water (SPC/E)
    addBondTerm("HW", "OW", BondFunctions::Form::Harmonic, "k=4637.0 eq=1.0");
    addAngleTerm("HW", "OW", "HW", AngleFunctions::Form::Harmonic, "k=383.0 eq=109.4");
    // -- Hydronium Ion
    addBondTerm("HW+", "OW+", BondFunctions::Form::Harmonic, "k=4030.0 eq=0.969");
    addAngleTerm("HW+", "OW+", "HW+", AngleFunctions::Form::Harmonic, "k=451.4 eq=113.1");
    // -- Ammonia
    addBondTerm("HN", "NH", BondFunctions::Form::Harmonic, "k=3864.0 eq=1.003");
    addAngleTerm("HN", "NH", "HN", AngleFunctions::Form::Harmonic, "k=334.01 eq=107.18");
    // -- Ammonium Ion
    addBondTerm("HN+", "NH+", BondFunctions::Form::Harmonic, "k=3660.3 eq=1.013");
    addAngleTerm("HN+", "NH+", "HN+", AngleFunctions::Form::Harmonic, "k=518.26 eq=109.47");
    // -- Sulphuric Acid
    addBondTerm("SA", "OA", BondFunctions::Form::Harmonic, "k=6258.0 eq=1.411");
    addBondTerm("SA", "OHA", BondFunctions::Form::Harmonic, "k=3083.8 eq=1.571");
    addBondTerm("OHA*", "HSA*", BondFunctions::Form::Harmonic, "k=4561.1 eq=0.949");
    addAngleTerm("OA", "SA", "OA", AngleFunctions::Form::Harmonic, "k=224.37 eq=123.69");
    addAngleTerm("OHA", "SA", "OA", AngleFunctions::Form::Harmonic, "k=364.56 eq=107.32");
    addAngleTerm("OHA", "SA", "OHA", AngleFunctions::Form::Harmonic, "k=608.15 eq=101.79");
    addAngleTerm("HSA", "OHA", "SA", AngleFunctions::Form::Harmonic, "k=396.82 eq=110.5");
    addTorsionTerm("OA", "SA", "OHA", "HSA", TorsionFunctions::Form::Cos3, "k1=0.0 k2=0.0 k3=0.0");
    addTorsionTerm("OHA", "SA", "OHA", "HSA", TorsionFunctions::Form::Cos3, "k1=-15.8628 k2=-7.797 k3=2.3399");
    // -- Hydrogen Sulfate Ion
    addBondTerm("SA-", "OA-", BondFunctions::Form::Harmonic, "k=5239.9 eq=1.438");
    addBondTerm("SA-", "OHA-", BondFunctions::Form::Harmonic, "k=2328.3 eq=1.634");
    addAngleTerm("OA-", "SA-", "OA-", AngleFunctions::Form::Harmonic, "k=456.97 eq=114.76");
    addAngleTerm("OHA-", "SA-", "OA-", AngleFunctions::Form::Harmonic, "k=814.48 eq=103.45");
    addAngleTerm("HSA-", "OHA-", "SA-", AngleFunctions::Form::Harmonic, "k=398.12 eq=106.39");
    addTorsionTerm("OA-", "SA-", "OHA-", "HSA-", TorsionFunctions::Form::Cos3, "k1=0.0 k2=0.0 k3=0.0");
    // -- Dimethylammonium Ion
    addBondTerm("NDM", "CDM", BondFunctions::Form::Harmonic, "k=3071.0 eq=1.499");
    addBondTerm("NDM", "HDM", BondFunctions::Form::Harmonic, "k=3632.0 eq=1.01");
    addBondTerm("CDM", "HCD", BondFunctions::Form::Harmonic, "k=2845.12 eq=1.09"); // Missing from SI - taken from OPLS-AA
    addAngleTerm("CDM", "NDM", "CDM", AngleFunctions::Form::Harmonic, "k=418.4 eq=114.3");
    addAngleTerm("HDM", "NDM", "CDM", AngleFunctions::Form::Harmonic, "k=418.4 eq=109.2");
    addAngleTerm("HCD", "CDM", "NDM", AngleFunctions::Form::Harmonic, "k=209.2 eq=108.5");
    addAngleTerm("HDM", "NDM", "HDM", AngleFunctions::Form::Harmonic, "k=292.9 eq=105.5");
    addAngleTerm("HCD", "CDM", "HCD", AngleFunctions::Form::Harmonic,
                 "k=276.144 eq=107.8"); // Missing from SI - taken from OPLS-AA
    addTorsionTerm("HCD", "CDM", "NDM", "HDM", TorsionFunctions::Form::Cos3, "k1=0.0 k2=0.0 k3=0.0");
    addTorsionTerm("HCD", "CDM", "NDM", "CDM", TorsionFunctions::Form::Cos3, "k1=0.0 k2=0.0 k3=3.3765");

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_Kulmala2010::name() const { return "Kulmala2010"; }

// Return description for Forcefield
std::string_view Forcefield_Kulmala2010::description() const
{
    return "Implements forcefield found in 'Enhancing effect of dimethylamine in sulfuric acid nucleation in the presence of "
           "water – a computational study', V. Loukonen, T. Kurten, I. K. Ortega, H. Vehkamaki, A. A. H. Padua, K. Sellegri, "
           "and M. Kulmala, <i>Atmos. Chem. Phys.</i> <b>10</b>, 4961 (2010), "
           "<a href='http://dx.doi.org/10.5194/acp-10-4961-2010'></a>.</br>Covers water (SPC/E), the hydronium ion, ammonia, "
           "the ammonium ion, sulfuric acid, the hydrogensulfate ion, and the dimethylammonium ion. Based on OPLS-AA "
           "parameters, with specific QM-derived parameters (see Supplementary Information, Tables 1-6).";
}

// Return short-range interaction style for AtomTypes
ShortRangeFunctions::Form Forcefield_Kulmala2010::shortRangeForm() const { return ShortRangeFunctions::Form::LennardJones; }
