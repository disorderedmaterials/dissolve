/*
    *** ForceKernel
    *** src/classes/forcekernel.h
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

#pragma once

#include "base/processpool.h"
#include "classes/cellarray.h"
#include "classes/kernelflags.h"
#include "templates/orderedpointerlist.h"

// Forward Declarations
class Atom;
class Box;
class Cell;
class ChangeStore;
class Configuration;
class PotentialMap;
class SpeciesAngle;
class SpeciesBond;
class SpeciesImproper;
class SpeciesTorsion;

// ForceKernel
class ForceKernel
{
    public:
    ForceKernel(ProcessPool &procPool, const Box *box, const PotentialMap &potentialMap, Array<double> &fx, Array<double> &fy,
                Array<double> &fz, double cutoffDistance = -1.0);
    ~ForceKernel() = default;

    /*
     * Source Data
     */
    protected:
    // Source Box (from Configuration)
    const Box *box_;
    // Potential map to use
    const PotentialMap &potentialMap_;
    // Squared cutoff distance to use in calculation
    double cutoffDistanceSquared_;
    // Force array for x component
    Array<double> &fx_;
    // Force array for y component
    Array<double> &fy_;
    // Force array for z component
    Array<double> &fz_;

    /*
     * Internal Force Calculation
     */
    private:
    // Calculate inter-particle forces between Atoms provided (no minimum image calculation)
    void forcesWithoutMim(const Atom *i, const Atom *j, double scale = 1.0);
    // Calculate inter-particle forces between Atoms provided (minimum image calculation)
    void forcesWithMim(const Atom *i, const Atom *j, double scale = 1.0);

    /*
     * PairPotential Terms
     */
    public:
    // Calculate forces between atoms provided
    void forces(const Atom *i, const Atom *j, bool applyMim, bool excludeIgeJ);
    // Calculate forces between two cells
    void forces(Cell *cell, Cell *otherCell, bool applyMim, bool excludeIgeJ, ProcessPool::DivisionStrategy strategy);
    // Calculate forces between Cell and its neighbours
    void forces(Cell *cell, bool excludeIgeJ, ProcessPool::DivisionStrategy strategy);
    // Calculate forces between Atom and Cell
    void forces(const Atom *i, Cell *cell, int flags, ProcessPool::DivisionStrategy strategy);
    // Calculate forces between atom and world
    void forces(const Atom *i, ProcessPool::DivisionStrategy strategy);

    /*
     * Intramolecular Terms
     */
    private:
    // Local calculated angle parameters
    double theta_;
    Vec3<double> dfi_dtheta_, dfk_dtheta_;
    // Local calculated torsion parameters
    double phi_;
    Matrix3 dxpj_dij_, dxpj_dkj_, dxpk_dkj_, dxpk_dlk_;
    Vec3<double> dcos_dxpj_, dcos_dxpk_;

    private:
    // Calculate angle force parameters from supplied vectors, storing result in local class variables
    void calculateAngleParameters(Vec3<double> vecji, Vec3<double> vecjk);
    // Calculate torsion force parameters from supplied vectors, storing result in local class variables
    void calculateTorsionParameters(const Vec3<double> vecji, const Vec3<double> vecjk, const Vec3<double> veckl);

    public:
    // Calculate SpeciesBond forces
    void forces(const SpeciesBond &bond, const Atom *i, const Atom *j);
    // Calculate SpeciesBond forces
    void forces(const SpeciesBond &bond);
    // Calculate SpeciesBond forces for specified Atom only
    void forces(const Atom *onlyThis, const SpeciesBond &bond, const Atom *i, const Atom *j);
    // Calculate angle force parameters from supplied vectors, storing results in supplied variables
    static void calculateAngleParameters(Vec3<double> vecji, Vec3<double> vecjk, double &theta, Vec3<double> &dfi_dtheta,
                                         Vec3<double> &dfk_dtheta);
    // Calculate SpeciesAngle forces
    void forces(const SpeciesAngle &angle, const Atom *i, const Atom *j, const Atom *k);
    // Calculate SpeciesAngle forces
    void forces(const SpeciesAngle &angle);
    // Calculate SpeciesAngle forces for specified Atom only
    void forces(const Atom *onlyThis, const SpeciesAngle &angle, const Atom *i, const Atom *j, const Atom *k);
    // Calculate torsion force parameters from supplied vectors, storing results in supplied variables
    static void calculateTorsionParameters(const Vec3<double> vecji, const Vec3<double> vecjk, const Vec3<double> veckl,
                                           double &phi, Matrix3 &dxpj_dij, Matrix3 &dxpj_dkj, Matrix3 &dxpk_dkj,
                                           Matrix3 &dxpk_dlk, Vec3<double> &dcos_dxpj, Vec3<double> &dcos_dxpk);
    // Calculate SpeciesTorsion forces
    void forces(const SpeciesTorsion &torsion, const Atom *i, const Atom *j, const Atom *k, const Atom *l);
    // Calculate SpeciesTorsion forces
    void forces(const SpeciesTorsion &torsion);
    // Calculate SpeciesTorsion forces for specified Atom only
    void forces(const Atom *onlyThis, const SpeciesTorsion &torsion, const Atom *i, const Atom *j, const Atom *k,
                const Atom *l);
    // Calculate SpeciesImproper forces
    void forces(const SpeciesImproper &torsion, const Atom *i, const Atom *j, const Atom *k, const Atom *l);
    // Calculate SpeciesImproper forces
    void forces(const SpeciesImproper &torsion);
    // Calculate SpeciesImproper forces for specified Atom only
    void forces(const Atom *onlyThis, const SpeciesImproper &torsion, const Atom *i, const Atom *j, const Atom *k,
                const Atom *l);

    /*
     * Parallel Comms
     */
    private:
    // Process pool over which this kernel operates
    ProcessPool &processPool_;
};
