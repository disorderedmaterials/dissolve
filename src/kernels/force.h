// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/cellArray.h"
#include "kernels/geometry.h"
#include "templates/flags.h"

// Forward Declarations
class Atom;
class Box;
class Cell;
class Configuration;
class PotentialMap;

// ForceKernel
class ForceKernel : public GeometryKernel
{
    private:
    friend class KernelProducer;
    friend class ExternalPotentialsForceKernel;
    ForceKernel(const Configuration *cfg, const PotentialMap &potentialMap);

    public:
    ~ForceKernel() = default;

    private:
    // Target Configuration
    const Configuration *configuration_{nullptr};

    /*
     * PairPotential Terms
     */
    private:
    // Calculate inter-particle forces between Atoms provided
    void forcesWithoutMim(const Atom &i, int indexI, const Atom &j, int indexJ, std::vector<Vector3> &f) const;
    // Calculate inter-particle forces between Atoms provided, scaling electrostatic and van der Waals components
    void forcesWithoutMim(const Atom &i, int indexI, const Atom &j, int indexJ, std::vector<Vector3> &f, double elecScale,
                          double srScale) const;
    // Calculate inter-particle forces between Atoms provided
    void forcesWithMim(const Atom &i, int indexI, const Atom &j, int indexJ, std::vector<Vector3> &f) const;
    // Calculate inter-particle forces between Atoms provided, scaling electrostatic and van der Waals components
    void forcesWithMim(const Atom &i, int indexI, const Atom &j, int indexJ, std::vector<Vector3> &f, double elecScale,
                       double srScale) const;
    // Calculate forces between two cells
    void cellToCellPairPotentialForces(const Cell *cell, const Cell *otherCell, bool applyMim, std::vector<Vector3> &f) const;

    /*
     * Extended Terms
     */
    private:
    // Calculate extended forces on supplied atom
    virtual void extendedForces(const Atom &i, Vector3 &fVec) const;
    // Calculate extended forces on supplied molecule
    virtual void extendedForces(const Molecule &mol, std::vector<Vector3> &f) const;

    /*
     * Totals
     */
    public:
    // Calculate total forces
    void totalForces(std::vector<Vector3> &ppForceVector, std::vector<Vector3> &geometryForceVector,
                     Flags<Kernel::CalculationFlags> flags = {}) const;
    // Calculate total forces with simple loops for testing
    void totalForcesSimple(std::vector<Vector3> &ppForceVector, std::vector<Vector3> &geometryForceVector,
                           Flags<Kernel::CalculationFlags> flags = {}) const;
};
