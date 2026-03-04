// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "kernels/species.h"
#include "classes/box.h"
#include "classes/potentialMap.h"
#include "classes/species.h"
#include "math/combinations.h"
#include "templates/algorithms.h"
#include <numeric>

/*
 * Totals
 */

// Return pair potential energy of Species
PairPotentialEnergyValue SpeciesKernel::pairPotentialEnergy(const Species *sp)
{
    const auto cutoff = PairPotential::range();

    Combinations comb(sp->nAtoms());
    return dissolve::transform_reduce(ParallelPolicies::par, dissolve::counting_iterator<int>(0),
                                      dissolve::counting_iterator<int>(comb.getNumCombinations()), 0.0, std::plus<>(),
                                      [&](const auto idx)
                                      {
                                          auto [n, m] = comb.nthCombination(idx);
                                          auto &i = sp->atom(n);
                                          auto &j = sp->atom(m);
                                          auto &rI = i.r();
                                          auto &rJ = j.r();

                                          // Get interatomic distance
                                          double r = (rJ - rI).magnitude();
                                          if (r > cutoff)
                                              return 0.0;

                                          // Get intramolecular scaling of atom pair
                                          auto &&[scalingType, elec14, vdw14] = i.scaling(&j);
                                          if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                                              return potentialMap_.energy(&i, &j, r);
                                          else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                                              return potentialMap_.energy(&i, &j, r, elec14, vdw14);

                                          return 0.0;
                                      });
}

// Return geometric energy of Species
GeometryEnergyValue SpeciesKernel::geometryEnergy(const Species *sp)
{
    GeometryEnergyValue energy;

    // Loop over bonds
    energy.bondEnergy = std::accumulate(sp->bonds().begin(), sp->bonds().end(), 0.0, [&](const auto acc, const auto &b)
                                        { return acc + bondEnergy(b, b.j()->r(), b.i()->r()); });

    // Loop over angles
    energy.angleEnergy = std::accumulate(sp->angles().begin(), sp->angles().end(), 0.0, [&](const auto acc, const auto &a)
                                         { return acc + angleEnergy(a, a.i()->r(), a.j()->r(), a.k()->r()); });

    // Loop over torsions
    energy.torsionEnergy = std::accumulate(sp->torsions().begin(), sp->torsions().end(), 0.0, [&](const auto acc, const auto &t)
                                           { return acc + torsionEnergy(t, t.i()->r(), t.j()->r(), t.k()->r(), t.l()->r()); });

    // Loop over impropers
    energy.improperEnergy =
        std::accumulate(sp->impropers().begin(), sp->impropers().end(), 0.0, [&](const auto acc, const auto &imp)
                        { return acc + improperEnergy(imp, imp.i()->r(), imp.j()->r(), imp.k()->r(), imp.l()->r()); });

    return energy;
}

// Return total energy (interatomic and intramolecular) of Species
EnergyResult SpeciesKernel::totalEnergy(const Species *sp) { return {pairPotentialEnergy(sp), geometryEnergy(sp)}; }
