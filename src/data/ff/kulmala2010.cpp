/*
    *** Kulmala 2010 Forcefield
    *** src/data/ff/kulmala2010.cpp
    Copyright T. Youngs 2019-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "data/ff/kulmala2010.h"

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
    addAtomType(ELEMENT_H, 1, "HW", "-O(nh=2)", "Water hydrogen", 0.4238, 0.0, 0.0);
    addAtomType(ELEMENT_O, 2, "OW", "-H(n=2)", "Water oxygen", -0.8476, 0.6502, 3.166);
    // -- Hydronium Ion
    addAtomType(ELEMENT_H, 3, "HW+", "-O(nh=3)", "Hydronium hydrogen", 0.4722, 0.0, 0.0);
    addAtomType(ELEMENT_O, 4, "OW+", "-H(n=3)", "Hydronium oxygen", -0.4166, 0.62, 3.234);
    // -- Ammonia
    addAtomType(ELEMENT_H, 5, "HN", "-N(nh=3)", "Ammonia hydrogen", 0.3, 0.12552, 2.5);
    addAtomType(ELEMENT_N, 6, "NH", "-H(n=3)", "Ammonia nitrogen", -0.9, 0.71128, 3.25);
    // -- Ammonium Ion
    addAtomType(ELEMENT_H, 7, "HN+", "-N(nh=4)", "Ammonium ion hydrogen", 0.44, 0.12552, 2.5);
    addAtomType(ELEMENT_N, 8, "NH+", "-H(n=4)", "Ammonium ion nitrogen", -0.76, 0.71128, 3.25);
    // -- Sulphuric Acid
    addAtomType(ELEMENT_S, 9, "SA", "-O(nh=1,n=2)", "Sulphuric acid sulfur", 1.1, 1.046, 3.55);
    addAtomType(ELEMENT_O, 10, "OHA", "nh=1,-&9", "Sulphuric acid hydroxyl oxygen", -0.53, 0.71176, 3.0);
    addAtomType(ELEMENT_O, 11, "OA", "nh=0,-&9", "Sulphuric acid oxygen", -0.46, 0.71176, 3.0);
    addAtomType(ELEMENT_H, 12, "HSA", "-O(-&9)", "Sulphuric acid hydroxyl hydrogen", 0.44, 0.0, 0.0);
    // -- Hydrogen Sulfate Ion
    addAtomType(ELEMENT_S, 13, "SA-", "-O(nh=1,n=1),-O(nh=0,n=3)", "Hydrogen sulphate sulfur", 1.27, 1.046, 3.55);
    addAtomType(ELEMENT_O, 14, "OHA-", "nh=1,-&13", "Hydrogen sulphate hydroxyl oxygen", -0.64, 0.71176, 3.0);
    addAtomType(ELEMENT_O, 15, "OA-", "nh=0,-&13", "Hydrogen sulphate oxygen", -0.67, 0.71176, 3.0);
    addAtomType(ELEMENT_H, 16, "HSA-", "-O(-&13)", "Hydrogen sulphate hydroxyl hydrogen", 0.38, 0.0, 0.0);
    // -- Dimethylammonium Ion
    addAtomType(ELEMENT_N, 17, "NDM", "nh=2,-C(nh=3,n=2)", "Dimethylammonium ion nitrogen", -0.12, 0.71128, 3.25);
    addAtomType(ELEMENT_H, 18, "HDM", "-&17", "Dimethylammonium ion hydrogen", 0.31, 0.12550, 2.5);
    addAtomType(ELEMENT_C, 19, "CDM", "nh=3,-&17", "Dimethylammonium ion methyl carbon", -0.2, 0.27614, 3.5);
    addAtomType(ELEMENT_H, 20, "HCD", "-&19", "Dimethylammonium ion methyl hydrogen", 0.15, 0.12550, 2.5);

    // Intramolecular terms
    // -- Water (SPC/E)
    addBondTerm("HW", "OW", SpeciesBond::HarmonicForm, {4637.0, 1.0});
    addAngleTerm("HW", "OW", "HW", SpeciesAngle::HarmonicForm, {383.0, 109.4});
    // -- Hydronium Ion
    addBondTerm("HW+", "OW+", SpeciesBond::HarmonicForm, {4030.0, 0.969});
    addAngleTerm("HW+", "OW+", "HW+", SpeciesAngle::HarmonicForm, {451.4, 113.1});
    // -- Ammonia
    addBondTerm("HN", "NH", SpeciesBond::HarmonicForm, {3864.0, 1.003});
    addAngleTerm("HN", "NH", "HN", SpeciesAngle::HarmonicForm, {334.01, 107.18});
    // -- Ammonium Ion
    addBondTerm("HN+", "NH+", SpeciesBond::HarmonicForm, {3660.3, 1.013});
    addAngleTerm("HN+", "NH+", "HN+", SpeciesAngle::HarmonicForm, {518.26, 109.47});
    // -- Sulphuric Acid
    addBondTerm("SA", "OA", SpeciesBond::HarmonicForm, {6258.0, 1.411});
    addBondTerm("SA", "OHA", SpeciesBond::HarmonicForm, {3083.8, 1.571});
    addBondTerm("OHA*", "HSA*", SpeciesBond::HarmonicForm, {4561.1, 0.949});
    addAngleTerm("OA", "SA", "OA", SpeciesAngle::HarmonicForm, {224.37, 123.69});
    addAngleTerm("OHA", "SA", "OA", SpeciesAngle::HarmonicForm, {364.56, 107.32});
    addAngleTerm("OHA", "SA", "OHA", SpeciesAngle::HarmonicForm, {608.15, 101.79});
    addAngleTerm("HSA", "OHA", "SA", SpeciesAngle::HarmonicForm, {396.82, 110.5});
    addTorsionTerm("OA", "SA", "OHA", "HSA", SpeciesTorsion::Cos3Form, {0.0, 0.0, 0.0});
    addTorsionTerm("OHA", "SA", "OHA", "HSA", SpeciesTorsion::Cos3Form, {-15.8628, -7.797, 2.3399});
    // -- Hydrogen Sulfate Ion
    addBondTerm("SA-", "OA-", SpeciesBond::HarmonicForm, {5239.9, 1.438});
    addBondTerm("SA-", "OHA-", SpeciesBond::HarmonicForm, {2328.3, 1.634});
    addAngleTerm("OA-", "SA-", "OA-", SpeciesAngle::HarmonicForm, {456.97, 114.76});
    addAngleTerm("OHA-", "SA-", "OA-", SpeciesAngle::HarmonicForm, {814.48, 103.45});
    addAngleTerm("HSA-", "OHA-", "SA-", SpeciesAngle::HarmonicForm, {398.12, 106.39});
    addTorsionTerm("OA-", "SA-", "OHA-", "HSA-", SpeciesTorsion::Cos3Form, {0.0, 0.0, 0.0});
    // -- Dimethylammonium Ion
    addBondTerm("NDM", "CDM", SpeciesBond::HarmonicForm, {3071.0, 1.499});
    addBondTerm("NDM", "HDM", SpeciesBond::HarmonicForm, {3632.0, 1.01});
    addBondTerm("CDM", "HCD", SpeciesBond::HarmonicForm, {2845.12, 1.09}); // Missing from SI - taken from OPLS-AA
    addAngleTerm("CDM", "NDM", "CDM", SpeciesAngle::HarmonicForm, {418.4, 114.3});
    addAngleTerm("HDM", "NDM", "CDM", SpeciesAngle::HarmonicForm, {418.4, 109.2});
    addAngleTerm("HCD", "CDM", "NDM", SpeciesAngle::HarmonicForm, {209.2, 108.5});
    addAngleTerm("HDM", "NDM", "HDM", SpeciesAngle::HarmonicForm, {292.9, 105.5});
    addAngleTerm("HCD", "CDM", "HCD", SpeciesAngle::HarmonicForm, {276.144, 107.8}); // Missing from SI - taken from OPLS-AA
    addTorsionTerm("HCD", "CDM", "NDM", "HDM", SpeciesTorsion::Cos3Form, {0.0, 0.0, 0.0});
    addTorsionTerm("HCD", "CDM", "NDM", "CDM", SpeciesTorsion::Cos3Form, {0.0, 0.0, 3.3765});

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
Forcefield::ShortRangeType Forcefield_Kulmala2010::shortRangeType() const { return Forcefield::LennardJonesType; }
