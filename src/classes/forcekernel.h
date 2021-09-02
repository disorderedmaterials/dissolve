// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/processpool.h"
#include "classes/cellarray.h"
#include "classes/kernelflags.h"
#include <optional>

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
    ForceKernel(ProcessPool &procPool, const Box *box, const CellArray &cells, const PotentialMap &potentialMap,
                std::optional<double> energyCutoff = std::nullopt);
    ~ForceKernel() = default;

    // Alias for force storage vector
    using ForceVector = std::vector<Vec3<double>>;

    /*
     * Source Data
     */
    protected:
    // Source Box (from Configuration)
    const Box *box_;
    // Source CellArray (from Configuration)
    const CellArray &cellArray_;
    // Potential map to use
    const PotentialMap &potentialMap_;
    // Squared cutoff distance to use in calculation
    double cutoffDistanceSquared_;

    /*
     * Internal Force Calculation
     */
    private:
    // Calculate inter-particle forces between Atoms provided (no minimum image calculation)
    void forcesWithoutMim(const Atom &i, const Atom &j, ForceVector &f, double scale = 1.00) const;
    // Calculate inter-particle forces between Atoms provided (minimum image calculation)
    void forcesWithMim(const Atom &i, const Atom &j, ForceVector &f, double scale = 1.00) const;

    /*
     * PairPotential Terms
     */
    public:
    // Calculate forces between atoms provided
    void forces(const Atom &i, const Atom &j, bool applyMim, bool excludeIgeJ, ForceVector &f) const;
    // Calculate forces between two cells
    void forces(const Cell *cell, const Cell *otherCell, bool applyMim, bool excludeIgeJ,
                ProcessPool::DivisionStrategy strategy, ForceVector &f) const;
    // Calculate forces between Cell and its neighbours
    void forces(const Cell *cell, bool excludeIgeJ, ProcessPool::DivisionStrategy strategy, ForceVector &f) const;
    // Calculate forces between Atom and Cell
    void forces(const Atom &i, const Cell *cell, int flags, ProcessPool::DivisionStrategy strategy, ForceVector &f) const;
    // Calculate forces between atom and world
    void forces(const Atom &i, ProcessPool::DivisionStrategy strategy, ForceVector &f) const;

    struct TorsionParameters
    {
        TorsionParameters() = default;
        double phi_;
        Matrix3 dxpj_dij_;
        Matrix3 dxpj_dkj_;
        Matrix3 dxpk_dkj_;
        Matrix3 dxpk_dlk_;
        Vec3<double> dcos_dxpj_;
        Vec3<double> dcos_dxpk_;
    };

    struct AngleParameters
    {
        AngleParameters() = default;
        double theta_;
        Vec3<double> dfi_dtheta_;
        Vec3<double> dfk_dtheta_;
    };
    // Calculate angle force parameters from supplied vectors
    static AngleParameters calculateAngleParameters(Vec3<double> vecji, Vec3<double> vecjk);
    // Calculate torsion force parameters from supplied vectors
    static TorsionParameters calculateTorsionParameters(const Vec3<double> &vecji, const Vec3<double> &vecjk,
                                                        const Vec3<double> &veckl);

    /*
     * Intramolecular Terms
     */
    private:
    // Add torsion forces for atom 'i' in 'i-j-k-l' into the specified vector index
    void addTorsionForceI(double du_dphi, int index, TorsionParameters &torsionParameters, ForceVector &f) const;
    // Sum torsion forces for atom 'j' in 'i-j-k-l' into the specified vector index
    void addTorsionForceJ(double du_dphi, int index, TorsionParameters &torsionParameters, ForceVector &f) const;
    // Sum torsion forces for atom 'k' in 'i-j-k-l' into the specified vector index
    void addTorsionForceK(double du_dphi, int index, TorsionParameters &torsionParameters, ForceVector &f) const;
    // Sum torsion forces for atom 'l' in 'i-j-k-l' into the specified vector index
    void addTorsionForceL(double du_dphi, int index, TorsionParameters &torsionParameters, ForceVector &f) const;

    public:
    // Bond terms
    // Calculate SpeciesBond forces for given atoms
    void forces(const SpeciesBond &bond, const Atom &i, const Atom &j, ForceVector &f) const;
    // Calculate SpeciesBond forces
    void forces(const SpeciesBond &bond, ForceVector &f) const;
    // Calculate SpeciesBond forces for specified Atom only
    void forces(const Atom &onlyThis, const SpeciesBond &bond, const Atom &i, const Atom &j, ForceVector &f) const;

    // Angle terms
    // Calculate SpeciesAngle forces for given atoms
    void forces(const SpeciesAngle &angle, const Atom &i, const Atom &j, const Atom &k, ForceVector &f) const;
    // Calculate SpeciesAngle forces
    void forces(const SpeciesAngle &angle, ForceVector &f) const;
    // Calculate SpeciesAngle forces for specified Atom only
    void forces(const Atom &onlyThis, const SpeciesAngle &angle, const Atom &i, const Atom &j, const Atom &k,
                ForceVector &f) const;

    // Torsion terms
    // Calculate SpeciesTorsion forces for given atoms
    void forces(const SpeciesTorsion &torsion, const Atom &i, const Atom &j, const Atom &k, const Atom &l,
                ForceVector &f) const;
    // Calculate SpeciesTorsion forces
    void forces(const SpeciesTorsion &torsion, ForceVector &f) const;
    // Calculate SpeciesTorsion forces for specified Atom only
    void forces(const Atom &onlyThis, const SpeciesTorsion &torsion, const Atom &i, const Atom &j, const Atom &k, const Atom &l,
                ForceVector &f) const;

    // Improper Terms
    // Calculate SpeciesImproper forces for given atoms
    void forces(const SpeciesImproper &improper, const Atom &i, const Atom &j, const Atom &k, const Atom &l,
                ForceVector &f) const;
    // Calculate SpeciesImproper forces
    void forces(const SpeciesImproper &improper, ForceVector &f) const;
    // Calculate SpeciesImproper forces for specified Atom only
    void forces(const Atom &onlyThis, const SpeciesImproper &improper, const Atom &i, const Atom &j, const Atom &k,
                const Atom &l, ForceVector &f) const;

    /*
     * Parallel Comms
     */
    private:
    // Process pool over which this kernel operates
    ProcessPool &processPool_;
};
