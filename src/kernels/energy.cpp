// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "kernels/energy.h"
#include "classes/atomVector.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/molecule.h"
#include "classes/potentialMap.h"
#include "classes/species.h"
#include "templates/algorithms.h"
#include <iterator>
#include <numeric>

/*
 * PairPotentialEnergyValue
 */

PairPotentialEnergyValue::PairPotentialEnergyValue(double inter, double intra) : interMolecular_(inter), intraMolecular_(intra)
{
}
PairPotentialEnergyValue PairPotentialEnergyValue::operator+(const PairPotentialEnergyValue &value) const
{
    return {interMolecular_ + value.interMolecular_, intraMolecular_ + value.intraMolecular_};
}
PairPotentialEnergyValue PairPotentialEnergyValue::operator-(const PairPotentialEnergyValue &value) const
{
    return {interMolecular_ - value.interMolecular_, intraMolecular_ - value.intraMolecular_};
}
PairPotentialEnergyValue &PairPotentialEnergyValue::operator+=(const PairPotentialEnergyValue &value)
{
    interMolecular_ += value.interMolecular_;
    intraMolecular_ += value.intraMolecular_;
    return *this;
}
PairPotentialEnergyValue &PairPotentialEnergyValue::operator*=(const double scale)
{
    interMolecular_ *= scale;
    intraMolecular_ *= scale;
    return *this;
}

// Increment Energies
void PairPotentialEnergyValue::addInterMolecular(double e) { interMolecular_ += e; }
void PairPotentialEnergyValue::addIntraMolecular(double e) { intraMolecular_ += e; }

// Return Energies
double PairPotentialEnergyValue::interMolecular() const { return interMolecular_; }
double PairPotentialEnergyValue::intraMolecular() const { return intraMolecular_; };
double PairPotentialEnergyValue::total() const { return interMolecular_ + intraMolecular_; }

/*
 * EnergyKernel
 */

EnergyKernel::EnergyKernel(const Configuration *cfg, const ProcessPool &procPool, const PotentialMap &potentialMap,
                           std::optional<double> energyCutoff)
    : GeometryKernel(cfg, procPool, potentialMap, energyCutoff)
{
}

/*
 * Base Routines
 */

// Return PairPotential energy between atoms
double EnergyKernel::pairPotentialEnergy(const Atom i, const Atom j, double r) const { return potentialMap_.energy(i, j, r); }

// Return PairPotential energy between atoms, scaling electrostatic and van der Waals components
double EnergyKernel::pairPotentialEnergy(const Atom i, const Atom j, double r, double elecScale, double vdwScale) const
{
    return potentialMap_.energy(i, j, r, elecScale, vdwScale);
}

/*
 * PairPotential Terms
 */

// Return PairPotential energy of atoms in the supplied cell
PairPotentialEnergyValue EnergyKernel::cellEnergy(const Cell &cell, bool includeIntraMolecular) const
{
    PairPotentialEnergyValue totalEnergy;
    auto &atoms = cell.atoms();

    for (auto i = 0; i < atoms.size(); ++i)
    {
        auto &ii = atoms[i];
        auto molI = ii.molecule();
        auto &rI = ii.r();

        // Straight loop over other cell atoms
        for (auto j = i + 1; j < atoms.size(); ++j)
        {
            // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
            auto &jj = atoms[j];

            auto rSq = (rI - jj.r()).magnitudeSq();
            if (rSq > cutoffDistanceSquared_)
                continue;

            // Check for atoms in the same molecule
            if (molI != jj.molecule())
                totalEnergy.addInterMolecular(pairPotentialEnergy(ii, jj, sqrt(rSq)));
            else if (includeIntraMolecular)
            {
                auto &&[scalingType, elec14, vdw14] = ii.scaling(jj);
                if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                    totalEnergy.addIntraMolecular(pairPotentialEnergy(ii, jj, sqrt(rSq)));
                else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                    totalEnergy.addIntraMolecular(pairPotentialEnergy(ii, jj, sqrt(rSq), elec14, vdw14));
            }
        }
    }

    return totalEnergy;
}

// Return PairPotential energy between atoms in supplied cells
PairPotentialEnergyValue EnergyKernel::cellToCellEnergy(const Cell &centralCell, const Cell &otherCell, bool applyMim,
                                                        bool includeIntraMolecular) const
{
    PairPotentialEnergyValue totalEnergy;
    auto &centralAtoms = centralCell.atoms();
    auto &otherAtoms = otherCell.atoms();

    // Loop over central cell atoms
    if (applyMim)
    {
        for (auto &ii : centralAtoms)
        {
            auto molI = ii.molecule();
            auto &rI = ii.r();

            // Straight loop over other cell atoms
            for (const auto &jj : otherAtoms)
            {
                // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
                auto rSq = box_->minimumDistanceSquared(rI, jj.r());
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (molI != jj.molecule())
                    totalEnergy.addInterMolecular(pairPotentialEnergy(ii, jj, sqrt(rSq)));
                else if (includeIntraMolecular)
                {
                    auto &&[scalingType, elec14, vdw14] = ii.scaling(jj);
                    if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                        totalEnergy.addIntraMolecular(pairPotentialEnergy(ii, jj, sqrt(rSq)));
                    else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                        totalEnergy.addIntraMolecular(pairPotentialEnergy(ii, jj, sqrt(rSq), elec14, vdw14));
                }
            }
        }
    }
    else
    {
        for (auto &ii : centralAtoms)
        {
            auto &molI = ii.molecule();
            auto &rI = ii.r();

            // Straight loop over other cell atoms
            for (const auto &jj : otherAtoms)
            {
                // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
                auto rSq = (rI - jj.r()).magnitudeSq();
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same molecule
                if (molI != jj.molecule())
                    totalEnergy.addInterMolecular(pairPotentialEnergy(ii, jj, sqrt(rSq)));
                else if (includeIntraMolecular)
                {
                    auto &&[scalingType, elec14, vdw14] = ii.scaling(jj);
                    if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                        totalEnergy.addIntraMolecular(pairPotentialEnergy(ii, jj, sqrt(rSq)));
                    else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                        totalEnergy.addIntraMolecular(pairPotentialEnergy(ii, jj, sqrt(rSq), elec14, vdw14));
                }
            }
        }
    }

    return totalEnergy;
}

// Return PairPotential energy of Atom with world
double EnergyKernel::pairPotentialEnergy(const Atom i) const
{
    assert(cellArray_);
    auto &cells = cellArray_->get();

    // Get cell neighbours for atom i's cell
    auto &neighbours = cells.neighbours(*i.cell());

    return dissolve::transform_reduce(ParallelPolicies::par, neighbours.begin(), neighbours.end(), 0.0, std::plus<>(),
                                      [&i, this](const auto &neighbour)
                                      {
                                          auto mimRequired = neighbour.requiresMIM_;
                                          auto &nbrCellAtoms = neighbour.neighbour_.atoms();
                                          return std::accumulate(
                                              nbrCellAtoms.begin(), nbrCellAtoms.end(), 0.0,
                                              [&i, mimRequired, this](const auto innerAcc, const auto j)
                                              {
                                                  // Calculate rSquared distance between atoms, and check it against
                                                  // the stored cutoff distance
                                                  auto rSq = mimRequired ? box_->minimumDistanceSquared(i.r(), j.r())
                                                                         : (i.r() - j.r()).magnitudeSq();
                                                  if (rSq > cutoffDistanceSquared_)
                                                      return innerAcc;

                                                  // Check for atoms in the same species
                                                  if (i.molecule().get() != j.molecule().get())
                                                      return innerAcc + pairPotentialEnergy(i, j, sqrt(rSq));

                                                  return innerAcc;
                                              });
                                      });
}

// Return PairPotential energy of Molecule with world
PairPotentialEnergyValue EnergyKernel::pairPotentialEnergy(const Molecule &mol, bool includeIntraMolecular) const
{
    assert(cellArray_);
    auto &cells = cellArray_->get();

    // Create a map of atoms in cells so we can treat all atoms with the same set of neighbours at once
    std::map<Cell *, std::vector<Atom>> locationMap;
    for (auto &i : mol.atoms())
        locationMap[i.cell()].push_back(i);

    auto totalEnergy = std::accumulate(
        locationMap.begin(), locationMap.end(), PairPotentialEnergyValue(),
        [&](const auto totalAcc, const auto &location)
        {
            const auto &centralCellAtoms = location.second;

            // Get cell neighbours for the cell
            auto &neighbours = cells.neighbours(*location.first);

            auto localEnergy = dissolve::transform_reduce(
                ParallelPolicies::par, neighbours.begin(), neighbours.end(), PairPotentialEnergyValue(), std::plus<>(),
                [&centralCellAtoms, includeIntraMolecular, this](const auto &neighbour)
                {
                    return std::accumulate(
                        centralCellAtoms.begin(), centralCellAtoms.end(), PairPotentialEnergyValue(),
                        [&neighbour, includeIntraMolecular, this](const auto acc, const auto &i)
                        {
                            auto mimRequired = neighbour.requiresMIM_;
                            auto &nbrCellAtoms = neighbour.neighbour_.atoms();
                            return acc +
                                   std::accumulate(
                                       nbrCellAtoms.begin(), nbrCellAtoms.end(), PairPotentialEnergyValue(),
                                       [i, mimRequired, includeIntraMolecular, this](const auto innerAcc, const auto j)
                                       {

                                           // Same molecule?
                                           auto sameMol = i.molecule().get() == j.molecule().get();
                                           if (sameMol)
                                           {
                                               if (!includeIntraMolecular)
                                                   return innerAcc;
                                               else if (&i == &j)
                                                   return innerAcc;
                                           }

                                           // Calculate rSquared distance between atoms, and check it against
                                           // the stored cutoff distance
                                           auto rSq = mimRequired ? box_->minimumDistanceSquared(i.r(), j.r())
                                                                  : (i.r() - j.r()).magnitudeSq();
                                           if (rSq > cutoffDistanceSquared_)
                                               return innerAcc;

                                           // If the same molecule need to check scaling (other checks already made above)
                                           if (sameMol)
                                           {
                                               auto &&[scalingType, elec14, vdw14] = i.scaling(j);
                                               if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                                                   return innerAcc +
                                                          PairPotentialEnergyValue(0.0, pairPotentialEnergy(i, j, sqrt(rSq)));
                                               else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                                                   return innerAcc +
                                                          PairPotentialEnergyValue(
                                                              0.0, pairPotentialEnergy(i, j, sqrt(rSq), elec14, vdw14));
                                               else
                                                   return innerAcc;
                                           }
                                           else
                                               return innerAcc +
                                                      PairPotentialEnergyValue(pairPotentialEnergy(i, j, sqrt(rSq)), 0.0);
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
double EnergyKernel::extendedEnergy(const Atom i) const { return 0.0; }

// Return energy of supplied molecule from ad hoc extended terms
double EnergyKernel::extendedEnergy(const Molecule &mol) const { return 0.0; }

/*
 * Totals
 */

// Return total interatomic PairPotential energy of the world
PairPotentialEnergyValue EnergyKernel::totalPairPotentialEnergy(bool includeIntraMolecular,
                                                                ProcessPool::DivisionStrategy strategy) const
{
    assert(cellArray_);
    auto &cells = cellArray_->get();

    // List of cell neighbour pairs
    auto &cellNeighbourPairs = cells.getCellNeighbourPairs();

    // Set start/stride for parallel loop
    auto offset = processPool_.interleavedLoopStart(strategy);
    auto nChunks = processPool_.interleavedLoopStride(strategy);

    PairPotentialEnergyValue ppEnergy;
    auto [begin, end] = chop_range(cellNeighbourPairs.begin(), cellNeighbourPairs.end(), nChunks, offset);

    ppEnergy += dissolve::transform_reduce(ParallelPolicies::par, begin, end, PairPotentialEnergyValue(), std::plus<>(),
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

    return ppEnergy;
}

// Return total interatomic PairPotential energy from summation of molecules
PairPotentialEnergyValue EnergyKernel::totalMoleculePairPotentialEnergy(bool includeIntraMolecular) const
{
    assert(molecules_);
    auto &mols = molecules_->get();
    PairPotentialEnergyValue molecularEnergy;
    for (const auto &mol : mols)
        molecularEnergy += pairPotentialEnergy(*mol, includeIntraMolecular);

    // In the typical case where there is more than one molecule, our sum will contain double the intermolecular
    // pairpotential energy, and zero intramolecular energy
    if (mols.size() > 1)
        molecularEnergy *= 0.5;

    return molecularEnergy;
}

// Return total energy of supplied atom with the world
EnergyResult EnergyKernel::totalEnergy(const Atom i) const
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
