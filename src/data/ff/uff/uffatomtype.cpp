// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/ff/uff/uffatomtype.h"
#include "data/ff/uff/uff.h"

UFFAtomType::UFFAtomType(Elements::Element Z, int index, std::string_view name, std::string_view neta,
                         std::string_view description, double r, double theta, double x, double D, double zeta,
                         double effectiveCharge, double chi, double V, double U)
    : ForcefieldAtomType(Z, index, name, neta, description, 0.0, {D * 4.184, x / (pow(2.0, 1.0 / 6.0))}), r_(r), theta_(theta),
      x_(x), D_(D), zeta_(zeta), effectiveCharge_(effectiveCharge), chi_(chi), V_(V), U_(U)
{
}

UFFAtomType &UFFAtomType::operator=(const UFFAtomType &source)
{
    r_ = source.r_;
    theta_ = source.theta_;
    x_ = source.x_;
    D_ = source.D_;
    zeta_ = source.zeta_;
    effectiveCharge_ = source.effectiveCharge_;
    chi_ = source.chi_;
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
double UFFAtomType::effectiveCharge() const { return effectiveCharge_; }

// Return GMP electronegativity (chi)
double UFFAtomType::chi() const { return chi_; }

// Return torsional parameter 1 (V)
double UFFAtomType::V() const { return V_; }

// Return torsional parameter 2 (U)
double UFFAtomType::U() const { return U_; }
