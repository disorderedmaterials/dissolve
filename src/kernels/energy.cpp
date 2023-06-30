// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "kernels/energy.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/molecule.h"
#include "classes/potentialmap.h"
#include "classes/species.h"
#include "templates/algorithms.h"
#include <iterator>
#include <numeric>

EnergyKernel::EnergyKernel(const Configuration *cfg, const ProcessPool &procPool, const PotentialMap &potentialMap,
                           std::optional<double> energyCutoff)
    : GeometryKernel(cfg, procPool, potentialMap, energyCutoff)
{
}

/*
 * Base Routines
 */

// Return PairPotential energy between atoms
double EnergyKernel::pairPotentialEnergy(const Atom &i, const Atom &j, double r) const { return potentialMap_.energy(i, j, r); }

// Return PairPotential energy between atoms, scaling electrostatic and van der Waals components
double EnergyKernel::pairPotentialEnergy(const Atom &i, const Atom &j, double r, double elecScale, double vdwScale) const
{
    return potentialMap_.energy(i, j, r, elecScale, vdwScale);
}

/*
 * PairPotential Terms
 */

// Return PairPotential energy of atoms in the supplied cell
double EnergyKernel::cellEnergy(const Cell &cell, bool includeIntraMolecular) const
{
    auto totalEnergy = 0.0;
    auto &atoms = cell.atoms();

    for (auto i = 0; i < atoms.size(); ++i)
    {
        auto &ii = atoms[i];
        auto molI = ii->molecule();
        auto &rI = ii->r();

        // Straight loop over other cell atoms
        for (auto j = i + 1; j < atoms.size(); ++j)
        {
            // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
            auto &jj = atoms[j];

            auto rSq = (rI - jj->r()).magnitudeSq();
            if (rSq > cutoffDistanceSquared_)
                continue;

            // Check for atoms in the same molecule
            if (molI != jj->molecule())
                totalEnergy += pairPotentialEnergy(*ii, *jj, sqrt(rSq));
            else if (includeIntraMolecular)
            {
                auto &&[scalingType, elec14, vdw14] = ii->scaling(jj);
                if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                    totalEnergy += pairPotentialEnergy(*ii, *jj, sqrt(rSq));
                else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                    totalEnergy += pairPotentialEnergy(*ii, *jj, sqrt(rSq), elec14, vdw14);
            }
        }
    }

    return totalEnergy;
}

// Return PairPotential energy between atoms in supplied cells
double EnergyKernel::cellToCellEnergy(const Cell &centralCell, const Cell &otherCell, bool applyMim,
                                      bool includeIntraMolecular) const
{
    auto totalEnergy = 0.0;
    auto &centralAtoms = centralCell.atoms();
    auto &otherAtoms = otherCell.atoms();

    // Loop over central cell atoms
    if (applyMim)
    {
        for (auto &ii : centralAtoms)
        {
            auto molI = ii->molecule();
            auto &rI = ii->r();

            // Straight loop over other cell atoms
            for (const auto &jj : otherAtoms)
            {
                // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
                auto rSq = box_->minimumDistanceSquared(rI, jj->r());
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (molI != jj->molecule())
                    totalEnergy += pairPotentialEnergy(*ii, *jj, sqrt(rSq));
                else if (includeIntraMolecular)
                {
                    auto &&[scalingType, elec14, vdw14] = ii->scaling(jj);
                    if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                        totalEnergy += pairPotentialEnergy(*ii, *jj, sqrt(rSq));
                    else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                        totalEnergy += pairPotentialEnergy(*ii, *jj, sqrt(rSq), elec14, vdw14);
                }
            }
        }
    }
    else
    {
        for (auto &ii : centralAtoms)
        {
            auto &molI = ii->molecule();
            auto &rI = ii->r();

            // Straight loop over other cell atoms
            for (const auto &jj : otherAtoms)
            {
                // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
                auto rSq = (rI - jj->r()).magnitudeSq();
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same molecule
                if (molI != jj->molecule())
                    totalEnergy += pairPotentialEnergy(*ii, *jj, sqrt(rSq));
                else if (includeIntraMolecular)
                {
                    auto &&[scalingType, elec14, vdw14] = ii->scaling(jj);
                    if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                        totalEnergy += pairPotentialEnergy(*ii, *jj, sqrt(rSq));
                    else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                        totalEnergy += pairPotentialEnergy(*ii, *jj, sqrt(rSq), elec14, vdw14);
                }
            }
        }
    }

    return totalEnergy;
}

// Return PairPotential energy of Atom with world
double EnergyKernel::pairPotentialEnergy(const Atom &i) const
{
    assert(cellArray_);
    auto &cells = cellArray_->get();

    // Get cell neighbours for atom i's cell
    auto &neighbours = cells.neighbours(*i.cell());

    return dissolve::transform_reduce(ParallelPolicies::par, neighbours.begin(), neighbours.end(), 0.0, std::plus<double>(),
                                      [&i, this](const auto &neighbour)
                                      {
                                          auto mimRequired = neighbour.requiresMIM_;
                                          auto &nbrCellAtoms = neighbour.neighbour_.atoms();
                                          return std::accumulate(
                                              nbrCellAtoms.begin(), nbrCellAtoms.end(), 0.0,
                                              [&i, mimRequired, this](const auto innerAcc, const auto *j)
                                              {
                                                  auto &jj = *j;

                                                  // Calculate rSquared distance between atoms, and check it against
                                                  // the stored cutoff distance
                                                  auto rSq = mimRequired ? box_->minimumDistanceSquared(i.r(), jj.r())
                                                                         : (i.r() - jj.r()).magnitudeSq();
                                                  if (rSq > cutoffDistanceSquared_)
                                                      return innerAcc;

                                                  // Check for atoms in the same species
                                                  if (i.molecule().get() != jj.molecule().get())
                                                      return innerAcc + pairPotentialEnergy(i, jj, sqrt(rSq));

                                                  return innerAcc;
                                              });
                                      });
}

// Return PairPotential energy of Molecule with world
double EnergyKernel::pairPotentialEnergy(const Molecule &mol, bool includeIntraMolecular) const
{
    assert(cellArray_);
    auto &cells = cellArray_->get();

    // Create a map of atoms in cells so we can treat all atoms with the same set of neighbours at once
    std::map<Cell *, std::vector<const Atom *>> locationMap;
    for (auto &i : mol.atoms())
        locationMap[i->cell()].push_back(i);

    auto totalEnergy = std::accumulate(
        locationMap.begin(), locationMap.end(), 0.0,
        [&](const auto totalAcc, const auto &location)
        {
            const auto &centralCellAtoms = location.second;

            // Get cell neighbours for the cell
            auto &neighbours = cells.neighbours(*location.first);

            auto localEnergy = dissolve::transform_reduce(
                ParallelPolicies::par, neighbours.begin(), neighbours.end(), 0.0, std::plus<double>(),
                [&centralCellAtoms, includeIntraMolecular, this](const auto &neighbour)
                {
                    return std::accumulate(
                        centralCellAtoms.begin(), centralCellAtoms.end(), 0.0,
                        [&neighbour, includeIntraMolecular, this](const auto acc, const auto &i)
                        {
                            auto &ii = *i;
                            auto mimRequired = neighbour.requiresMIM_;
                            auto &nbrCellAtoms = neighbour.neighbour_.atoms();
                            return acc + std::accumulate(
                                             nbrCellAtoms.begin(), nbrCellAtoms.end(), 0.0,
                                             [&ii, mimRequired, includeIntraMolecular, this](const auto innerAcc, const auto *j)
                                             {
                                                 auto &jj = *j;

                                                 // Same molecule?
                                                 auto sameMol = ii.molecule().get() == jj.molecule().get();
                                                 if (sameMol)
                                                 {
                                                     if (!includeIntraMolecular)
                                                         return innerAcc;
                                                     else if (&ii == &jj)
                                                         return innerAcc;
                                                 }

                                                 // Calculate rSquared distance between atoms, and check it against
                                                 // the stored cutoff distance
                                                 auto rSq = mimRequired ? box_->minimumDistanceSquared(ii.r(), jj.r())
                                                                        : (ii.r() - jj.r()).magnitudeSq();
                                                 if (rSq > cutoffDistanceSquared_)
                                                     return innerAcc;

                                                 // If the same molecule need to check scaling (other checks already made above)
                                                 if (sameMol)
                                                 {
                                                     auto &&[scalingType, elec14, vdw14] = ii.scaling(&jj);
                                                     if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                                                         return innerAcc + pairPotentialEnergy(ii, jj, sqrt(rSq));
                                                     else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                                                         return innerAcc +
                                                                pairPotentialEnergy(ii, jj, sqrt(rSq), elec14, vdw14);
                                                     else
                                                         return innerAcc;
                                                 }
                                                 else
                                                     return innerAcc + pairPotentialEnergy(ii, jj, sqrt(rSq));
                                             });
                        });
                });

            return totalAcc + localEnergy;
        });

    return totalEnergy;
}

/*
 * Extended Terms
 */

// Return energy of supplied atom from ad hoc extended terms
double EnergyKernel::extendedEnergy(const Atom &i) const { return 0.0; }

// Return energy of supplied molecule from ad hoc extended terms
double EnergyKernel::extendedEnergy(const Molecule &mol) const { return 0.0; }

/*
 * Totals
 */

// Return total interatomic PairPotential energy of the world
double EnergyKernel::totalPairPotentialEnergy(bool includeIntraMolecular, ProcessPool::DivisionStrategy strategy) const
{
    assert(cellArray_);
    auto &cells = cellArray_->get();

    // List of cell neighbour pairs
    auto &cellNeighbourPairs = cells.getCellNeighbourPairs();

    // Set start/stride for parallel loop
    auto offset = processPool_.interleavedLoopStart(strategy);
    auto nChunks = processPool_.interleavedLoopStride(strategy);

    auto totalEnergy = 0.0;
    auto [begin, end] = chop_range(cellNeighbourPairs.begin(), cellNeighbourPairs.end(), nChunks, offset);

    totalEnergy += dissolve::transform_reduce(ParallelPolicies::par, begin, end, 0.0, std::plus<double>(),
                                              [&](const auto &pair)
                                              {
                                                  auto &cellI = pair.master_;
                                                  auto &cellJ = pair.neighbour_;
                                                  auto mimRequired = pair.requiresMIM_;
                                                  if (&cellI == &cellJ)
                                                      return cellEnergy(cellI, includeIntraMolecular);
                                                  else
                                                      return cellToCellEnergy(cellI, cellJ, mimRequired, includeIntraMolecular);
                                              });

    return totalEnergy;
}

// Return total interatomic PairPotential energy from summation of molecules
double EnergyKernel::totalMoleculePairPotentialEnergy(bool includeIntraMolecular) const
{
    assert(molecules_);
    auto &mols = molecules_->get();
    auto molecularEnergy = 0.0;
    for (const auto &mol : mols)
        molecularEnergy += pairPotentialEnergy(*mol, includeIntraMolecular);

    // In the typical case where there is more than one molecule, our sum will contain double the intermolecular
    // pairpotential energy, and zero intramolecular energy
    if (mols.size() > 1)
        molecularEnergy *= 0.5;

    return molecularEnergy;
}

// Return total energy of supplied atom with the world
EnergyResult EnergyKernel::totalEnergy(const Atom &i) const
{
    return {pairPotentialEnergy(i), totalGeometryEnergy(i), extendedEnergy(i)};
}

// Return total energy of supplied molecule with the world
EnergyResult EnergyKernel::totalEnergy(const Molecule &mol, Flags<EnergyCalculationFlags> flags) const
{
    return {flags.isSet(ExcludePairPotential) ? 0.0
                                              : pairPotentialEnergy(mol, !flags.isSet(ExcludeIntraMolecularPairPotential)),
            flags.isSet(ExcludeGeometry) ? 0.0 : totalGeometryEnergy(mol),
            flags.isSet(ExcludeExtended) ? 0.0 : extendedEnergy(mol)};
}
