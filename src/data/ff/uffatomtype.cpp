// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "data/ff/uffatomtype.h"
#include "data/ff/uff.h"

UFFAtomType::UFFAtomType(int z, int index, std::string_view name, std::string_view neta, std::string_view description, double r,
                         double theta, double x, double D, double zeta, double Z, double chi, int geom, double V, double U)
    : ForcefieldAtomType(z, index, name, neta, description, 0.0, 0.0, 0.0)
{
    // Set the atomtype's data
    r_ = r;
    theta_ = theta;
    x_ = x;
    D_ = D;
    zeta_ = zeta;
    Z_ = Z;
    chi_ = chi;
    geom_ = geom;
    V_ = V;
    U_ = U;
}

UFFAtomType &UFFAtomType::operator=(const UFFAtomType &source)
{
    r_ = source.r_;
    theta_ = source.theta_;
    x_ = source.x_;
    D_ = source.D_;
    zeta_ = source.zeta_;
    Z_ = source.Z_;
    chi_ = source.chi_;
    geom_ = source.geom_;
    V_ = source.V_;
    U_ = source.U_;

    return *this;
}

/*
 * Data
 */

// Return single bond radius (r, Angstroms)
double UFFAtomType::r() const { return r_; }

// Return descriptive angle (theta, degrees)
double UFFAtomType::theta() const { return theta_; }

// Return nonbond distance (x, Angstroms)
double UFFAtomType::x() const { return x_; }

// Return nonbond energy (D, kcal)
double UFFAtomType::D() const { return D_; }

// Return nonbond scale (zeta)
double UFFAtomType::zeta() const { return zeta_; }

// Return effective charge (Z)
double UFFAtomType::Z() const { return Z_; }

// Return GMP electronegativity (chi)
double UFFAtomType::chi() const { return chi_; }

// Return torsional parameter 1 (V)
double UFFAtomType::V() const { return V_; }

// Return torsional parameter 2 (U)
double UFFAtomType::U() const { return U_; }

// Return integer representing geometry about the type (geom)
int UFFAtomType::geom() const { return geom_; }
