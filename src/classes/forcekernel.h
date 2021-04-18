// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/processpool.h"
#include "classes/cellarray.h"
#include "classes/kernelflags.h"

// Forward Declarations
class Atom;
class Box;
class Cell;
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
    ForceKernel(ProcessPool &procPool, const Box *box, const PotentialMap &potentialMap, std::vector<Vec3<double>> &f,
                double cutoffDistance = -1.0);
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
    // Force vector
    std::vector<Vec3<double>> &f_;

    /*
     * Internal Force Calculation
     */
    private:
    // Calculate inter-particle forces between Atoms provided (no minimum image calculation)
    void forcesWithoutMim(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j, double scale = 1.0);
    // Calculate inter-particle forces between Atoms provided (minimum image calculation)
    void forcesWithMim(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j, double scale = 1.0);

    /*
     * PairPotential Terms
     */
    public:
    // Calculate forces between atoms provided
    void forces(const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j, bool applyMim, bool excludeIgeJ);
    // Calculate forces between two cells
    void forces(Cell *cell, Cell *otherCell, bool applyMim, bool excludeIgeJ, ProcessPool::DivisionStrategy strategy);
    // Calculate forces between Cell and its neighbours
    void forces(Cell *cell, bool excludeIgeJ, ProcessPool::DivisionStrategy strategy);
    // Calculate forces between Atom and Cell
    void forces(const std::shared_ptr<Atom> i, Cell *cell, int flags, ProcessPool::DivisionStrategy strategy);
    // Calculate forces between atom and world
    void forces(const std::shared_ptr<Atom> i, ProcessPool::DivisionStrategy strategy);

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
    // Sum torsion forces for atom 'i' in 'i-j-k-l' into the specified vector index
    void sumTorsionForceI(double du_dphi, int index);
    // Sum torsion forces for atom 'j' in 'i-j-k-l' into the specified vector index
    void sumTorsionForceJ(double du_dphi, int index);
    // Sum torsion forces for atom 'k' in 'i-j-k-l' into the specified vector index
    void sumTorsionForceK(double du_dphi, int index);
    // Sum torsion forces for atom 'l' in 'i-j-k-l' into the specified vector index
    void sumTorsionForceL(double du_dphi, int index);

    public:
    // Calculate SpeciesBond forces
    void forces(const SpeciesBond &bond, const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j);
    // Calculate SpeciesBond forces
    void forces(const SpeciesBond &bond);
    // Calculate SpeciesBond forces for specified Atom only
    void forces(const std::shared_ptr<Atom> onlyThis, const SpeciesBond &bond, const std::shared_ptr<Atom> i,
                const std::shared_ptr<Atom> j);
    // Calculate angle force parameters from supplied vectors, storing results in supplied variables
    static void calculateAngleParameters(Vec3<double> vecji, Vec3<double> vecjk, double &theta, Vec3<double> &dfi_dtheta,
                                         Vec3<double> &dfk_dtheta);
    // Calculate SpeciesAngle forces
    void forces(const SpeciesAngle &angle, const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j,
                const std::shared_ptr<Atom> k);
    // Calculate SpeciesAngle forces
    void forces(const SpeciesAngle &angle);
    // Calculate SpeciesAngle forces for specified Atom only
    void forces(const std::shared_ptr<Atom> onlyThis, const SpeciesAngle &angle, const std::shared_ptr<Atom> i,
                const std::shared_ptr<Atom> j, const std::shared_ptr<Atom> k);
    // Calculate torsion force parameters from supplied vectors, storing results in supplied variables
    static void calculateTorsionParameters(const Vec3<double> vecji, const Vec3<double> vecjk, const Vec3<double> veckl,
                                           double &phi, Matrix3 &dxpj_dij, Matrix3 &dxpj_dkj, Matrix3 &dxpk_dkj,
                                           Matrix3 &dxpk_dlk, Vec3<double> &dcos_dxpj, Vec3<double> &dcos_dxpk);
    // Calculate SpeciesTorsion forces
    void forces(const SpeciesTorsion &torsion, const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j,
                const std::shared_ptr<Atom> k, const std::shared_ptr<Atom> l);
    // Calculate SpeciesTorsion forces
    void forces(const SpeciesTorsion &torsion);
    // Calculate SpeciesTorsion forces for specified Atom only
    void forces(const std::shared_ptr<Atom> onlyThis, const SpeciesTorsion &torsion, const std::shared_ptr<Atom> i,
                const std::shared_ptr<Atom> j, const std::shared_ptr<Atom> k, const std::shared_ptr<Atom> l);
    // Calculate SpeciesImproper forces
    void forces(const SpeciesImproper &torsion, const std::shared_ptr<Atom> i, const std::shared_ptr<Atom> j,
                const std::shared_ptr<Atom> k, const std::shared_ptr<Atom> l);
    // Calculate SpeciesImproper forces
    void forces(const SpeciesImproper &torsion);
    // Calculate SpeciesImproper forces for specified Atom only
    void forces(const std::shared_ptr<Atom> onlyThis, const SpeciesImproper &torsion, const std::shared_ptr<Atom> i,
                const std::shared_ptr<Atom> j, const std::shared_ptr<Atom> k, const std::shared_ptr<Atom> l);

    /*
     * Parallel Comms
     */
    private:
    // Process pool over which this kernel operates
    ProcessPool &processPool_;
};
