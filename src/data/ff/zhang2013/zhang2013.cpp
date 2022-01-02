// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "data/ff/zhang2013/zhang2013.h"

/*
 * Implements forcefield found in "Sorption-Induced Structural Transition of Zeolitic Imidazolate Framework-8: A Hybrid
 * Molecular Simulation Study", L. Zhang, Z. Hu and J. Jiang, J. Am. Chem. Soc. *135*, 3722 (2013), 10.1021/ja401129h
 *
 * Forcefield covers ZIF-8. Sorbate parameters published in the original paper are not implemented here.
 *
 * Notes:
 * Any inconsistencies between the forcefield as implemented here and the original work are the sole responsibility of TGAY.
 * All energy values are in kJ/mol.
 */

/*
 * Set Up
 */

// Set up / create all forcefield terms
bool Forcefield_Zhang2013::setUp()
{
    // Atom types
    addAtomType(Elements::Zn, 1, "Zn", "-N", "Zinc", 0.6894, {0.280328, 2.462});
    addAtomType(Elements::N, 2, "N", "-Zn,ring(size=5,N,C,N,C,C)", "Imidazolate nitrogen", -0.28, {0.156063, 3.261});
    addAtomType(Elements::C, 3, "C1", "-C(nh=3),-N(n=2),ring(size=5,N,C,N,C,C)", "Imidazolate methylated carbon", 0.4184,
                {0.237233, 3.431});
    addAtomType(Elements::C, 4, "C2", "-H,ring(size=5,N,C,N,C,C)", "Imidazolate C4,5 carbon", -0.191, {0.237233, 3.431});
    addAtomType(Elements::C, 5, "C3", "nh=3,-C,ring(n=0)", "Methyl carbon on imidazolate", -0.5726, {0.237233, 3.431});
    addAtomType(Elements::H, 6, "H1", "-&5", "Hydrogen on methyl carbon", 0.1481, {0.0995792, 2.571});
    addAtomType(Elements::H, 7, "H2", "-&4", "Hydrogen on C4,5 carbon", 0.1536, {0.0995792, 2.571});

    // Intramolecular terms
    addBondTerm("N", "C1", SpeciesBond::HarmonicForm, {4083.58, 1.339});
    addBondTerm("N", "C2", SpeciesBond::HarmonicForm, {3430.88, 1.371});
    addBondTerm("C1", "C3", SpeciesBond::HarmonicForm, {2652.66, 1.492});
    addBondTerm("C2", "C2", SpeciesBond::HarmonicForm, {4334.62, 1.346});
    addBondTerm("C2", "H2", SpeciesBond::HarmonicForm, {3071.06, 0.929});
    addBondTerm("C3", "H1", SpeciesBond::HarmonicForm, {2845.12, 0.959});
    addBondTerm("Zn", "N", SpeciesBond::HarmonicForm, {719.648, 1.987});
    addAngleTerm("C1", "N", "C2", SpeciesAngle::HarmonicForm, {585.76, 105.24});
    addAngleTerm("N", "C1", "N", SpeciesAngle::HarmonicForm, {585.76, 112.17});
    addAngleTerm("N", "C1", "C3", SpeciesAngle::HarmonicForm, {585.76, 123.89});
    addAngleTerm("N", "C2", "C2", SpeciesAngle::HarmonicForm, {585.76, 108.67});
    addAngleTerm("N", "C2", "H2", SpeciesAngle::HarmonicForm, {418.4, 125.66});
    addAngleTerm("C2", "C2", "H2", SpeciesAngle::HarmonicForm, {418.4, 125.67});
    addAngleTerm("C1", "C3", "H1", SpeciesAngle::HarmonicForm, {418.4, 109.44});
    addAngleTerm("H1", "C3", "H1", SpeciesAngle::HarmonicForm, {292.88, 109.50});
    addAngleTerm("Zn", "N", "C1", SpeciesAngle::HarmonicForm, {418.4, 127.50});
    addAngleTerm("Zn", "N", "C2", SpeciesAngle::HarmonicForm, {292.88, 128.00});
    addAngleTerm("N", "Zn", "N", SpeciesAngle::HarmonicForm, {87.864, 109.47});
    addTorsionTerm("C2", "N", "C1", "N", SpeciesTorsion::CosineForm, {20.0832, 180.0, 2, 1.0});
    addTorsionTerm("C2", "N", "C1", "C3", SpeciesTorsion::CosineForm, {17.3636, 180.0, 2, 1.0});
    addTorsionTerm("C1", "N", "C2", "C2", SpeciesTorsion::CosineForm, {20.0832, 180.0, 2, 1.0});
    addTorsionTerm("C1", "N", "C2", "H2", SpeciesTorsion::CosineForm, {20.0832, 180.0, 2, 1.0});
    addTorsionTerm("N", "C2", "C2", "N", SpeciesTorsion::CosineForm, {16.736, 180.0, 2, 1.0});
    addTorsionTerm("N", "C2", "C2", "H2", SpeciesTorsion::CosineForm, {16.736, 180.0, 2, 1.0});
    addTorsionTerm("H2", "C2", "C2", "H2", SpeciesTorsion::CosineForm, {16.736, 180.0, 2, 1.0});
    addTorsionTerm("Zn", "N", "C1", "N", SpeciesTorsion::CosineForm, {0.4184, 180.0, 2, 1.0});
    addTorsionTerm("Zn", "N", "C1", "C3", SpeciesTorsion::CosineForm, {0.4184, 180.0, 2, 1.0});
    addTorsionTerm("Zn", "N", "C2", "C2", SpeciesTorsion::CosineForm, {0.4184, 180.0, 2, 1.0});
    addTorsionTerm("N", "Zn", "N", "C1", SpeciesTorsion::CosineForm, {0.728016, 0.0, 3, 1.0});
    addTorsionTerm("N", "Zn", "N", "C2", SpeciesTorsion::CosineForm, {0.728016, 0.0, 3, 1.0});
    addTorsionTerm("H2", "C2", "N", "Zn", SpeciesTorsion::NoForm, {});
    addTorsionTerm("H1", "C3", "C1", "N", SpeciesTorsion::NoForm, {});
    addImproperTerm("C1", "C3", "N", "N", SpeciesTorsion::CosineForm, {4.6024, 180.0, 2, 1.0});
    addImproperTerm("C2", "C2", "H2", "N", SpeciesTorsion::CosineForm, {4.6024, 180.0, 2, 1.0});

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_Zhang2013::name() const { return "Zhang2013"; }

// Return description for Forcefield
std::string_view Forcefield_Zhang2013::description() const
{
    return "Implements ZIF-8 forcefield found in 'Sorption-Induced Structural Transition of "
           "Zeolitic Imidazolate Framework-8: A Hybrid Molecular Simulation Study', L. Zhang, Z. Hu and J. Jiang, <i>J. Am. "
           "Chem. Soc.</i> <b>135</b>, 3722 (2013), <a href='http:/dx.doi.org/10.1021/ja401129h'></a>.<br/>Covers only ZIF-8 "
           "parameters listed in the supplementary information. Sorbate parameters are not included.";
}

// Return short-range interaction style for AtomTypes
Forcefield::ShortRangeType Forcefield_Zhang2013::shortRangeType() const { return Forcefield::ShortRangeType::LennardJones; }
