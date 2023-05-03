// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/processpool.h"
#include "classes/cellarray.h"
#include "kernels/geometry.h"
#include "templates/combinable.h"
#include <optional>

// Forward Declarations
class Atom;
class Box;
class Cell;
class Configuration;
class PotentialMap;
class Species;
class SpeciesAngle;
class SpeciesBond;
class SpeciesImproper;
class SpeciesTorsion;

// ForceKernel
class ForceKernel : public GeometryKernel
{
    private:
    friend class KernelProducer;
    ForceKernel(const Configuration *cfg, const ProcessPool &procPool, const PotentialMap &potentialMap,
                std::optional<double> energyCutoff = {});
    ForceKernel(const Box *box, const ProcessPool &procPool, const PotentialMap &potentialMap,
                std::optional<double> energyCutoff = {});

    public:
    ~ForceKernel() = default;

    // Create combinable forces storage container
    static dissolve::CombinableContainer<ForceVector> createCombinableForces(ForceVector &parentForces)
    {
        return {parentForces, [&]() { return std::vector<Vec3<double>>(parentForces.size()); }};
    }

    /*
     * PairPotential Terms
     */
    private:
    // Calculate inter-particle forces between Atoms provided
    void forcesWithoutMim(const Atom &i, const Atom &j, ForceVector &f) const;
    // Calculate inter-particle forces between Atoms provided, scaling electrostatic and van der Waals components
    void forcesWithoutMim(const Atom &i, const Atom &j, ForceVector &f, double elecScale, double vdwScale) const;
    // Calculate inter-particle forces between Atoms provided
    void forcesWithMim(const Atom &i, const Atom &j, ForceVector &f) const;
    // Calculate inter-particle forces between Atoms provided, scaling electrostatic and van der Waals components
    void forcesWithMim(const Atom &i, const Atom &j, ForceVector &f, double elecScale, double vdwScale) const;
    // Calculate forces between Cell and its neighbours
    void cellPairPotentialForces(const Cell *cell, bool excludeIgeJ, ProcessPool::DivisionStrategy strategy,
                                 ForceVector &f) const;
    // Calculate forces between two cells
    void cellToCellPairPotentialForces(const Cell *cell, const Cell *otherCell, bool applyMim, bool excludeIgeJ,
                                       ProcessPool::DivisionStrategy strategy, ForceVector &f) const;

    public:
    // Calculate total pairpotential forces between atoms in the world
    void totalPairPotentialForces(ForceVector &f, ProcessPool::DivisionStrategy strategy) const;

    /*
     * Intramolecular Terms
     */
    public:
    // Calculate total (intramolecular) forces within the specified molecule
    void totalIntramolecularForces(Molecule *mol, ForceVector &f, bool includePairPotentialTerms) const;
    // Calculate total intramolecular forces in the world
    void totalIntramolecularForces(ForceVector &f, bool includePairPotentialTerms,
                                   ProcessPool::DivisionStrategy strategy) const;
};
