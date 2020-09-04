/*
    *** EnergyKernel
    *** src/classes/energykernel.cpp
    Copyright T. Youngs 2012-2020

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

#include "classes/energykernel.h"
#include "base/processpool.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/molecule.h"
#include "classes/potentialmap.h"
#include "classes/species.h"
#include <iterator>
#include <numeric>

EnergyKernel::EnergyKernel(ProcessPool &procPool, Configuration *config, const PotentialMap &potentialMap, double energyCutoff)
    : configuration_(config), cells_(config->cells()), potentialMap_(potentialMap), processPool_(procPool)
{
    box_ = configuration_->box();
    cutoffDistanceSquared_ = (energyCutoff < 0.0 ? potentialMap_.range() * potentialMap_.range() : energyCutoff * energyCutoff);
}

EnergyKernel::~EnergyKernel() {}

/*
 * Internal Routines
 */

// Return PairPotential energy between atoms provided as pointers, at the distance specified
double EnergyKernel::pairPotentialEnergy(const Atom *i, const Atom *j, double r) { return potentialMap_.energy(i, j, r); }

// Return PairPotential energy between atoms provided as pointers (no minimum image calculation)
double EnergyKernel::energyWithoutMim(const Atom *i, const Atom *j)
{
    return pairPotentialEnergy(i, j, (i->r() - j->r()).magnitude());
}

// Return PairPotential energy between atoms provided as pointers (minimum image calculation)
double EnergyKernel::energyWithMim(const Atom *i, const Atom *j)
{
    // 	Messenger::print("EnergyKernel::atoms(*,*) - energy {}-{} is {} at {} mim\n",
    // min(i->arrayIndex(),j->arrayIndex()), max(i->arrayIndex(),j->arrayIndex()), pairPotentialEnergy(i->masterTypeIndex(),
    // j->masterTypeIndex(), box_->minimumDistance(j, i)), box_->minimumDistance(j, i));
    return pairPotentialEnergy(i, j, box_->minimumDistance(j, i));
}

/*
 * PairPotential Terms
 */

// Return PairPotential energy between atoms (provided as pointers)
double EnergyKernel::energy(const Atom *i, const Atom *j, bool applyMim, bool excludeIgeJ)
{
#ifdef CHECKS
    if (i == nullptr)
    {
        Messenger::error("NULL_POINTER - nullptr (i) passed to EnergyKernel::energy(Atom,Atom,bool,bool).\n");
        return 0.0;
    }
    if (j == nullptr)
    {
        Messenger::error("NULL_POINTER - nullptr (j) passed to EnergyKernel::energy(Atom,Atom,bool,bool).\n");
        return 0.0;
    }
#endif
    // If Atoms are the same, we refuse to calculate
    if (i == j)
        return 0.0;

    // Check indices of Atoms if required
    if (excludeIgeJ && (i->arrayIndex() >= j->arrayIndex()))
        return 0.0;

    if (applyMim)
        return energyWithMim(i, j);
    else
        return energyWithoutMim(i, j);
}

// Return PairPotential energy between atoms in supplied cells
double EnergyKernel::energy(Cell *centralCell, Cell *otherCell, bool applyMim, bool excludeIgeJ, bool interMolecular,
                            ProcessPool::DivisionStrategy strategy, bool performSum)
{
#ifdef CHECKS
    if (centralCell == nullptr)
    {
        Messenger::error(
            "NULL_POINTER - nullptr (central cell) pointer passed to EnergyKernel::energy(Cell,Cell,bool,bool).\n");
        return 0.0;
    }
    if (otherCell == nullptr)
    {
        Messenger::error("NULL_POINTER - nullptr (other cell) pointer passed to EnergyKernel::energy(Cell,Cell,bool,bool).\n");
        return 0.0;
    }
#endif
    double totalEnergy = 0.0;
    auto &centralAtoms = centralCell->atoms();
    auto &otherAtoms = otherCell->atoms();
    Atom *ii, *jj;
    Vec3<double> rI;
    std::shared_ptr<Molecule> molI;
    double rSq, scale;
    auto central = centralAtoms.begin();

    // Get start/stride for specified loop context
    auto start = processPool_.interleavedLoopStart(strategy);
    auto stride = processPool_.interleavedLoopStride(strategy);

    // Loop over central cell atoms
    if (applyMim)
    {
        for (auto indexI = centralAtoms.begin() + start; indexI < centralAtoms.end(); indexI += stride)
        {
            ii = *indexI;
            molI = ii->molecule();
            rI = ii->r();

            // Straight loop over other cell atoms
            for (auto *jj : otherAtoms)
            {
                // Check exclusion of I >= J
                if (excludeIgeJ && (ii >= jj))
                    continue;

                // Calculate rSquared distance betwenn atoms, and check it against the stored cutoff distance
                rSq = box_->minimumDistanceSquared(rI, jj->r());
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (molI != jj->molecule())
                    totalEnergy += pairPotentialEnergy(ii, jj, sqrt(rSq));
                else if (!interMolecular)
                {
                    scale = ii->scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(ii, jj, sqrt(rSq)) * scale;
                }
            }
        }
    }
    else
    {
        for (auto indexI = centralCell->atoms().begin() + start; indexI < centralCell->atoms().end(); indexI += stride)
        {
            ii = *indexI;
            molI = ii->molecule();
            rI = ii->r();

            // Straight loop over other cell atoms
            for (auto *jj : otherAtoms)
            {
                // Check exclusion of I >= J
                if (excludeIgeJ && (ii >= jj))
                    continue;

                // Calculate rSquared distance betwenn atoms, and check it against the stored cutoff distance
                rSq = (rI - jj->r()).magnitudeSq();
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same molecule
                if (molI != jj->molecule())
                    totalEnergy += pairPotentialEnergy(ii, jj, sqrt(rSq));
                else if (!interMolecular)
                {
                    scale = ii->scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(ii, jj, sqrt(rSq)) * scale;
                }
            }
        }
    }

    // Perform relevant sum if requested
    if (performSum)
        processPool_.allSum(&totalEnergy, 1, strategy);

    return totalEnergy;
}

// Return PairPotential energy between cell and atomic neighbours
double EnergyKernel::energy(Cell *centralCell, bool excludeIgeJ, bool interMolecular, ProcessPool::DivisionStrategy strategy,
                            bool performSum)
{
    double totalEnergy = 0.0;
    auto &centralAtoms = centralCell->atoms();
    Atom *ii, *jj;
    Vec3<double> rJ;
    std::shared_ptr<Molecule> molJ;
    double rSq, scale;

    // Get start/stride for specified loop context
    auto start = processPool_.interleavedLoopStart(strategy);
    auto stride = processPool_.interleavedLoopStride(strategy);

    // Straight loop over Cells *not* requiring mim
    for (auto *otherCell : centralCell->cellNeighbours())
    {
        auto &otherAtoms = otherCell->atoms();

        for (auto *jj : otherAtoms)
        {
            molJ = jj->molecule();
            rJ = jj->r();

            auto central = centralAtoms.begin();
            // Loop over central cell atoms
            for (auto indexI = centralAtoms.begin() + start; indexI < centralAtoms.end(); indexI += stride)
            {
                ii = *indexI;

                // Check exclusion of I >= J (comparison by pointer)
                if (excludeIgeJ && (ii >= jj))
                    continue;

                // Calculate rSquared distance betwenn atoms, and check it against the stored cutoff distance
                rSq = (ii->r() - rJ).magnitudeSq();
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (ii->molecule() != molJ)
                    totalEnergy += pairPotentialEnergy(jj, ii, sqrt(rSq));
                else if (!interMolecular)
                {
                    scale = ii->scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(jj, ii, sqrt(rSq)) * scale;
                }
            }
        }
    }

    // Straight loop over Cells requiring mim
    for (auto *otherCell : centralCell->mimCellNeighbours())
    {
        auto &otherAtoms = otherCell->atoms();

        for (auto *jj : otherAtoms)
        {
            molJ = jj->molecule();
            rJ = jj->r();

            auto central = centralAtoms.begin();
            // Loop over central cell atoms
            for (auto indexI = centralAtoms.begin() + start; indexI < centralAtoms.end(); indexI += stride)
            {
                ii = *indexI;

                // Check exclusion of I >= J (comparison by pointer)
                if (excludeIgeJ && (ii >= jj))
                    continue;

                // Calculate rSquared distance betwenn atoms, and check it against the stored cutoff distance
                rSq = box_->minimumDistanceSquared(ii->r(), rJ);
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (ii->molecule() != molJ)
                    totalEnergy += pairPotentialEnergy(jj, ii, sqrt(rSq));
                else if (!interMolecular)
                {
                    scale = ii->scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(jj, ii, sqrt(rSq)) * scale;
                }
            }
        }
    }

    // Perform relevant sum if requested
    if (performSum)
        processPool_.allSum(&totalEnergy, 1, strategy);

    return totalEnergy;
}

// Return PairPotential energy between Atom and Cell contents
double EnergyKernel::energy(const Atom *i, const Cell *cell, int flags, ProcessPool::DivisionStrategy strategy, bool performSum)
{
#ifdef CHECKS
    if (i == nullptr)
    {
        Messenger::error("NULL_POINTER - NULL atom pointer passed to EnergyKernel::energy(Atom,Cell).\n");
        return 0.0;
    }
    if (cell == nullptr)
    {
        Messenger::error("NULL_POINTER - nullptr passed to EnergyKernel::energy(Atom,Cell).\n");
        return 0.0;
    }
#endif
    double totalEnergy = 0.0;
    Atom *jj;
    int j;
    double rSq, scale;
    auto &otherAtoms = cell->atoms();
    auto other = otherAtoms.begin();
    auto nOtherAtoms = cell->nAtoms();

    // Grab some information on the supplied Atom
    std::shared_ptr<Molecule> moleculeI = i->molecule();
    const auto rI = i->r();

    // Get start/stride for specified loop context
    auto start = processPool_.interleavedLoopStart(strategy);
    auto stride = processPool_.interleavedLoopStride(strategy);

    if (flags & KernelFlags::ApplyMinimumImageFlag)
    {
        // Loop over other Atoms
        if (flags & KernelFlags::ExcludeSelfFlag)
            for (auto indexJ = otherAtoms.begin() + start; indexJ < otherAtoms.end(); indexJ += stride)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Check for same atom
                if (i == jj)
                    continue;

                // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
                rSq = box_->minimumDistanceSquared(rI, jj->r());
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    totalEnergy += pairPotentialEnergy(i, jj, sqrt(rSq));
                else
                {
                    scale = i->scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(i, jj, sqrt(rSq)) * scale;
                }
            }
        else if (flags & KernelFlags::ExcludeIGEJFlag)
            for (auto indexJ = otherAtoms.begin() + start; indexJ < otherAtoms.end(); indexJ += stride)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Pointer comparison for i >= jj
                if (i >= jj)
                    continue;

                // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
                rSq = box_->minimumDistanceSquared(rI, jj->r());
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    totalEnergy += pairPotentialEnergy(i, jj, sqrt(rSq));
                else
                {
                    scale = i->scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(i, jj, sqrt(rSq)) * scale;
                }
            }
        else if (flags & KernelFlags::ExcludeIntraIGEJFlag)
            for (auto indexJ = otherAtoms.begin() + start; indexJ < otherAtoms.end(); indexJ += stride)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
                rSq = box_->minimumDistanceSquared(rI, jj->r());
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    totalEnergy += pairPotentialEnergy(i, jj, sqrt(rSq));
                else
                {
                    // Pointer comparison for i >= jj
                    if (i >= jj)
                        continue;

                    scale = i->scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(i, jj, sqrt(rSq)) * scale;
                }
            }
        else
            for (auto indexJ = otherAtoms.begin() + start; indexJ < otherAtoms.end(); indexJ += stride)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
                rSq = box_->minimumDistanceSquared(rI, jj->r());
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    totalEnergy += pairPotentialEnergy(i, jj, sqrt(rSq));
                else
                {
                    scale = i->scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(i, jj, sqrt(rSq)) * scale;
                }
            }
    }
    else
    {
        // Loop over atom neighbours
        if (flags & KernelFlags::ExcludeSelfFlag)
            for (auto indexJ = otherAtoms.begin() + start; indexJ < otherAtoms.end(); indexJ += stride)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Check for same atom
                if (i == jj)
                    continue;

                // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
                rSq = (rI - jj->r()).magnitudeSq();
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    totalEnergy += pairPotentialEnergy(i, jj, sqrt(rSq));
                else
                {
                    scale = i->scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(i, jj, sqrt(rSq)) * scale;
                }
            }
        else if (flags & KernelFlags::ExcludeIGEJFlag)
            for (auto indexJ = otherAtoms.begin() + start; indexJ < otherAtoms.end(); indexJ += stride)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Pointer comparison for i >= jj
                if (i >= jj)
                    continue;

                // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
                rSq = (rI - jj->r()).magnitudeSq();
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    totalEnergy += pairPotentialEnergy(i, jj, sqrt(rSq));
                else
                {
                    scale = i->scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(i, jj, sqrt(rSq)) * scale;
                }
            }
        else if (flags & KernelFlags::ExcludeIntraIGEJFlag)
            for (auto indexJ = otherAtoms.begin() + start; indexJ < otherAtoms.end(); indexJ += stride)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
                rSq = (rI - jj->r()).magnitudeSq();
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    totalEnergy += pairPotentialEnergy(i, jj, sqrt(rSq));
                else
                {
                    // Pointer comparison for i >= jj
                    if (i >= jj)
                        continue;

                    scale = i->scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(i, jj, sqrt(rSq)) * scale;
                }
            }
        else
            for (auto indexJ = otherAtoms.begin() + start; indexJ < otherAtoms.end(); indexJ += stride)
            {
                // Grab other Atom pointer
                jj = *indexJ;

                // Calculate rSquared distance between atoms, and check it against the stored cutoff distance
                rSq = (rI - jj->r()).magnitudeSq();
                if (rSq > cutoffDistanceSquared_)
                    continue;

                // Check for atoms in the same species
                if (moleculeI != jj->molecule())
                    totalEnergy += pairPotentialEnergy(i, jj, sqrt(rSq));
                else
                {
                    scale = i->scaling(jj);
                    if (scale > 1.0e-3)
                        totalEnergy += pairPotentialEnergy(i, jj, sqrt(rSq)) * scale;
                }
            }
    }

    // Perform relevant sum if requested
    if (performSum)
        processPool_.allSum(&totalEnergy, 1, strategy);

    return totalEnergy;
}

// Return PairPotential energy of Atom with world
double EnergyKernel::energy(const Atom *i, ProcessPool::DivisionStrategy strategy, bool performSum)
{
#ifdef CHECKS
    if (i == nullptr)
    {
        Messenger::error("NULL_POINTER - nullptr passed to EnergyKernel::energy(Atom,ParallelStyle).\n");
        return 0.0;
    }
#endif
    Cell *cellI = i->cell();

    // This Atom with its own Cell
    double totalEnergy = energy(i, cellI, KernelFlags::ExcludeSelfFlag, strategy, false);

    // Cell neighbours not requiring minimum image
    for (auto *neighbour : cellI->cellNeighbours())
        totalEnergy += energy(i, neighbour, KernelFlags::NoFlags, strategy, false);

    // Cell neighbours requiring minimum image
    for (auto *neighbour : cellI->mimCellNeighbours())
        totalEnergy += energy(i, neighbour, KernelFlags::ApplyMinimumImageFlag, strategy, false);

    // Perform relevant sum if requested
    if (performSum)
        processPool_.allSum(&totalEnergy, 1, strategy);

    return totalEnergy;
}

// Return PairPotential energy of Molecule with world
double EnergyKernel::energy(std::shared_ptr<const Molecule> mol, ProcessPool::DivisionStrategy strategy, bool performSum)
{
    double totalEnergy = 0.0;

    for (auto *ii : mol->atoms())
    {
        auto *cellI = ii->cell();

        // This Atom with its own Cell
        totalEnergy += energy(ii, cellI, KernelFlags::ExcludeIntraIGEJFlag, strategy, false);

        // Cell neighbours not requiring minimum image
        totalEnergy +=
            std::accumulate(cellI->cellNeighbours().begin(), cellI->cellNeighbours().end(), 0.0,
                            [&ii, this, &strategy](const auto &acc, const auto *neighbour) {
                                return acc + energy(ii, neighbour, KernelFlags::ExcludeIntraIGEJFlag, strategy, false);
                            });

        // Cell neighbours requiring minimum image
        totalEnergy += std::accumulate(
            cellI->mimCellNeighbours().begin(), cellI->mimCellNeighbours().end(), 0.0,
            [&ii, this, &strategy](const auto &acc, const auto *neighbour) {
                return acc + energy(ii, neighbour, KernelFlags::ApplyMinimumImageFlag | KernelFlags::ExcludeIntraIGEJFlag,
                                    strategy, false);
            });
    }

    // Perform relevant sum if requested
    if (performSum)
        processPool_.allSum(&totalEnergy, 1, strategy);

    return totalEnergy;
}

// Return molecular correction energy related to intramolecular terms involving supplied atom
double EnergyKernel::correct(const Atom *i)
{
    // Loop over atoms in molecule
    auto nMolAtoms = i->molecule()->nAtoms();
    Atom *j;
    std::vector<Atom *> atoms = i->molecule()->atoms();
    double scale, r, correctionEnergy = 0.0;
    const auto rI = i->r();

    correctionEnergy = std::accumulate(atoms.begin(), atoms.end(), 0.0, [&](const auto &acc, auto *j) {
        if (i == j)
            return acc;
        scale = 1.0 - i->scaling(j);
        if (scale <= 1.0e-3)
            return acc;
        r = box_->minimumDistance(rI, j->r());
        return acc + pairPotentialEnergy(i, j, r) * scale;
    });

    return -correctionEnergy;
}

// Return total interatomic PairPotential energy of the system
double EnergyKernel::energy(const CellArray &cellArray, bool interMolecular, ProcessPool::DivisionStrategy strategy,
                            bool performSum)
{
    // Get sub-strategy to use
    ProcessPool::DivisionStrategy subStrategy = ProcessPool::subDivisionStrategy(strategy);

    // Set start/stride for parallel loop
    auto start = processPool_.interleavedLoopStart(strategy);
    auto stride = processPool_.interleavedLoopStride(strategy);

    double totalEnergy = 0.0;
    Cell *cell;
    for (int cellId = start; cellId < cellArray.nCells(); cellId += stride)
    {
        cell = cellArray.cell(cellId);

        // This cell with itself
        totalEnergy += energy(cell, cell, false, true, interMolecular, subStrategy, performSum);

        // Interatomic interactions between atoms in this cell and its neighbours
        totalEnergy += energy(cell, true, interMolecular, subStrategy, performSum);
    }

    return totalEnergy;
}

/*
 * Intramolecular Terms
 */

// Return SpeciesBond energy at Atoms specified
double EnergyKernel::energy(const SpeciesBond &bond, const Atom *i, const Atom *j)
{
#ifdef CHECKS
    // Check for spurious bond distances
    double distance = i->cell()->mimRequired(j->cell()) ? box_->minimumDistance(i, j) : (i->r() - j->r()).magnitude();
    if (distance > 5.0)
        Messenger::print("!!! Long bond: {}-{} = {} Angstroms\n", i->arrayIndex(), j->arrayIndex(), distance);
#endif
    // Determine whether we need to apply minimum image to the distance calculation
    if (i->cell()->mimRequired(j->cell()))
        return bond.energy(box_->minimumDistance(i, j));
    else
        return bond.energy((i->r() - j->r()).magnitude());
}

// Return SpeciesBond energy
double EnergyKernel::energy(const SpeciesBond &b) { return b.energy((b.j()->r() - b.i()->r()).magnitude()); }

// Return SpeciesAngle energy at Atoms specified
double EnergyKernel::energy(const SpeciesAngle &angle, const Atom *i, const Atom *j, const Atom *k)
{
    Vec3<double> vecji, vecjk;

    // Determine whether we need to apply minimum image between 'j-i' and 'j-k'
    if (j->cell()->mimRequired(i->cell()))
        vecji = box_->minimumVector(j, i);
    else
        vecji = i->r() - j->r();
    if (j->cell()->mimRequired(k->cell()))
        vecjk = box_->minimumVector(j, k);
    else
        vecjk = k->r() - j->r();

    // Normalise vectors
    vecji.normalise();
    vecjk.normalise();

    // Determine Angle energy
    return angle.energy(Box::angleInDegrees(vecji, vecjk));
}

// Return SpeciesAngle energy
double EnergyKernel::energy(const SpeciesAngle &angle)
{
    Vec3<double> vecji = angle.i()->r() - angle.j()->r(), vecjk = angle.k()->r() - angle.j()->r();

    // Normalise vectors
    vecji.normalise();
    vecjk.normalise();

    // Determine Angle energy
    return angle.energy(Box::angleInDegrees(vecji, vecjk));
}

// Return SpeciesTorsion energy at Atoms specified
double EnergyKernel::energy(const SpeciesTorsion &torsion, const Atom *i, const Atom *j, const Atom *k, const Atom *l)
{
    Vec3<double> vecji, vecjk, veckl;

    // Calculate vectors, ensuring we account for minimum image
    if (j->cell()->mimRequired(i->cell()))
        vecji = box_->minimumVector(j, i);
    else
        vecji = i->r() - j->r();
    if (j->cell()->mimRequired(k->cell()))
        vecjk = box_->minimumVector(j, k);
    else
        vecjk = k->r() - j->r();
    if (k->cell()->mimRequired(l->cell()))
        veckl = box_->minimumVector(k, l);
    else
        veckl = l->r() - k->r();

    return torsion.energy(Box::torsionInDegrees(vecji, vecjk, veckl));
}

// Return SpeciesTorsion energy
double EnergyKernel::energy(const SpeciesTorsion &torsion)
{
    return torsion.energy(Box::torsionInDegrees(torsion.i()->r() - torsion.j()->r(), torsion.k()->r() - torsion.j()->r(),
                                                torsion.l()->r() - torsion.k()->r()));
}

// Return SpeciesImproper energy
double EnergyKernel::energy(const SpeciesImproper &imp, const Atom *i, const Atom *j, const Atom *k, const Atom *l)
{
    Vec3<double> vecji, vecjk, veckl;

    // Calculate vectors, ensuring we account for minimum image
    if (j->cell()->mimRequired(i->cell()))
        vecji = box_->minimumVector(j, i);
    else
        vecji = i->r() - j->r();
    if (j->cell()->mimRequired(k->cell()))
        vecjk = box_->minimumVector(j, k);
    else
        vecjk = k->r() - j->r();
    if (k->cell()->mimRequired(l->cell()))
        veckl = box_->minimumVector(k, l);
    else
        veckl = l->r() - k->r();

    return imp.energy(Box::torsionInDegrees(vecji, vecjk, veckl));
}

// Return intramolecular energy for the supplied Atom
double EnergyKernel::intramolecularEnergy(std::shared_ptr<const Molecule> mol, const Atom *i)
{
#ifdef CHECKS
    if (i == nullptr)
    {
        Messenger::error("NULL Atom given to EnergyKernel::intraEnergy().\n");
        return 0.0;
    }
    if (i->speciesAtom() == nullptr)
    {
        Messenger::error("NULL SpeciesAtom in Atom given to EnergyKernel::intraEnergy().\n");
        return 0.0;
    }
#endif
    // Get the SpeciesAtom
    const SpeciesAtom *spAtom = i->speciesAtom();

    // If no terms are present, return zero
    if ((spAtom->nBonds() == 0) && (spAtom->nAngles() == 0) && (spAtom->nTorsions() == 0))
        return 0.0;

    auto intraEnergy = 0.0;

    // Add energy from SpeciesAngle terms
    intraEnergy += std::accumulate(spAtom->bonds().begin(), spAtom->bonds().end(), 0.0,
                                   [this, &mol](const auto acc, const SpeciesBond &bond) {
                                       return acc + energy(bond, mol->atom(bond.indexI()), mol->atom(bond.indexJ()));
                                   });

    // Add energy from SpeciesAngle terms
    intraEnergy += std::accumulate(
        spAtom->angles().begin(), spAtom->angles().end(), 0.0, [this, &mol](const auto acc, const SpeciesAngle &angle) {
            return acc + energy(angle, mol->atom(angle.indexI()), mol->atom(angle.indexJ()), mol->atom(angle.indexK()));
        });

    // Add energy from SpeciesTorsion terms
    intraEnergy += std::accumulate(spAtom->torsions().begin(), spAtom->torsions().end(), 0.0,
                                   [this, &mol](const auto acc, const SpeciesTorsion &torsion) {
                                       return acc + energy(torsion, mol->atom(torsion.indexI()), mol->atom(torsion.indexJ()),
                                                           mol->atom(torsion.indexK()), mol->atom(torsion.indexL()));
                                   });

    // Add energy from SpeciesImproper terms
    intraEnergy += std::accumulate(spAtom->impropers().begin(), spAtom->impropers().end(), 0.0,
                                   [this, &mol](const auto acc, const SpeciesImproper &improper) {
                                       return acc + energy(improper, mol->atom(improper.indexI()), mol->atom(improper.indexJ()),
                                                           mol->atom(improper.indexK()), mol->atom(improper.indexL()));
                                   });

    return intraEnergy;
}

// Return intramolecular energy for the supplied Molecule
double EnergyKernel::intramolecularEnergy(std::shared_ptr<const Molecule> mol)
{
#ifdef CHECKS
    if (mol == nullptr)
    {
        Messenger::error("NULL Molecule pointer given to EnergyKernel::intramolecularEnergy.\n");
        return 0.0;
    }
#endif

    auto intraEnergy = 0.0;

    // Loop over Bonds
    intraEnergy = std::accumulate(mol->species()->constBonds().begin(), mol->species()->constBonds().end(), intraEnergy,
                                  [mol, this](const auto acc, const auto &bond) {
                                      return acc + energy(bond, mol->atom(bond.indexI()), mol->atom(bond.indexJ()));
                                  });
    // Loop over Angles
    intraEnergy = std::accumulate(mol->species()->constAngles().begin(), mol->species()->constAngles().end(), intraEnergy,
                                  [mol, this](const auto acc, const auto &angle) {
                                      return acc + energy(angle, mol->atom(angle.indexI()), mol->atom(angle.indexJ()),
                                                          mol->atom(angle.indexK()));
                                  });

    // Loop over Torsions
    intraEnergy = std::accumulate(mol->species()->constTorsions().begin(), mol->species()->constTorsions().end(), intraEnergy,
                                  [mol, this](const auto acc, const auto &torsion) {
                                      return acc + energy(torsion, mol->atom(torsion.indexI()), mol->atom(torsion.indexJ()),
                                                          mol->atom(torsion.indexK()), mol->atom(torsion.indexL()));
                                  });

    // Loop over Impropers
    intraEnergy = std::accumulate(mol->species()->constImpropers().begin(), mol->species()->constImpropers().end(), intraEnergy,
                                  [mol, this](const auto acc, const auto &improper) {
                                      return acc + energy(improper, mol->atom(improper.indexI()), mol->atom(improper.indexJ()),
                                                          mol->atom(improper.indexK()), mol->atom(improper.indexL()));
                                  });

    return intraEnergy;
}
