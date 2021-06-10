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

EnergyKernel::EnergyKernel(ProcessPool &procPool, const Configuration *cfg, const PotentialMap &potentialMap,
                           double energyCutoff)
    : configuration_(cfg), cells_(cfg->cells()), potentialMap_(potentialMap), processPool_(procPool)
{
    box_ = configuration_->box();
    cutoffDistanceSquared_ = (energyCutoff < 0.0 ? potentialMap_.range() * potentialMap_.range() : energyCutoff * energyCutoff);
}

/*
 * Internal Routines
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
    return pairPotentialEnergy(i, j, box_->minimumDistance(j, i));
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

// Return PairPotential energy between atoms in supplied cells
double EnergyKernel::energy(const Cell &centralCell, const Cell &otherCell, bool applyMim, bool interMolecular)
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
                else if (!interMolecular)
                {
                    double scale = ii->scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(*ii, *jj, sqrt(rSq)) * scale;
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

                else if (!interMolecular)
                {
                    double scale = ii->scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(*ii, *jj, sqrt(rSq)) * scale;
                }
            }
        }
    }
    return totalEnergy;
}

// Return PairPotential energy between atoms in supplied cell
double EnergyKernel::energy(const Cell &cell, bool interMolecular)
{
    auto totalEnergy = 0.0;
    auto &atoms = cell.atoms();

    for (int i = 0; i < atoms.size(); ++i)
    {
        auto &ii = atoms[i];
        auto molI = ii->molecule();
        auto &rI = ii->r();

        // Straight loop over other cell atoms
        for (int j = i + 1; j < atoms.size(); ++j)
        {
            // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
            auto &jj = atoms[j];

            double rSq = (rI - jj->r()).magnitudeSq();
            if (rSq > cutoffDistanceSquared_)
                continue;

            // Check for atoms in the same molecule
            if (molI != jj->molecule())
                totalEnergy += pairPotentialEnergy(*ii, *jj, sqrt(rSq));
            else if (!interMolecular)
            {
                double scale = ii->scaling(jj);
                if (scale > 1.0e-3)
                    totalEnergy += pairPotentialEnergy(*ii, *jj, sqrt(rSq)) * scale;
            }
        }
    }
    return totalEnergy;
}

// Return PairPotential energy between Atom and Cell contents
double EnergyKernel::energy(const Atom &i, const Cell *cell, int flags, ProcessPool::DivisionStrategy strategy, bool performSum)
{
    assert(cell);

    auto totalEnergy = 0.0;
    Atom *jj;
    double rSq, scale;
    auto &otherAtoms = cell->atoms();

    // Grab some information on the supplied Atom
    auto moleculeI = i.molecule();
    const auto rI = i.r();

    // Get start/stride for specified loop context
    auto offset = processPool_.interleavedLoopStart(strategy);
    auto nChunks = processPool_.interleavedLoopStride(strategy);

    if (flags & KernelFlags::ApplyMinimumImageFlag)
    {
        auto [begin, end] = chop_range(otherAtoms.begin(), otherAtoms.end(), nChunks, offset);
        // Loop over other Atoms
        if (flags & KernelFlags::ExcludeSelfFlag)
            for (auto indexJ = begin; indexJ < end; ++indexJ)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Check for same atom
                if (&i == jj)
                    continue;

                // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
                rSq = box_->minimumDistanceSquared(rI, jj->r());
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    totalEnergy += pairPotentialEnergy(i, *jj, sqrt(rSq));
                else
                {
                    scale = i.scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(i, *jj, sqrt(rSq)) * scale;
                }
            }
        else if (flags & KernelFlags::ExcludeIGEJFlag)
            for (auto indexJ = begin; indexJ < end; ++indexJ)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Check for i >= jj
                if (i.arrayIndex() >= jj->arrayIndex())
                    continue;

                // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
                rSq = box_->minimumDistanceSquared(rI, jj->r());
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    totalEnergy += pairPotentialEnergy(i, *jj, sqrt(rSq));
                else
                {
                    scale = i.scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(i, *jj, sqrt(rSq)) * scale;
                }
            }
        else if (flags & KernelFlags::ExcludeIntraIGEJFlag)
            for (auto indexJ = begin; indexJ < end; ++indexJ)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
                rSq = box_->minimumDistanceSquared(rI, jj->r());
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    totalEnergy += pairPotentialEnergy(i, *jj, sqrt(rSq));
                else
                {
                    // Check for i >= jj
                    if (i.arrayIndex() >= jj->arrayIndex())
                        continue;

                    scale = i.scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(i, *jj, sqrt(rSq)) * scale;
                }
            }
        else
            for (auto indexJ = begin; indexJ < end; ++indexJ)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
                rSq = box_->minimumDistanceSquared(rI, jj->r());
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    totalEnergy += pairPotentialEnergy(i, *jj, sqrt(rSq));
                else
                {
                    scale = i.scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(i, *jj, sqrt(rSq)) * scale;
                }
            }
    }
    else
    {
        // Loop over atom neighbours
        auto [begin, end] = chop_range(otherAtoms.begin(), otherAtoms.end(), nChunks, offset);
        if (flags & KernelFlags::ExcludeSelfFlag)
            for (auto indexJ = begin; indexJ < end; ++indexJ)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Check for same atom
                if (&i == jj)
                    continue;

                // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
                rSq = (rI - jj->r()).magnitudeSq();
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    totalEnergy += pairPotentialEnergy(i, *jj, sqrt(rSq));
                else
                {
                    scale = i.scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(i, *jj, sqrt(rSq)) * scale;
                }
            }
        else if (flags & KernelFlags::ExcludeIGEJFlag)
            for (auto indexJ = begin; indexJ < end; ++indexJ)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Check for i >= jj
                if (i.arrayIndex() >= jj->arrayIndex())
                    continue;

                // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
                rSq = (rI - jj->r()).magnitudeSq();
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    totalEnergy += pairPotentialEnergy(i, *jj, sqrt(rSq));
                else
                {
                    scale = i.scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(i, *jj, sqrt(rSq)) * scale;
                }
            }
        else if (flags & KernelFlags::ExcludeIntraIGEJFlag)
            for (auto indexJ = begin; indexJ < end; ++indexJ)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
                rSq = (rI - jj->r()).magnitudeSq();
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    totalEnergy += pairPotentialEnergy(i, *jj, sqrt(rSq));
                else
                {
                    // Check for i >= jj
                    if (i.arrayIndex() >= jj->arrayIndex())
                        continue;

                    scale = i.scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(i, *jj, sqrt(rSq)) * scale;
                }
            }
        else
            for (auto indexJ = begin; indexJ < end; ++indexJ)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
                rSq = (rI - jj->r()).magnitudeSq();
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    totalEnergy += pairPotentialEnergy(i, *jj, sqrt(rSq));
                else
                {
                    scale = i.scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(i, *jj, sqrt(rSq)) * scale;
                }
            }
    }

    // Perform relevant sum if requested
    if (performSum)
        processPool_.allSum(&totalEnergy, 1, strategy);

    return totalEnergy;
}

// Return PairPotential energy of Atom with world
double EnergyKernel::energy(const Atom &i)
{
    // Get cell neighbours for atom i's cell
    auto &neighbours = cells_.neighbours(*i.cell());

    return dissolve::transform_reduce(
        ParallelPolicies::par, neighbours.begin(), neighbours.end(), 0.0, std::plus<double>(),
        [&i, this](const auto &neighbour) {
            auto mimRequired = neighbour.requiresMIM_;
            auto &nbrCellAtoms = neighbour.neighbour_.atoms();
            return std::accumulate(
                nbrCellAtoms.begin(), nbrCellAtoms.end(), 0.0, [&i, mimRequired, this](const auto innerAcc, const auto *j) {
                    auto &jj = *j;

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
double EnergyKernel::energy(const Molecule &mol, ProcessPool::DivisionStrategy strategy, bool performSum)
{
    // Create a map of atoms in cells so we can treat all atoms with the same set of neighbours at once
    std::map<Cell *, std::vector<const Atom *>> locationMap;
    for (auto &i : mol.atoms())
        locationMap[i->cell()].push_back(i.get());

    auto totalEnergy =
        std::accumulate(locationMap.begin(), locationMap.end(), 0.0, [&](const auto totalAcc, const auto &location) {
            const auto &centralCellAtoms = location.second;

            // Get cell neighbours for the cell
            auto &neighbours = cells_.neighbours(*location.first);

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
                                                         [&ii, mimRequired, this](const auto innerAcc, const auto *j) {
                                                             auto &jj = *j;

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

    // Perform relevant sum if requested
    if (performSum)
        processPool_.allSum(&totalEnergy, 1, strategy);

    return totalEnergy;
}

// Return molecular correction energy related to intramolecular terms involving supplied atom
double EnergyKernel::correct(const Atom &i)
{
    // Loop over atoms in molecule
    auto &atoms = i.molecule()->atoms();
    const auto &rI = i.r();

    double correctionEnergy = dissolve::transform_reduce(ParallelPolicies::par, atoms.begin(), atoms.end(), 0.0,
                                                         std::plus<double>(), [&](auto &j) -> double {
                                                             if (&i == j.get())
                                                                 return 0.0;
                                                             double scale = 1.0 - i.scaling(j.get());
                                                             if (scale <= 1.0e-3)
                                                                 return 0.0;
                                                             double r = box_->minimumDistance(rI, j->r());
                                                             return pairPotentialEnergy(i, *j, r) * scale;
                                                         });

    return -correctionEnergy;
}

// Return total interatomic PairPotential energy of the system
double EnergyKernel::energy(const CellArray &cellArray, bool interMolecular, ProcessPool::DivisionStrategy strategy,
                            bool performSum)
{
    // Get sub-strategy to use
    ProcessPool::DivisionStrategy subStrategy = ProcessPool::subDivisionStrategy(strategy);

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
                return energy(cellI, interMolecular);
            else
                return energy(cellI, cellJ, mimRequired, interMolecular);
        });

    // Perform relevant sum if requested
    if (performSum)
        processPool_.allSum(&totalEnergy, 1, strategy);

    return totalEnergy;
}

/*
 * Intramolecular Terms
 */

// Return SpeciesBond energy at Atoms specified
double EnergyKernel::energy(const SpeciesBond &bond, const Atom &i, const Atom &j)
{
    // Determine whether we need to apply minimum image to the distance calculation
    if (i.cell()->mimRequired(j.cell()))
        return bond.energy(box_->minimumDistance(i, j));
    else
        return bond.energy((i.r() - j.r()).magnitude());
}

// Return SpeciesBond energy
double EnergyKernel::energy(const SpeciesBond &b) { return b.energy((b.j()->r() - b.i()->r()).magnitude()); }

// Return SpeciesAngle energy at Atoms specified
double EnergyKernel::energy(const SpeciesAngle &angle, const Atom &i, const Atom &j, const Atom &k)
{
    Vec3<double> vecji, vecjk;

    // Determine whether we need to apply minimum image between 'j-i' and 'j-k'
    if (j.cell()->mimRequired(i.cell()))
        vecji = box_->minimumVector(j, i);
    else
        vecji = i.r() - j.r();
    if (j.cell()->mimRequired(k.cell()))
        vecjk = box_->minimumVector(j, k);
    else
        vecjk = k.r() - j.r();

    // Normalise vectors
    vecji.normalise();
    vecjk.normalise();

    // Determine Angle energy
    return angle.energy(Box::angleInDegrees(vecji, vecjk));
}

// Return SpeciesAngle energy
double EnergyKernel::energy(const SpeciesAngle &angle)
{
    auto vecji = angle.i()->r() - angle.j()->r(), vecjk = angle.k()->r() - angle.j()->r();

    // Normalise vectors
    vecji.normalise();
    vecjk.normalise();

    // Determine Angle energy
    return angle.energy(Box::angleInDegrees(vecji, vecjk));
}

// Return SpeciesTorsion energy at Atoms specified
double EnergyKernel::energy(const SpeciesTorsion &torsion, const Atom &i, const Atom &j, const Atom &k, const Atom &l)
{
    Vec3<double> vecji, vecjk, veckl;

    // Calculate vectors, ensuring we account for minimum image
    if (j.cell()->mimRequired(i.cell()))
        vecji = box_->minimumVector(j, i);
    else
        vecji = i.r() - j.r();
    if (j.cell()->mimRequired(k.cell()))
        vecjk = box_->minimumVector(j, k);
    else
        vecjk = k.r() - j.r();
    if (k.cell()->mimRequired(l.cell()))
        veckl = box_->minimumVector(k, l);
    else
        veckl = l.r() - k.r();

    return torsion.energy(Box::torsionInDegrees(vecji, vecjk, veckl));
}

// Return SpeciesTorsion energy
double EnergyKernel::energy(const SpeciesTorsion &torsion)
{
    return torsion.energy(Box::torsionInDegrees(torsion.i()->r() - torsion.j()->r(), torsion.k()->r() - torsion.j()->r(),
                                                torsion.l()->r() - torsion.k()->r()));
}

// Return SpeciesImproper energy at Atoms specified
double EnergyKernel::energy(const SpeciesImproper &imp, const Atom &i, const Atom &j, const Atom &k, const Atom &l)
{
    Vec3<double> vecji, vecjk, veckl;

    // Calculate vectors, ensuring we account for minimum image
    if (j.cell()->mimRequired(i.cell()))
        vecji = box_->minimumVector(j, i);
    else
        vecji = i.r() - j.r();
    if (j.cell()->mimRequired(k.cell()))
        vecjk = box_->minimumVector(j, k);
    else
        vecjk = k.r() - j.r();
    if (k.cell()->mimRequired(l.cell()))
        veckl = box_->minimumVector(k, l);
    else
        veckl = l.r() - k.r();

    return imp.energy(Box::torsionInDegrees(vecji, vecjk, veckl));
}

// Return SpeciesImproper energy
double EnergyKernel::energy(const SpeciesImproper &imp)
{
    return imp.energy(
        Box::torsionInDegrees(imp.i()->r() - imp.j()->r(), imp.k()->r() - imp.j()->r(), imp.l()->r() - imp.k()->r()));
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
                                       return acc + energy(bond, *mol.atom(bond.indexI()), *mol.atom(bond.indexJ()));
                                   });

    // Add energy from SpeciesAngle terms
    intraEnergy += std::accumulate(
        spAtom->angles().begin(), spAtom->angles().end(), 0.0, [this, &mol](const auto acc, const SpeciesAngle &angle) {
            return acc + energy(angle, *mol.atom(angle.indexI()), *mol.atom(angle.indexJ()), *mol.atom(angle.indexK()));
        });

    // Add energy from SpeciesTorsion terms
    intraEnergy += std::accumulate(spAtom->torsions().begin(), spAtom->torsions().end(), 0.0,
                                   [this, &mol](const auto acc, const SpeciesTorsion &torsion) {
                                       return acc + energy(torsion, *mol.atom(torsion.indexI()), *mol.atom(torsion.indexJ()),
                                                           *mol.atom(torsion.indexK()), *mol.atom(torsion.indexL()));
                                   });

    // Add energy from SpeciesImproper terms
    intraEnergy += std::accumulate(spAtom->impropers().begin(), spAtom->impropers().end(), 0.0,
                                   [this, &mol](const auto acc, const SpeciesImproper &improper) {
                                       return acc + energy(improper, *mol.atom(improper.indexI()), *mol.atom(improper.indexJ()),
                                                           *mol.atom(improper.indexK()), *mol.atom(improper.indexL()));
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
        [&mol, this](const auto &bond) { return energy(bond, *mol.atom(bond.indexI()), *mol.atom(bond.indexJ())); });

    // Loop over Angles
    intraEnergy = dissolve::transform_reduce(
        ParallelPolicies::seq, mol.species()->angles().begin(), mol.species()->angles().end(), intraEnergy, std::plus<double>(),
        [&mol, this](const auto &angle) -> double {
            return energy(angle, *mol.atom(angle.indexI()), *mol.atom(angle.indexJ()), *mol.atom(angle.indexK()));
        });

    // Loop over Torsions
    intraEnergy =
        dissolve::transform_reduce(ParallelPolicies::par, mol.species()->torsions().begin(), mol.species()->torsions().end(),
                                   intraEnergy, std::plus<double>(), [&mol, this](const auto &torsion) -> double {
                                       return energy(torsion, *mol.atom(torsion.indexI()), *mol.atom(torsion.indexJ()),
                                                     *mol.atom(torsion.indexK()), *mol.atom(torsion.indexL()));
                                   });

    // Loop over Impropers
    intraEnergy =
        dissolve::transform_reduce(ParallelPolicies::par, mol.species()->impropers().begin(), mol.species()->impropers().end(),
                                   intraEnergy, std::plus<double>(), [&mol, this](const auto &improper) -> double {
                                       return energy(improper, *mol.atom(improper.indexI()), *mol.atom(improper.indexJ()),
                                                     *mol.atom(improper.indexK()), *mol.atom(improper.indexL()));
                                   });

    return intraEnergy;
}
