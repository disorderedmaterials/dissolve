// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "kernels/species.h"
#include "classes/box.h"
#include "classes/potentialMap.h"
#include "classes/species.h"
#include "templates/algorithms.h"
#include <iterator>
#include <numeric>

// Return PairPotential energy between atoms
double SpeciesKernel::pairPotentialEnergy(const SpeciesAtom *i, const SpeciesAtom *j, double r) const
{
    return potentialMap_.energy(i, j, r);
}

// Return PairPotential energy between atoms, scaling electrostatic and van der Waals components
double SpeciesKernel::pairPotentialEnergy(const SpeciesAtom *i, const SpeciesAtom *j, double r, double elecScale,
                                          double srScale) const
{
    return potentialMap_.energy(i, j, r, elecScale, srScale);
}

/*
 * PairPotential Terms
 */

/*
 * Totals
 */

// // Return total energy of supplied molecule with the world
// EnergyResult SpeciesKernel::totalEnergy(const Molecule &mol, Flags<EnergyCalculationFlags> flags) const
// {
//     return {flags.isSet(ExcludePairPotential) ? 0.0
//                                               : pairPotentialEnergy(mol, !flags.isSet(ExcludeIntraMolecularPairPotential)),
//             flags.isSet(ExcludeGeometry) ? 0.0 : totalGeometryEnergy(mol),
//             flags.isSet(ExcludeExtended) ? 0.0 : extendedEnergy(mol)};
// }
