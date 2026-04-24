// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "kernels/energy.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/molecule.h"
#include "classes/potentialMap.h"
#include "classes/species.h"
#include "templates/algorithms.h"
#include <iterator>
#include <numeric>

EnergyKernel::EnergyKernel(const Configuration *cfg, const PotentialMap &potentialMap)
    : GeometryKernel(cfg->box(), potentialMap), configuration_(cfg)
{
}

/*
 * Base Routines
 */

// Return PairPotential energy between atoms
double EnergyKernel::pairPotentialEnergy(const ConfigurationAtom &i, const ConfigurationAtom &j, double r) const
{
    return potentialMap_.energy(i, j, r);
}

// Return PairPotential energy between atoms, scaling electrostatic and van der Waals components
double EnergyKernel::pairPotentialEnergy(const ConfigurationAtom &i, const ConfigurationAtom &j, double r, double elecScale,
                                         double srScale) const
{
    return potentialMap_.energy(i, j, r, elecScale, srScale);
}

/*
 * PairPotential Terms
 */

// Return PairPotential energy of atoms in the supplied cell
Kernel::PairPotentialEnergyValue EnergyKernel::cellEnergy(const Cell &cell, bool includeInter, bool includeIntra) const
{
    Kernel::PairPotentialEnergyValue totalEnergy;
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

            if (molI != jj->molecule())
            {
                if (includeInter)
                    totalEnergy.interMolecular += pairPotentialEnergy(*ii, *jj, sqrt(rSq));
            }
            else if (includeIntra)
            {
                auto &&[scalingType, elec14, vdw14] = ii->scaling(jj);
                if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                    totalEnergy.intraMolecular += pairPotentialEnergy(*ii, *jj, sqrt(rSq));
                else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                    totalEnergy.intraMolecular += pairPotentialEnergy(*ii, *jj, sqrt(rSq), elec14, vdw14);
            }
        }
    }

    return totalEnergy;
}

// Return PairPotential energy between atoms in supplied cells
Kernel::PairPotentialEnergyValue EnergyKernel::cellToCellEnergy(const Cell &centralCell, const Cell &otherCell, bool applyMim,
                                                                bool includeInter, bool includeIntra) const
{
    Kernel::PairPotentialEnergyValue totalEnergy;
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
                {
                    if (includeInter)
                        totalEnergy.interMolecular += pairPotentialEnergy(*ii, *jj, sqrt(rSq));
                }
                else if (includeIntra)
                {
                    auto &&[scalingType, elec14, vdw14] = ii->scaling(jj);
                    if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                        totalEnergy.intraMolecular += pairPotentialEnergy(*ii, *jj, sqrt(rSq));
                    else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                        totalEnergy.intraMolecular += pairPotentialEnergy(*ii, *jj, sqrt(rSq), elec14, vdw14);
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
                {
                    if (includeInter)
                        totalEnergy.interMolecular += pairPotentialEnergy(*ii, *jj, sqrt(rSq));
                }
                else if (includeIntra)
                {
                    auto &&[scalingType, elec14, vdw14] = ii->scaling(jj);
                    if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                        totalEnergy.intraMolecular += pairPotentialEnergy(*ii, *jj, sqrt(rSq));
                    else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                        totalEnergy.intraMolecular += pairPotentialEnergy(*ii, *jj, sqrt(rSq), elec14, vdw14);
                }
            }
        }
    }

    return totalEnergy;
}

// Return PairPotential energy of Atom with world
double EnergyKernel::pairPotentialEnergy(const ConfigurationAtom &i) const
{
    auto &cells = configuration_->cells();

    // Get cell neighbours for atom i's cell
    auto &neighbours = cells.neighbours(*i.cell());

    return dissolve::transform_reduce(ParallelPolicies::par, neighbours.begin(), neighbours.end(), 0.0, std::plus<>(),
                                      [&i, this](const auto &neighbour)
                                      {
                                          auto mimRequired = neighbour.requiresMIM;
                                          auto &nbrCellAtoms = neighbour.cell.atoms();
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
Kernel::PairPotentialEnergyValue EnergyKernel::pairPotentialEnergy(const Molecule &mol, bool includeInter,
                                                                   bool includeIntra) const
{
    auto &cells = configuration_->cells();

    // Create a map of atoms in cells so we can treat all atoms with the same set of neighbours at once
    std::map<Cell *, std::vector<const ConfigurationAtom *>> locationMap;
    for (auto &i : mol.atoms())
        locationMap[i->cell()].push_back(i);

    Kernel::PairPotentialEnergyValue energy;

    if (includeInter)
    {
        energy.interMolecular = std::accumulate(
            locationMap.begin(), locationMap.end(), 0.0,
            [&](const auto totalAcc, const auto &location)
            {
                const auto &centralCellAtoms = location.second;

                // Get cell neighbours for the cell
                auto &neighbours = cells.neighbours(*location.first);

                auto localEnergy = dissolve::transform_reduce(
                    ParallelPolicies::par, neighbours.begin(), neighbours.end(), 0.0, std::plus<>(),
                    [&centralCellAtoms, this](const auto &neighbour)
                    {
                        return std::accumulate(
                            centralCellAtoms.begin(), centralCellAtoms.end(), 0.0,
                            [&neighbour, this](const auto acc, const auto &i)
                            {
                                auto &ii = *i;
                                auto mimRequired = neighbour.requiresMIM;
                                auto &nbrCellAtoms = neighbour.cell.atoms();
                                return acc + std::accumulate(nbrCellAtoms.begin(), nbrCellAtoms.end(), 0.0,
                                                             [&ii, mimRequired, this](const auto innerAcc, const auto *j)
                                                             {
                                                                 auto &jj = *j;

                                                                 // Don't consider atoms within the target molecule here - add
                                                                 // it on afterwards
                                                                 if (ii.molecule().get() == jj.molecule().get())
                                                                     return innerAcc;

                                                                 // Calculate rSquared distance between atoms, and check it
                                                                 // against the stored cutoff distance
                                                                 auto rSq = mimRequired
                                                                                ? box_->minimumDistanceSquared(ii.r(), jj.r())
                                                                                : (ii.r() - jj.r()).magnitudeSq();
                                                                 if (rSq > cutoffDistanceSquared_)
                                                                     return innerAcc;

                                                                 return innerAcc + pairPotentialEnergy(ii, jj, sqrt(rSq));
                                                             });
                            });
                    });

                return totalAcc + localEnergy;
            });
    }

    // Include intramolecular pairpotential (self terms)?
    if (includeIntra)
    {
        auto intra = 0.0;
        dissolve::for_each_pair(ParallelPolicies::seq, mol.nAtoms(),
                                [&](int i, int j)
                                {
                                    if (i == j)
                                        return;
                                    const auto &ii = *mol.atom(i);
                                    const auto &jj = *mol.atom(j);
                                    auto rSq = box_->minimumDistanceSquared(ii.r(), jj.r());

                                    if (rSq <= cutoffDistanceSquared_)
                                    {
                                        auto &&[scalingType, elec14, vdw14] = ii.scaling(&jj);
                                        if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                                            intra += pairPotentialEnergy(ii, jj, sqrt(rSq));
                                        else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                                            intra += pairPotentialEnergy(ii, jj, sqrt(rSq), elec14, vdw14);
                                    }
                                });
        energy.intraMolecular = intra;
    }

    return energy;
}

/*
 * Extended Terms
 */

// Return total extended energy
double EnergyKernel::totalExtendedEnergy() const
{
    const auto &molecules = configuration_->molecules();

    return dissolve::transform_reduce(ParallelPolicies::par, molecules.begin(), molecules.end(), 0.0, std::plus(),
                                      [&](const auto &mol) { return extendedEnergy(*mol); });
}

// Return energy of supplied atom from ad hoc extended terms
double EnergyKernel::extendedEnergy(const ConfigurationAtom &i) const { return 0.0; }

// Return energy of supplied molecule from ad hoc extended terms
double EnergyKernel::extendedEnergy(const Molecule &mol) const { return 0.0; }

/*
 * Totals
 */

// Return total interatomic PairPotential energy of the world
Kernel::PairPotentialEnergyValue EnergyKernel::totalPairPotentialEnergy(bool includeInter, bool includeIntra) const
{
    auto &cells = configuration_->cells();

    // List of cell neighbour pairs
    auto &cellNeighbourPairs = cells.getCellNeighbourPairs();
    return dissolve::transform_reduce(ParallelPolicies::par, cellNeighbourPairs.begin(), cellNeighbourPairs.end(),
                                      Kernel::PairPotentialEnergyValue(), std::plus<>(),
                                      [&](const auto &pair)
                                      {
                                          if (&pair.cell == &pair.neighbour)
                                              return cellEnergy(pair.cell, includeInter, includeIntra);
                                          else
                                              return cellToCellEnergy(pair.cell, pair.neighbour, pair.requiresMIM, includeInter,
                                                                      includeIntra);
                                      });
}

// Return total interatomic PairPotential energy from summation of molecules
Kernel::PairPotentialEnergyValue EnergyKernel::totalMoleculePairPotentialEnergy(bool includeInter, bool includeIntra) const
{
    auto &mols = configuration_->molecules();
    Kernel::PairPotentialEnergyValue molecularEnergy;
    for (const auto &mol : mols)
        molecularEnergy += pairPotentialEnergy(*mol, includeInter, includeIntra);

    // In the typical case where there is more than one molecule, our sum will contain double the intermolecular
    // pairpotential energy
    return {molecularEnergy.interMolecular * 0.5, molecularEnergy.intraMolecular};
}

// Return total energy of supplied atom
Kernel::EnergyResult EnergyKernel::totalEnergy(Flags<Kernel::CalculationFlags> flags) const
{
    return {totalPairPotentialEnergy(flags.isNotSet(Kernel::ExcludeInterMolecularPairPotential),
                                     flags.isNotSet(Kernel::ExcludeIntraMolecularPairPotential)),
            flags.isSet(Kernel::ExcludeGeometric) ? 0.0 : totalGeometryEnergy(configuration_),
            flags.isSet(Kernel::ExcludeExtended) ? 0.0 : totalExtendedEnergy()};
}

// Return total energy of supplied atom
Kernel::EnergyResult EnergyKernel::totalEnergy(const ConfigurationAtom &i) const
{
    return {{pairPotentialEnergy(i), 0.0}, geometryEnergy(i), extendedEnergy(i)};
}

// Return total energy of supplied molecule
Kernel::EnergyResult EnergyKernel::totalEnergy(const Molecule &mol, Flags<Kernel::CalculationFlags> flags) const
{
    return {pairPotentialEnergy(mol, flags.isNotSet(Kernel::ExcludeInterMolecularPairPotential),
                                flags.isNotSet(Kernel::ExcludeIntraMolecularPairPotential)),
            flags.isSet(Kernel::ExcludeGeometric) ? Kernel::GeometryEnergyValue() : geometryEnergy(mol),
            flags.isSet(Kernel::ExcludeExtended) ? 0.0 : extendedEnergy(mol)};
}

// Return potential map
const PotentialMap &EnergyKernel::potentialMap() const { return potentialMap_; }

// Calculate energy components with simple double-loops for testing
Kernel::EnergyResult EnergyKernel::totalEnergySimple() const
{
    Kernel::PairPotentialEnergyValue ppEnergy;
    Kernel::GeometryEnergyValue geometryEnergy;

    const auto cutoff = PairPotential::range();

    // Calculate interatomic energy in a loop over defined Molecules
    const auto molecules = configuration_->molecules();
    for (auto n = 0; n < molecules.size(); ++n)
    {
        auto molN = molecules[n];

        // Molecule self-energy
        for (auto ii = 0; ii < molN->nAtoms() - 1; ++ii)
        {
            auto i = molN->atom(ii);

            for (auto jj = ii + 1; jj < molN->nAtoms(); ++jj)
            {
                auto j = molN->atom(jj);

                // Get interatomic distance
                auto r = box_->minimumDistance(i->r(), j->r());
                if (r > cutoff)
                    continue;

                // Get intramolecular scaling of atom pair
                auto &&[scalingType, elec14, vdw14] = i->scaling(j);
                if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                    ppEnergy.intraMolecular += potentialMap_.analyticEnergy(*i, *j, r);
                else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                    ppEnergy.intraMolecular += potentialMap_.analyticEnergy(*i, *j, r, elec14, vdw14);
            }
        }

        // Molecule-molecule energy
        for (auto m = n + 1; m < molecules.size(); ++m)
        {
            auto molM = molecules[m];

            // Double loop over atoms
            for (auto ii = 0; ii < molN->nAtoms(); ++ii)
            {
                auto i = molN->atom(ii);

                for (auto jj = 0; jj < molM->nAtoms(); ++jj)
                {
                    auto j = molM->atom(jj);

                    // Get interatomic distance and check cutoff
                    auto r = box_->minimumDistance(i->r(), j->r());
                    if (r > cutoff)
                        continue;

                    ppEnergy.interMolecular += potentialMap_.analyticEnergy(*i, *j, r);
                }
            }
        }

        // Bond energy
        for (const auto &bond : molN->species()->bonds())
            geometryEnergy.bondEnergy +=
                bond.energy(box_->minimumDistance(molN->atom(bond.indexI())->r(), molN->atom(bond.indexJ())->r()));

        // Angle energy
        for (const auto &angle : molN->species()->angles())
        {
            geometryEnergy.angleEnergy += angle.energy(box_->angleInRadians(
                molN->atom(angle.indexI())->r(), molN->atom(angle.indexJ())->r(), molN->atom(angle.indexK())->r()));
        }

        // Torsion energy
        for (const auto &torsion : molN->species()->torsions())
        {
            geometryEnergy.torsionEnergy +=
                torsion.energy(box_->torsionInRadians(molN->atom(torsion.indexI())->r(), molN->atom(torsion.indexJ())->r(),
                                                      molN->atom(torsion.indexK())->r(), molN->atom(torsion.indexL())->r()));
        }

        // Improper energy
        for (const auto &imp : molN->species()->impropers())
        {
            geometryEnergy.improperEnergy +=
                imp.energy(box_->torsionInRadians(molN->atom(imp.indexI())->r(), molN->atom(imp.indexJ())->r(),
                                                  molN->atom(imp.indexK())->r(), molN->atom(imp.indexL())->r()));
        }
    }

    return {ppEnergy, geometryEnergy};
}
