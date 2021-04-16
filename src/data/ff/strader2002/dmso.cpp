// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/ff/strader2002/dmso.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/speciesatom.h"

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
    addAtomType(Elements::C, 1, "CD", "-S,-H(n=3)", "DMSO carbon", -0.148, {0.3265, 3.6349});
    addAtomType(Elements::S, 2, "SD", "nbonds=3,-O,-C(n=2)", "DMSO sulfur", 0.312, {1.4651, 3.5636});
    addAtomType(Elements::O, 3, "OD", "-&2", "DMSO oxygen", -0.556, {0.50232, 3.0291});
    addAtomType(Elements::H, 4, "HD", "-C", "DMSO hydrogen", 0.090, {0.10046, 2.3876});

    // Bond terms
    addBondTerm("CD", "SD", SpeciesBond::HarmonicForm, {2009.28, 1.80});
    addBondTerm("SD", "OD", SpeciesBond::HarmonicForm, {4520.88, 1.53});
    addBondTerm("CD", "HD", SpeciesBond::HarmonicForm, {2695.78, 1.11});

    // Angle terms
    addAngleTerm("CD", "SD", "CD", SpeciesAngle::HarmonicForm, {284.65, 95.0});
    addAngleTerm("CD", "SD", "OD", SpeciesAngle::HarmonicForm, {661.39, 106.75});
    addAngleTerm("HD", "CD", "SD", SpeciesAngle::HarmonicForm, {385.94, 111.30});
    addAngleTerm("HD", "CD", "HD", SpeciesAngle::HarmonicForm, {297.21, 108.40});

    // Dihedral terms
    addTorsionTerm("HD", "CD", "SD", "OD", SpeciesTorsion::CosineForm, {0.8372, 3.0, 0.0, 1});
    addTorsionTerm("HD", "CD", "SD", "CD", SpeciesTorsion::CosineForm, {0.8372, 3.0, 0.0, 1});

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
Forcefield::ShortRangeType Forcefield_Strader2002::shortRangeType() const { return Forcefield::LennardJonesType; }
