// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/energykernel.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/molecule.h"
#include "classes/potentialmap.h"
#include "classes/species.h"
#include "templates/algorithms.h"
#include <iterator>
#include <numeric>

EnergyKernel::EnergyKernel(ProcessPool &procPool, const Box *box, const CellArray &cells, const PotentialMap &potentialMap,
                           std::optional<double> energyCutoff)
    : box_(box), cellArray_(cells), potentialMap_(potentialMap), processPool_(procPool)
{
    cutoffDistanceSquared_ =
        energyCutoff.has_value() ? energyCutoff.value() * energyCutoff.value() : potentialMap_.range() * potentialMap_.range();
}

/*
 * Base Routines
 */

// Return PairPotential energy between atoms
double EnergyKernel::pairPotentialEnergy(const Atom &i, const Atom &j, double r) { return potentialMap_.energy(i, j, r); }

// Return PairPotential energy between atoms
double EnergyKernel::energyWithoutMim(const Atom &i, const Atom &j)
{
    return pairPotentialEnergy(i, j, (i.r() - j.r()).magnitude());
}

// Return PairPotential energy between atoms provided
double EnergyKernel::energyWithMim(const Atom &i, const Atom &j)
{
    return pairPotentialEnergy(i, j, box_->minimumDistance(j.r(), i.r()));
}

/*
 * PairPotential Terms
 */

// Return PairPotential energy between atoms
double EnergyKernel::energy(const Atom &i, const Atom &j, bool applyMim, bool excludeIgeJ)
{
    // If Atoms are the same, we refuse to calculate
    if (&i == &j)
        return 0.0;

    // Check indices of Atoms if required
    if (excludeIgeJ && (i.arrayIndex() >= j.arrayIndex()))
        return 0.0;

    if (applyMim)
        return energyWithMim(i, j);
    else
        return energyWithoutMim(i, j);
}

// Return PairPotential energy of atoms in the supplied cell with all other cells
double EnergyKernel::energy(const Cell &cell, bool includeIntraMolecular)
{
    auto totalEnergy = 0.0;
    auto &atoms = cell.atoms();

    for (auto ii = atoms.begin(); ii != atoms.end(); ++ii)
    {
        auto molI = ii->molecule();
        auto &rI = ii->r();

        // Straight loop over other cell atoms
        for (auto jj = ii + 1; jj < atoms.end(); ++jj)
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
                auto scale = ii->scaling(&*jj);
                if (scale > 1.0e-3)
                    totalEnergy += pairPotentialEnergy(*ii, *jj, sqrt(rSq)) * scale;
            }
        }
    }

    return totalEnergy;
}

// Return PairPotential energy between atoms in supplied cells
double EnergyKernel::energy(const Cell &centralCell, const Cell &otherCell, bool applyMim, bool includeIntraMolecular)
{
    auto totalEnergy = 0.0;
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
                    totalEnergy += pairPotentialEnergy(ii, jj, sqrt(rSq));
                else if (includeIntraMolecular)
                {
                    auto scale = ii.scaling(&jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(ii, jj, sqrt(rSq)) * scale;
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
                    totalEnergy += pairPotentialEnergy(ii, jj, sqrt(rSq));
                else if (includeIntraMolecular)
                {
                    auto scale = ii.scaling(&jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(ii, jj, sqrt(rSq)) * scale;
                }
            }
        }
    }
    return totalEnergy;
}

// Return PairPotential energy of Atom with world
double EnergyKernel::energy(const Atom &i)
{
    // Get cell neighbours for atom i's cell
    auto &neighbours = cellArray_.neighbours(*i.cell());

    return dissolve::transform_reduce(
        ParallelPolicies::par, neighbours.begin(), neighbours.end(), 0.0, std::plus<double>(),
        [&i, this](const auto &neighbour) {
            auto mimRequired = neighbour.requiresMIM_;
            auto &nbrCellAtoms = neighbour.neighbour_.atoms();
            return std::accumulate(
                nbrCellAtoms.begin(), nbrCellAtoms.end(), 0.0, [&i, mimRequired, this](const auto innerAcc, const auto &jj) {
                    // Calculate rSquared distance between atoms, and check it against
                    // the stored cutoff distance
                    auto rSq = mimRequired ? box_->minimumDistanceSquared(i.r(), jj.r()) : (i.r() - jj.r()).magnitudeSq();
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
double EnergyKernel::energy(const Molecule &mol, ProcessPool::DivisionStrategy strategy)
{
    // Create a map of atoms in cells so we can treat all atoms with the same set of neighbours at once
    std::map<Cell *, std::vector<const Atom *>> locationMap;
    for (auto &i : mol.atoms())
        locationMap[i.cell()].push_back(&i);

    auto totalEnergy =
        std::accumulate(locationMap.begin(), locationMap.end(), 0.0, [&](const auto totalAcc, const auto &location) {
            const auto &centralCellAtoms = location.second;

            // Get cell neighbours for the cell
            auto &neighbours = cellArray_.neighbours(*location.first);

            auto localEnergy = dissolve::transform_reduce(
                ParallelPolicies::par, neighbours.begin(), neighbours.end(), 0.0, std::plus<double>(),
                [&centralCellAtoms, this](const auto &neighbour) {
                    return std::accumulate(
                        centralCellAtoms.begin(), centralCellAtoms.end(), 0.0,
                        [&neighbour, this](const auto acc, const auto &i) {
                            auto &ii = *i;
                            auto mimRequired = neighbour.requiresMIM_;
                            auto &nbrCellAtoms = neighbour.neighbour_.atoms();
                            return acc + std::accumulate(nbrCellAtoms.begin(), nbrCellAtoms.end(), 0.0,
                                                         [&ii, mimRequired, this](const auto innerAcc, const auto &jj) {
                                                             // Calculate rSquared distance between atoms, and check it against
                                                             // the stored cutoff distance
                                                             auto rSq = mimRequired
                                                                            ? box_->minimumDistanceSquared(ii.r(), jj.r())
                                                                            : (ii.r() - jj.r()).magnitudeSq();
                                                             if (rSq > cutoffDistanceSquared_)
                                                                 return innerAcc;

                                                             // Check for atoms in the same species
                                                             if (ii.molecule().get() != jj.molecule().get())
                                                                 return innerAcc + pairPotentialEnergy(ii, jj, sqrt(rSq));

                                                             return innerAcc;
                                                         });
                        });
                });

            return totalAcc + localEnergy;
        });

    return totalEnergy;
}

// Return molecular correction energy related to intramolecular terms involving supplied atom
double EnergyKernel::correct(const Atom &i)
{
    // Loop over atoms in molecule
    auto atoms = i.molecule()->atoms();
    const auto &rI = i.r();

    // double correctionEnergy = dissolve::transform_reduce(ParallelPolicies::par, atoms.begin(), atoms.end(), 0.0,
    double correctionEnergy =
        std::transform_reduce(atoms.begin(), atoms.end(), 0.0, std::plus<double>(), [&](auto &j) -> double {
            if (&i == &j)
                return 0.0;
            double scale = 1.0 - i.scaling(&j);
            if (scale <= 1.0e-3)
                return 0.0;
            double r = box_->minimumDistance(rI, j.r());
            return pairPotentialEnergy(i, j, r) * scale;
        });

    return -correctionEnergy;
}

// Return total interatomic PairPotential energy of the system
double EnergyKernel::energy(const CellArray &cellArray, bool includeIntraMolecular, ProcessPool::DivisionStrategy strategy)
{
    // List of cell neighbour pairs
    auto &cellNeighbourPairs = cellArray.getCellNeighbourPairs();

    // Set start/stride for parallel loop
    auto offset = processPool_.interleavedLoopStart(strategy);
    auto nChunks = processPool_.interleavedLoopStride(strategy);

    auto totalEnergy = 0.0;
    auto [begin, end] = chop_range(cellNeighbourPairs.begin(), cellNeighbourPairs.end(), nChunks, offset);

    totalEnergy +=
        dissolve::transform_reduce(ParallelPolicies::par, begin, end, 0.0, std::plus<double>(), [&](const auto &pair) {
            auto &cellI = pair.master_;
            auto &cellJ = pair.neighbour_;
            auto mimRequired = pair.requiresMIM_;
            if (&cellI == &cellJ)
                return energy(cellI, includeIntraMolecular);
            else
                return energy(cellI, cellJ, mimRequired, includeIntraMolecular);
        });

    return totalEnergy;
}

/*
 * Intramolecular Terms
 */

// Return SpeciesBond energy at Atoms specified
double EnergyKernel::energy(const SpeciesBond &b, const Atom &i, const Atom &j)
{
    return b.energy(box_->minimumDistance(i.r(), j.r()));
}

// Return SpeciesAngle energy at Atoms specified
double EnergyKernel::energy(const SpeciesAngle &a, const Atom &i, const Atom &j, const Atom &k)
{
    return a.energy(Box::angleInDegrees(box_->minimumVectorN(j.r(), i.r()), box_->minimumVectorN(j.r(), k.r())));
}

// Return SpeciesTorsion energy at Atoms specified
double EnergyKernel::energy(const SpeciesTorsion &t, const Atom &i, const Atom &j, const Atom &k, const Atom &l)
{
    return t.energy(Box::torsionInDegrees(box_->minimumVector(j.r(), i.r()), box_->minimumVector(j.r(), k.r()),
                                          box_->minimumVector(k.r(), l.r())));
}

// Return SpeciesImproper energy at Atoms specified
double EnergyKernel::energy(const SpeciesImproper &imp, const Atom &i, const Atom &j, const Atom &k, const Atom &l)
{
    return imp.energy(Box::torsionInDegrees(box_->minimumVector(j.r(), i.r()), box_->minimumVector(j.r(), k.r()),
                                            box_->minimumVector(k.r(), l.r())));
}

// Return intramolecular energy for the supplied Atom
double EnergyKernel::intramolecularEnergy(const Molecule &mol, const Atom &i)
{
    // Get the SpeciesAtom
    const auto *spAtom = i.speciesAtom();
    assert(spAtom);

    // If no terms are present, return zero
    if ((spAtom->nBonds() == 0) && (spAtom->nAngles() == 0) && (spAtom->nTorsions() == 0))
        return 0.0;

    auto intraEnergy = 0.0;

    // Add energy from SpeciesAngle terms
    intraEnergy += std::accumulate(spAtom->bonds().begin(), spAtom->bonds().end(), 0.0,
                                   [this, &mol](const auto acc, const SpeciesBond &bond) {
                                       return acc + energy(bond, mol.atom(bond.indexI()), mol.atom(bond.indexJ()));
                                   });

    // Add energy from SpeciesAngle terms
    intraEnergy += std::accumulate(
        spAtom->angles().begin(), spAtom->angles().end(), 0.0, [this, &mol](const auto acc, const SpeciesAngle &angle) {
            return acc + energy(angle, mol.atom(angle.indexI()), mol.atom(angle.indexJ()), mol.atom(angle.indexK()));
        });

    // Add energy from SpeciesTorsion terms
    intraEnergy += std::accumulate(spAtom->torsions().begin(), spAtom->torsions().end(), 0.0,
                                   [this, &mol](const auto acc, const SpeciesTorsion &torsion) {
                                       return acc + energy(torsion, mol.atom(torsion.indexI()), mol.atom(torsion.indexJ()),
                                                           mol.atom(torsion.indexK()), mol.atom(torsion.indexL()));
                                   });

    // Add energy from SpeciesImproper terms
    intraEnergy += std::accumulate(spAtom->impropers().begin(), spAtom->impropers().end(), 0.0,
                                   [this, &mol](const auto acc, const SpeciesImproper &improper) {
                                       return acc + energy(improper, mol.atom(improper.indexI()), mol.atom(improper.indexJ()),
                                                           mol.atom(improper.indexK()), mol.atom(improper.indexL()));
                                   });

    return intraEnergy;
}

// Return intramolecular energy for the supplied Molecule
double EnergyKernel::intramolecularEnergy(const Molecule &mol)
{
    auto intraEnergy = 0.0;

    // Loop over Bonds
    intraEnergy = dissolve::transform_reduce(
        ParallelPolicies::par, mol.species()->bonds().begin(), mol.species()->bonds().end(), intraEnergy, std::plus<double>(),
        [&mol, this](const auto &bond) { return energy(bond, mol.atom(bond.indexI()), mol.atom(bond.indexJ())); });

    // Loop over Angles
    intraEnergy = dissolve::transform_reduce(
        ParallelPolicies::seq, mol.species()->angles().begin(), mol.species()->angles().end(), intraEnergy, std::plus<double>(),
        [&mol, this](const auto &angle) -> double {
            return energy(angle, mol.atom(angle.indexI()), mol.atom(angle.indexJ()), mol.atom(angle.indexK()));
        });

    // Loop over Torsions
    intraEnergy =
        dissolve::transform_reduce(ParallelPolicies::par, mol.species()->torsions().begin(), mol.species()->torsions().end(),
                                   intraEnergy, std::plus<double>(), [&mol, this](const auto &torsion) -> double {
                                       return energy(torsion, mol.atom(torsion.indexI()), mol.atom(torsion.indexJ()),
                                                     mol.atom(torsion.indexK()), mol.atom(torsion.indexL()));
                                   });

    // Loop over Impropers
    intraEnergy =
        dissolve::transform_reduce(ParallelPolicies::par, mol.species()->impropers().begin(), mol.species()->impropers().end(),
                                   intraEnergy, std::plus<double>(), [&mol, this](const auto &improper) -> double {
                                       return energy(improper, mol.atom(improper.indexI()), mol.atom(improper.indexJ()),
                                                     mol.atom(improper.indexK()), mol.atom(improper.indexL()));
                                   });

    return intraEnergy;
}
