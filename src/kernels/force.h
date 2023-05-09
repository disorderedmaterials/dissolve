// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/processpool.h"
#include "classes/cellarray.h"
#include "kernels/geometry.h"
#include "templates/combinable.h"
#include "templates/flags.h"
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
    void cellPairPotentialForces(const Cell *cell, bool excludeIgeJ, bool includeIntraMolecular,
                                 ProcessPool::DivisionStrategy strategy, ForceVector &f) const;
    // Calculate forces between two cells
    void cellToCellPairPotentialForces(const Cell *cell, const Cell *otherCell, bool applyMim, bool excludeIgeJ,
                                       bool includeIntraMolecular, ProcessPool::DivisionStrategy strategy,
                                       ForceVector &f) const;

    /*
     * Extended Terms
     */
    private:
    // Calculate extended forces on supplied molecule
    virtual void extendedForces(const Molecule &mol, ForceVector &f) const;

    /*
     * Totals
     */
    public:
    // Force calculation flags
    enum ForceCalculationFlags
    {
        ExcludePairPotential,
        ExcludeGeometry,
        ExcludeExtended,
        ExcludeIntraMolecularPairPotential
    };

    public:
    // Calculate total forces in the world
    void totalForces(ForceVector &fUnbound, ForceVector &fBound, ProcessPool::DivisionStrategy strategy,
                     Flags<ForceCalculationFlags> flags = {}) const;
};
