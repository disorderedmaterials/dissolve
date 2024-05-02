// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/processPool.h"
#include "kernels/base.h"
#include "math/matrix3.h"

// Forward Declarations
class Atom;
class Molecule;
class SpeciesAngle;
class SpeciesBond;
class SpeciesImproper;
class SpeciesTorsion;

// Kernel for intramolecular terms only
class GeometryKernel : public KernelBase
{
    public:
    GeometryKernel(const Configuration *cfg, const ProcessPool &procPool, const PotentialMap &potentialMap,
                   std::optional<double> energyCutoff = {});
    GeometryKernel(const Box *box, const ProcessPool &procPool, const PotentialMap &potentialMap,
                   std::optional<double> energyCutoff = {});
    ~GeometryKernel() = default;

    /*
     * Bond Terms
     */
    public:
    // Return SpeciesBond energy at Atoms specified
    double bondEnergy(const SpeciesBond &b, const Atom &i, const Atom &j) const;
    // Calculate SpeciesBond forces
    void bondForces(const SpeciesBond &bond, const Atom &i, int indexI, const Atom &j, int indexJ, ForceVector &f) const;
    void bondForces(const SpeciesBond &bond, const Vec3<double> &ri, const Vec3<double> &rj, ForceVector &f) const;

    /*
     * Angle Terms
     */
    private:
    struct AngleParameters
    {
        AngleParameters() = default;
        double theta_;
        Vec3<double> dfi_dtheta_;
        Vec3<double> dfk_dtheta_;
    };

    public:
    // Return SpeciesAngle energy at Atoms specified
    double angleEnergy(const SpeciesAngle &a, const Atom &i, const Atom &j, const Atom &k) const;
    // Calculate angle force parameters from supplied vectors
    static AngleParameters calculateAngleForceParameters(Vec3<double> vecji, Vec3<double> vecjk);
    // Calculate SpeciesAngle forces
    void angleForces(const SpeciesAngle &angle, const Atom &i, int indexI, const Atom &j, int indexJ, const Atom &k, int indexK,
                     ForceVector &f) const;
    void angleForces(const SpeciesAngle &angle, const Vec3<double> &ri, const Vec3<double> &rj, const Vec3<double> &rk,
                     ForceVector &f) const;

    /*
     * Torsion Terms
     */
    private:
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
    // Add torsion forces for atom 'i' in 'i-j-k-l' into the specified vector index
    void addTorsionForceI(double du_dphi, int index, TorsionParameters &torsionParameters, ForceVector &f) const;
    // Sum torsion forces for atom 'j' in 'i-j-k-l' into the specified vector index
    void addTorsionForceJ(double du_dphi, int index, TorsionParameters &torsionParameters, ForceVector &f) const;
    // Sum torsion forces for atom 'k' in 'i-j-k-l' into the specified vector index
    void addTorsionForceK(double du_dphi, int index, TorsionParameters &torsionParameters, ForceVector &f) const;
    // Sum torsion forces for atom 'l' in 'i-j-k-l' into the specified vector index
    void addTorsionForceL(double du_dphi, int index, TorsionParameters &torsionParameters, ForceVector &f) const;

    public:
    // Return SpeciesTorsion energy at Atoms specified
    double torsionEnergy(const SpeciesTorsion &t, const Atom &i, const Atom &j, const Atom &k, const Atom &l) const;
    // Calculate torsion force parameters from supplied vectors
    static TorsionParameters calculateTorsionForceParameters(const Vec3<double> &vecji, const Vec3<double> &vecjk,
                                                             const Vec3<double> &veckl);
    // Calculate SpeciesTorsion forces
    void torsionForces(const SpeciesTorsion &torsion, const Atom &i, int indexI, const Atom &j, int indexJ, const Atom &k,
                       int indexK, const Atom &l, int indexL, ForceVector &f) const;
    void torsionForces(const SpeciesTorsion &torsion, const Vec3<double> &ri, const Vec3<double> &rj, const Vec3<double> &rk,
                       const Vec3<double> &rl, ForceVector &f) const;

    /*
     * Improper Terms
     */
    public:
    // Return SpeciesImproper energy at Atoms specified
    double improperEnergy(const SpeciesImproper &imp, const Atom &i, const Atom &j, const Atom &k, const Atom &l) const;
    // Calculate SpeciesImproper forces
    void improperForces(const SpeciesImproper &improper, const Atom &i, int indexI, const Atom &j, int indexJ, const Atom &k,
                        int indexK, const Atom &l, int indexL, ForceVector &f) const;
    void improperForces(const SpeciesImproper &improper, const Vec3<double> &ri, const Vec3<double> &rj, const Vec3<double> &rk,
                        const Vec3<double> &rl, ForceVector &f) const;

    /*
     * Total Energy
     */
    public:
    // Return total energy for the geometry terms involving the specified atom
    double totalGeometryEnergy(const Atom &i) const;
    // Return total energy for the geometry terms in the specified molecule
    double totalGeometryEnergy(const Molecule &mol) const;

    /*
     * Total Force
     */
    public:
    // Calculate total forces within the specified molecule arising from geometry terms
    void totalGeometryForces(const Molecule &mol, ForceVector &f) const;
};
