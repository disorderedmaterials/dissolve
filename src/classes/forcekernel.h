// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/processpool.h"
#include "classes/cellarray.h"
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
class ForceKernel
{
    public:
    ForceKernel(const ProcessPool &procPool, const Configuration *cfg, const PotentialMap &potentialMap,
                std::optional<double> energyCutoff = {});
    ForceKernel(const ProcessPool &procPool, const Box *box, const PotentialMap &potentialMap,
                std::optional<double> energyCutoff = {});
    ~ForceKernel() = default;

    // Alias for force storage vector
    using ForceVector = std::vector<Vec3<double>>;
    // Create combinable forces storage container
    static dissolve::CombinableContainer<ForceVector> createCombinableForces(ForceVector &parentForces)
    {
        return {parentForces, [&]() { return std::vector<Vec3<double>>(parentForces.size()); }};
    }

    /*
     * Source Data
     */
    protected:
    // Target Box
    const Box *box_;
    // Target CellArray (if available)
    OptionalReferenceWrapper<const CellArray> cellArray_;
    // Target molecule array (if available))
    OptionalReferenceWrapper<const std::vector<std::shared_ptr<Molecule>>> molecules_;
    // Potential map to use
    const PotentialMap &potentialMap_;
    // Squared cutoff distance to use in calculation
    double cutoffDistanceSquared_;

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
    // Calculate forces between two cells
    void forces(const Cell *cell, const Cell *otherCell, bool applyMim, bool excludeIgeJ,
                ProcessPool::DivisionStrategy strategy, ForceVector &f) const;
    // Calculate forces between Cell and its neighbours
    void forces(const Cell *cell, bool excludeIgeJ, ProcessPool::DivisionStrategy strategy, ForceVector &f) const;

    public:
    // Calculate total pairpotential forces between atoms in the world
    void totalPairPotentialForces(ForceVector &f, ProcessPool::DivisionStrategy strategy) const;

    /*
     * Intramolecular Terms
     */
    private:
    struct AngleParameters
    {
        AngleParameters() = default;
        double theta_;
        Vec3<double> dfi_dtheta_;
        Vec3<double> dfk_dtheta_;
    };
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
    // Calculate SpeciesBond forces
    void forces(const SpeciesBond &bond, const Atom &i, const Atom &j, ForceVector &f) const;
    void forces(const SpeciesBond &bond, const Vec3<double> &ri, const Vec3<double> &rj, ForceVector &f) const;
    // Calculate SpeciesBond forces for specified Atom only
    void forces(const Atom &onlyThis, const SpeciesBond &bond, const Atom &i, const Atom &j, ForceVector &f) const;
    // Calculate angle force parameters from supplied vectors
    static AngleParameters calculateAngleParameters(Vec3<double> vecji, Vec3<double> vecjk);
    // Calculate SpeciesAngle forces
    void forces(const SpeciesAngle &angle, const Atom &i, const Atom &j, const Atom &k, ForceVector &f) const;
    void forces(const SpeciesAngle &angle, const Vec3<double> &ri, const Vec3<double> &rj, const Vec3<double> &rk,
                ForceVector &f) const;
    // Calculate SpeciesAngle forces for specified Atom only
    void forces(const Atom &onlyThis, const SpeciesAngle &angle, const Atom &i, const Atom &j, const Atom &k,
                ForceVector &f) const;
    // Calculate torsion force parameters from supplied vectors
    static TorsionParameters calculateTorsionParameters(const Vec3<double> &vecji, const Vec3<double> &vecjk,
                                                        const Vec3<double> &veckl);
    // Calculate SpeciesTorsion forces
    void forces(const SpeciesTorsion &torsion, const Atom &i, const Atom &j, const Atom &k, const Atom &l,
                ForceVector &f) const;
    void forces(const SpeciesTorsion &torsion, const Vec3<double> &ri, const Vec3<double> &rj, const Vec3<double> &rk,
                const Vec3<double> &rl, ForceVector &f) const;
    // Calculate SpeciesTorsion forces for specified Atom only
    void forces(const Atom &onlyThis, const SpeciesTorsion &torsion, const Atom &i, const Atom &j, const Atom &k, const Atom &l,
                ForceVector &f) const;
    // Calculate SpeciesImproper forces
    void forces(const SpeciesImproper &improper, const Atom &i, const Atom &j, const Atom &k, const Atom &l,
                ForceVector &f) const;
    void forces(const SpeciesImproper &torsion, const Vec3<double> &ri, const Vec3<double> &rj, const Vec3<double> &rk,
                const Vec3<double> &rl, ForceVector &f) const;
    // Calculate SpeciesImproper forces for specified Atom only
    void forces(const Atom &onlyThis, const SpeciesImproper &improper, const Atom &i, const Atom &j, const Atom &k,
                const Atom &l, ForceVector &f) const;
    // Calculate intramolecular forces within the specified molecule
    void intramolecularForces(Molecule *mol, ForceVector &f, bool includePairPotentialTerms) const;
    // Calculate total intramolecular forces in the world
    void totalIntramolecularForces(ForceVector &f, bool includePairPotentialTerms,
                                   ProcessPool::DivisionStrategy strategy) const;

    /*
     * Parallel Comms
     */
    private:
    // Process pool over which this kernel operates
    const ProcessPool &processPool_;
};
