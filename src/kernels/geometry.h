// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

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
    GeometryKernel(const Configuration *cfg, const PotentialMap &potentialMap, std::optional<double> energyCutoff = {});
    GeometryKernel(const Box *box, const PotentialMap &potentialMap, std::optional<double> energyCutoff = {});
    ~GeometryKernel() = default;

    /*
     * Bond Terms
     */
    public:
    // Return SpeciesBond energy at Atoms specified
    double bondEnergy(const SpeciesBond &b, const Atom &i, const Atom &j) const;
    // Calculate SpeciesBond forces
    void bondForces(const SpeciesBond &bond, const Atom &i, int indexI, const Atom &j, int indexJ, ForceVector &f) const;
    void bondForces(const SpeciesBond &bond, const Vector3 &ri, const Vector3 &rj, ForceVector &f) const;

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
    // Return SpeciesAngle energy at Atoms specified
    double angleEnergy(const SpeciesAngle &a, const Atom &i, const Atom &j, const Atom &k) const;
    // Calculate angle force parameters from supplied vectors
    static AngleParameters calculateAngleForceParameters(Vector3 vecji, Vector3 vecjk);
    // Calculate SpeciesAngle forces
    void angleForces(const SpeciesAngle &angle, const Atom &i, int indexI, const Atom &j, int indexJ, const Atom &k, int indexK,
                     ForceVector &f) const;
    void angleForces(const SpeciesAngle &angle, const Vector3 &ri, const Vector3 &rj, const Vector3 &rk, ForceVector &f) const;

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
    static TorsionParameters calculateTorsionForceParameters(const Vector3 &vecji, const Vector3 &vecjk, const Vector3 &veckl);
    // Calculate SpeciesTorsion forces
    void torsionForces(const SpeciesTorsion &torsion, const Atom &i, int indexI, const Atom &j, int indexJ, const Atom &k,
                       int indexK, const Atom &l, int indexL, ForceVector &f) const;
    void torsionForces(const SpeciesTorsion &torsion, const Vector3 &ri, const Vector3 &rj, const Vector3 &rk,
                       const Vector3 &rl, ForceVector &f) const;

    /*
     * Improper Terms
     */
    public:
    // Return SpeciesImproper energy at Atoms specified
    double improperEnergy(const SpeciesImproper &imp, const Atom &i, const Atom &j, const Atom &k, const Atom &l) const;
    // Calculate SpeciesImproper forces
    void improperForces(const SpeciesImproper &improper, const Atom &i, int indexI, const Atom &j, int indexJ, const Atom &k,
                        int indexK, const Atom &l, int indexL, ForceVector &f) const;
    void improperForces(const SpeciesImproper &improper, const Vector3 &ri, const Vector3 &rj, const Vector3 &rk,
                        const Vector3 &rl, ForceVector &f) const;

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
