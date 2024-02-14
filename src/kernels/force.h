// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/processPool.h"
#include "classes/cellArray.h"
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
    friend class ExternalPotentialsForceKernel;
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
    void forcesWithoutMim(const Atom i, int indexI, const Atom j, int indexJ, ForceVector &f) const;
    // Calculate inter-particle forces between Atoms provided, scaling electrostatic and van der Waals components
    void forcesWithoutMim(const Atom i, int indexI, const Atom j, int indexJ, ForceVector &f, double elecScale,
                          double vdwScale) const;
    // Calculate inter-particle forces between Atoms provided
    void forcesWithMim(const Atom i, int indexI, const Atom j, int indexJ, ForceVector &f) const;
    // Calculate inter-particle forces between Atoms provided, scaling electrostatic and van der Waals components
    void forcesWithMim(const Atom i, int indexI, const Atom j, int indexJ, ForceVector &f, double elecScale,
                       double vdwScale) const;
    // Calculate forces between two cells
    void cellToCellPairPotentialForces(const Cell *cell, const Cell *otherCell, bool applyMim, ForceVector &f) const;

    /*
     * Extended Terms
     */
    private:
    // Calculate extended forces on supplied atom
    virtual void extendedForces(const Atom i, Vec3<double> &fVec) const;
    // Calculate extended forces on supplied molecule
    virtual void extendedForces(const Molecule &mol, ForceVector &f) const;

    /*
     * Totals
     */
    public:
    // Force calculation flags
    enum ForceCalculationFlags
    {
        ExcludeInterMolecularPairPotential,
        ExcludeIntraMolecularPairPotential,
        ExcludeGeometry,
        ExcludeExtended
    };

    public:
    // Calculate total forces in the world
    void totalForces(ForceVector &fUnbound, ForceVector &fBound, ProcessPool::DivisionStrategy strategy,
                     Flags<ForceCalculationFlags> flags = {}) const;
};
