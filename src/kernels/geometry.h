// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "kernels/base.h"
#include "kernels/common.h"
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
    GeometryKernel(const Configuration *cfg, const PotentialMap &potentialMap);
    GeometryKernel(const Box *box, const PotentialMap &potentialMap);
    ~GeometryKernel() = default;

    /*
     * Bond Terms
     */
    public:
    // Return SpeciesBond energy using specified positions
    double bondEnergy(const SpeciesBond &b, const Vector3 &ri, const Vector3 &rj) const;
    // Calculate SpeciesBond forces
    void bondForces(const SpeciesBond &bond, const Atom &i, int indexI, const Atom &j, int indexJ,
                    std::vector<Vector3> &f) const;
    void bondForces(const SpeciesBond &bond, const Vector3 &ri, const Vector3 &rj, std::vector<Vector3> &f) const;

    /*
     * Angle Terms
     */
    private:
    struct AngleParameters
    {
        AngleParameters() = default;
        double theta;
        Vector3 dfi_dtheta;
        Vector3 dfk_dtheta;
    };

    public:
    // Return SpeciesAngle energy using specified positions
    double angleEnergy(const SpeciesAngle &a, const Vector3 &ri, const Vector3 &rj, const Vector3 &rk) const;
    // Calculate angle force parameters from supplied vectors
    static AngleParameters calculateAngleForceParameters(Vector3 vecji, Vector3 vecjk);
    // Calculate SpeciesAngle forces
    void angleForces(const SpeciesAngle &angle, const Atom &i, int indexI, const Atom &j, int indexJ, const Atom &k, int indexK,
                     std::vector<Vector3> &f) const;
    void angleForces(const SpeciesAngle &angle, const Vector3 &ri, const Vector3 &rj, const Vector3 &rk,
                     std::vector<Vector3> &f) const;

    /*
     * Torsion Terms
     */
    private:
    struct TorsionParameters
    {
        TorsionParameters() = default;
        double phi;
        Matrix3 dxpj_dij;
        Matrix3 dxpj_dkj;
        Matrix3 dxpk_dkj;
        Matrix3 dxpk_dlk;
        Vector3 dcos_dxpj;
        Vector3 dcos_dxpk;
    };
    // Add torsion forces for atom 'i' in 'i-j-k-l' into the specified vector index
    void addTorsionForceI(double du_dphi, int index, TorsionParameters &torsionParameters, std::vector<Vector3> &f) const;
    // Sum torsion forces for atom 'j' in 'i-j-k-l' into the specified vector index
    void addTorsionForceJ(double du_dphi, int index, TorsionParameters &torsionParameters, std::vector<Vector3> &f) const;
    // Sum torsion forces for atom 'k' in 'i-j-k-l' into the specified vector index
    void addTorsionForceK(double du_dphi, int index, TorsionParameters &torsionParameters, std::vector<Vector3> &f) const;
    // Sum torsion forces for atom 'l' in 'i-j-k-l' into the specified vector index
    void addTorsionForceL(double du_dphi, int index, TorsionParameters &torsionParameters, std::vector<Vector3> &f) const;

    public:
    // Return SpeciesTorsion energy using specified positions
    double torsionEnergy(const SpeciesTorsion &t, const Vector3 &ri, const Vector3 &rj, const Vector3 &rk,
                         const Vector3 &rl) const;
    // Calculate torsion force parameters from supplied vectors
    static TorsionParameters calculateTorsionForceParameters(const Vector3 &vecji, const Vector3 &vecjk, const Vector3 &veckl);
    // Calculate SpeciesTorsion forces
    void torsionForces(const SpeciesTorsion &torsion, const Atom &i, int indexI, const Atom &j, int indexJ, const Atom &k,
                       int indexK, const Atom &l, int indexL, std::vector<Vector3> &f) const;
    void torsionForces(const SpeciesTorsion &torsion, const Vector3 &ri, const Vector3 &rj, const Vector3 &rk,
                       const Vector3 &rl, std::vector<Vector3> &f) const;

    /*
     * Improper Terms
     */
    public:
    // Return SpeciesImproper energy using specified positions
    double improperEnergy(const SpeciesImproper &imp, const Vector3 &ri, const Vector3 &rj, const Vector3 &rk,
                          const Vector3 &rl) const;
    // Calculate SpeciesImproper forces
    void improperForces(const SpeciesImproper &improper, const Atom &i, int indexI, const Atom &j, int indexJ, const Atom &k,
                        int indexK, const Atom &l, int indexL, std::vector<Vector3> &f) const;
    void improperForces(const SpeciesImproper &improper, const Vector3 &ri, const Vector3 &rj, const Vector3 &rk,
                        const Vector3 &rl, std::vector<Vector3> &f) const;

    /*
     * Total Energy
     */
    public:
    // Return geometry energy for the specified atom
    Kernel::GeometryEnergyValue totalGeometryEnergy(const Atom &i) const;
    // Return geometry energy for the specified molecule
    Kernel::GeometryEnergyValue totalGeometryEnergy(const Molecule &mol) const;

    /*
     * Total Force
     */
    public:
    // Calculate total forces within the specified molecule arising from geometry terms
    void totalGeometryForces(const Molecule &mol, std::vector<Vector3> &f) const;
};
