// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "kernels/force.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/molecule.h"
#include "classes/potentialMap.h"
#include "classes/species.h"
#include "templates/algorithms.h"
#include <iterator>

ForceKernel::ForceKernel(const Configuration *cfg, const PotentialMap &potentialMap)
    : GeometryKernel(cfg->box(), potentialMap), configuration_(cfg)
{
}

/*
 * Force Calculation
 */

// Calculate PairPotential forces between Atoms provided
void ForceKernel::forcesWithoutMim(const ConfigurationAtom &i, int indexI, const ConfigurationAtom &j, int indexJ,
                                   std::vector<Vector3> &f) const
{
    auto vij = j.r() - i.r();
    auto distanceSq = vij.magnitudeSq();
    if (distanceSq > cutoffDistanceSquared_)
        return;
    auto r = sqrt(distanceSq);
    vij /= r;
    vij *= potentialMap_.force(i, j, r);
    f[indexI] -= vij;
    f[indexJ] += vij;
}

// Calculate inter-particle forces between Atoms provided, scaling electrostatic and van der Waals components
void ForceKernel::forcesWithoutMim(const ConfigurationAtom &i, int indexI, const ConfigurationAtom &j, int indexJ,
                                   std::vector<Vector3> &f, double elecScale, double srScale) const
{
    auto vij = j.r() - i.r();
    auto distanceSq = vij.magnitudeSq();
    if (distanceSq > cutoffDistanceSquared_)
        return;
    auto r = sqrt(distanceSq);
    vij /= r;
    vij *= potentialMap_.force(i, j, r, elecScale, srScale);
    f[indexI] -= vij;
    f[indexJ] += vij;
}

// Calculate PairPotential forces between Atoms provided
void ForceKernel::forcesWithMim(const ConfigurationAtom &i, int indexI, const ConfigurationAtom &j, int indexJ,
                                std::vector<Vector3> &f) const
{
    auto vij = box_.minimumVector(i.r(), j.r());
    auto distanceSq = vij.magnitudeSq();
    if (distanceSq > cutoffDistanceSquared_)
        return;
    auto r = sqrt(distanceSq);
    vij /= r;
    vij *= potentialMap_.force(i, j, r);
    f[indexI] -= vij;
    f[indexJ] += vij;
}

// Calculate inter-particle forces between Atoms provided, scaling electrostatic and van der Waals components
void ForceKernel::forcesWithMim(const ConfigurationAtom &i, int indexI, const ConfigurationAtom &j, int indexJ,
                                std::vector<Vector3> &f, double elecScale, double srScale) const
{
    auto vij = box_.minimumVector(i.r(), j.r());
    auto distanceSq = vij.magnitudeSq();
    if (distanceSq > cutoffDistanceSquared_)
        return;
    auto r = sqrt(distanceSq);
    vij /= r;
    vij *= potentialMap_.force(i, j, r, elecScale, srScale);
    f[indexI] -= vij;
    f[indexJ] += vij;
}

/*
 * PairPotential Terms
 */

// Calculate forces between atoms in supplied cells
void ForceKernel::cellToCellPairPotentialForces(const Cell *centralCell, const Cell *otherCell, bool applyMim,
                                                std::vector<Vector3> &f) const
{
    assert(centralCell && otherCell);
    auto &centralAtoms = centralCell->atoms();
    auto &otherAtoms = otherCell->atoms();
    std::shared_ptr<Molecule> molI;

    // Loop over all atom pairs excluding any within the same molecule
    if (applyMim)
    {
        for (const auto &i : centralAtoms)
        {
            molI = i->molecule();
            auto indexI = molI->globalAtomIndex(i);

            for (auto *j : otherAtoms)
                if (molI != j->molecule())
                    forcesWithMim(*i, indexI, *j, j->molecule()->globalAtomIndex(j), f);
        }
    }
    else
    {
        for (const auto &i : centralAtoms)
        {
            molI = i->molecule();
            auto indexI = molI->globalAtomIndex(i);

            for (auto *j : otherAtoms)
                if (molI != j->molecule())
                    forcesWithoutMim(*i, indexI, *j, j->molecule()->globalAtomIndex(j), f);
        }
    }
}

/*
 * Extended Terms
 */

// Calculate extended forces on supplied atom
void ForceKernel::extendedForces(const ConfigurationAtom &i, Vector3 &fVec) const { return; }

// Calculate extended forces on supplied molecule
void ForceKernel::extendedForces(const Molecule &mol, std::vector<Vector3> &f) const { return; }

/*
 * Totals
 */

// Calculate total forces in the world
void ForceKernel::totalForces(std::vector<Vector3> &ppForceVector, std::vector<Vector3> &geometryForceVector,
                              Flags<Kernel::CalculationFlags> flags) const
{
    // Resize and zero force arrays
    ppForceVector.resize(configuration_->nAtoms());
    geometryForceVector.resize(configuration_->nAtoms());
    std::fill(ppForceVector.begin(), ppForceVector.end(), Vector3());
    std::fill(geometryForceVector.begin(), geometryForceVector.end(), Vector3());

    auto &cellArray = configuration_->cells();
    auto &molecules = configuration_->molecules();

    auto combinablePP = Kernel::createCombinableVector3(ppForceVector);
    auto combinableGeometric = Kernel::createCombinableVector3(geometryForceVector);

    // Pair potential forces between different molecules
    if (flags.isNotSet(Kernel::ExcludeInterMolecularPairPotential))
    {
        // Force operator
        auto unaryOp = [&](const int id)
        {
            auto *cellI = cellArray.cell(id);
            auto &fLocal = combinablePP.local();

            // Interatomic interactions between atoms in this cell, excluding those within the same molecule
            dissolve::for_each_pair(ParallelPolicies::seq, cellI->atoms(),
                                    [&](int indexI, const auto &i, int indexJ, const auto &j)
                                    {
                                        if (indexI == indexJ)
                                            return;
                                        // Check for atoms in the same molecule
                                        if (i->molecule() != j->molecule())
                                            forcesWithoutMim(*i, i->index(), *j, j->index(), fLocal);
                                    });

            // Interatomic interactions between atoms in this cell and its neighbours
            auto &neighbours = cellArray.neighbours(*cellI);
            for (auto it = std::next(neighbours.begin()); it != neighbours.end(); ++it)
            {
                if (it->cell.index() < cellI->index())
                    cellToCellPairPotentialForces(cellI, &it->cell, it->requiresMIM, fLocal);
            }
        };

        // Execute lambda operator for each cell
        dissolve::for_each(ParallelPolicies::par, dissolve::counting_iterator<int>(0),
                           dissolve::counting_iterator<int>(cellArray.nCells()), unaryOp);
    }

    // Other molecule forces
    auto moleculeForceOperator = [&](const auto &mol)
    {
        auto &fLocalUnbound = combinablePP.local();
        auto &fLocalBound = combinableGeometric.local();

        auto offset = mol->globalAtomOffset();

        // Geometric terms
        if (flags.isNotSet(Kernel::ExcludeGeometric))
            totalGeometryForces(*mol.get(), fLocalBound);

        // Pair potential interactions between atoms within the molecule
        if (flags.isNotSet(Kernel::ExcludeIntraMolecularPairPotential))
            dissolve::for_each_pair(ParallelPolicies::seq, mol->atoms(),
                                    [&](int indexI, const auto &i, int indexJ, const auto &j)
                                    {
                                        if (indexI == indexJ)
                                            return;
                                        auto &&[scalingType, elec14, vdw14] = i->scaling(j);
                                        if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                                            forcesWithMim(*i, offset + indexI, *j, offset + indexJ, fLocalUnbound);
                                        else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                                            forcesWithMim(*i, offset + indexI, *j, offset + indexJ, fLocalUnbound, elec14,
                                                          vdw14);
                                    });

        // Extended forces
        if (flags.isNotSet(Kernel::ExcludeExtended))
            extendedForces(*mol.get(), fLocalUnbound);
    };

    dissolve::for_each(ParallelPolicies::par, molecules.begin(), molecules.end(), moleculeForceOperator);

    combinablePP.finalize();
    combinableGeometric.finalize();

    // Must multiply by 100.0 to convert from kJ/mol to 10J/mol (our internal MD units)
    std::transform(ppForceVector.begin(), ppForceVector.end(), ppForceVector.begin(), [](auto f) { return f * 100.0; });
    std::transform(geometryForceVector.begin(), geometryForceVector.end(), geometryForceVector.begin(),
                   [](auto f) { return f * 100.0; });
}

// Calculate total forces with simple loops for testing
void ForceKernel::totalForcesSimple(std::vector<Vector3> &ppForceVector, std::vector<Vector3> &geometryForceVector,
                                    Flags<Kernel::CalculationFlags> flags) const
{
    // Resize and zero force arrays
    ppForceVector.resize(configuration_->nAtoms());
    geometryForceVector.resize(configuration_->nAtoms());
    std::fill(ppForceVector.begin(), ppForceVector.end(), Vector3());
    std::fill(geometryForceVector.begin(), geometryForceVector.end(), Vector3());

    const auto &molecules = configuration_->molecules();
    std::shared_ptr<Molecule> molN, molM;

    // Calculate interatomic and intramolecular energy in a loop over defined Molecules
    for (auto n = 0; n < configuration_->nMolecules(); ++n)
    {
        molN = molecules[n];
        auto offsetN = molN->globalAtomOffset();

        // Intramolecular forces (excluding bound terms) in molecule N
        if (flags.isNotSet(Kernel::CalculationFlags::ExcludeIntraMolecularPairPotential))
            for (auto ii = 0; ii < molN->nAtoms() - 1; ++ii)
            {
                auto *i = molN->atom(ii);

                for (auto jj = ii + 1; jj < molN->nAtoms(); ++jj)
                {
                    auto *j = molN->atom(jj);

                    // Get intramolecular scaling of atom pair
                    auto &&[scalingType, elec14, vdw14] = i->scaling(j);

                    if (scalingType == SpeciesAtom::ScaledInteraction::Excluded)
                        continue;

                    // Determine final forces
                    auto vij = box_.minimumVector(i->r(), j->r());
                    auto magjisq = vij.magnitudeSq();
                    if (magjisq > cutoffDistanceSquared_)
                        continue;
                    auto r = sqrt(magjisq);
                    vij /= r;

                    if (scalingType == SpeciesAtom::ScaledInteraction::NotScaled)
                        vij *= potentialMap_.analyticForce(*molN->atom(ii), *molN->atom(jj), r);
                    else if (scalingType == SpeciesAtom::ScaledInteraction::Scaled)
                        vij *= potentialMap_.analyticForce(*molN->atom(ii), *molN->atom(jj), r, elec14, vdw14);

                    ppForceVector[offsetN + ii] -= vij;
                    ppForceVector[offsetN + jj] += vij;
                }
            }

        // Forces between molecule N and molecule M
        if (flags.isNotSet(Kernel::CalculationFlags::ExcludeInterMolecularPairPotential))
            for (auto m = n + 1; m < configuration_->nMolecules(); ++m)
            {
                molM = molecules[m];
                auto offsetM = molM->globalAtomOffset();

                // Double loop over atoms
                for (auto ii = 0; ii < molN->nAtoms(); ++ii)
                {
                    auto *i = molN->atom(ii);

                    for (auto jj = 0; jj < molM->nAtoms(); ++jj)
                    {
                        auto *j = molM->atom(jj);

                        // Determine final forces
                        auto vij = box_.minimumVector(i->r(), j->r());
                        auto magjisq = vij.magnitudeSq();
                        if (magjisq > cutoffDistanceSquared_)
                            continue;
                        auto r = sqrt(magjisq);
                        vij /= r;

                        vij *= potentialMap_.analyticForce(*i, *j, r);

                        ppForceVector[offsetN + ii] -= vij;
                        ppForceVector[offsetM + jj] += vij;
                    }
                }
            }

        if (flags.isNotSet(Kernel::CalculationFlags::ExcludeGeometric))
        {
            // Bond forces
            for (const auto &bond : molN->species()->bonds())
                bondForces(bond, *molN->atom(bond.i()->index()), offsetN + bond.i()->index(), *molN->atom(bond.j()->index()),
                           offsetN + bond.j()->index(), geometryForceVector);

            // Angle forces
            for (const auto &angle : molN->species()->angles())
                angleForces(angle, *molN->atom(angle.i()->index()), offsetN + angle.i()->index(),
                            *molN->atom(angle.j()->index()), offsetN + angle.j()->index(), *molN->atom(angle.k()->index()),
                            offsetN + angle.k()->index(), geometryForceVector);

            // Torsion forces
            for (const auto &torsion : molN->species()->torsions())
                torsionForces(torsion, *molN->atom(torsion.i()->index()), offsetN + torsion.i()->index(),
                              *molN->atom(torsion.j()->index()), offsetN + torsion.j()->index(),
                              *molN->atom(torsion.k()->index()), offsetN + torsion.k()->index(),
                              *molN->atom(torsion.l()->index()), offsetN + torsion.l()->index(), geometryForceVector);

            // Improper forces
            for (const auto &imp : molN->species()->impropers())
                improperForces(imp, *molN->atom(imp.i()->index()), offsetN + imp.i()->index(), *molN->atom(imp.j()->index()),
                               offsetN + imp.j()->index(), *molN->atom(imp.k()->index()), offsetN + imp.k()->index(),
                               *molN->atom(imp.l()->index()), offsetN + imp.l()->index(), geometryForceVector);
        }
    }

    // Convert forces to 10J/mol
    std::transform(ppForceVector.begin(), ppForceVector.end(), ppForceVector.begin(), [](auto &f) { return f * 100.0; });
    std::transform(geometryForceVector.begin(), geometryForceVector.end(), geometryForceVector.begin(),
                   [](auto &f) { return f * 100.0; });
}
