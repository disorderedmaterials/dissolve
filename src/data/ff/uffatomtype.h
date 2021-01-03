// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/ffatomtype.h"
#include "templates/list.h"

// Forward Declarations
/* none */

// Universal Forcefield AtomType Data
class UFFAtomType : public ForcefieldAtomType
{
    public:
    UFFAtomType(int z = 0, int index = -1, std::string_view name = "", std::string_view neta = "",
                std::string_view description = "", double r = 0.0, double theta = 0.0, double x = 0.0, double D = 0.0,
                double zeta = 0.0, double Z = 0.0, double chi = 0.0, int geom = 0, double V = 0.0, double U = 0.0);
    UFFAtomType &operator=(const UFFAtomType &source);

    /*
     * Data
     */
    private:
    // Generator parameters
    double r_, theta_, x_, D_, zeta_, Z_, chi_, V_, U_;
    int geom_;

    public:
    // Return single bond radius (r, Angstroms)
    double r() const;
    // Return descriptive angle (theta, degrees)
    double theta() const;
    // Return nonbond distance (x, Angstroms)
    double x() const;
    // Return nonbond energy (D, kcal)
    double D() const;
    // Return nonbond scale (zeta)
    double zeta() const;
    // Return effective charge (Z)
    double Z() const;
    // Return GMP electronegativity (chi)
    double chi() const;
    // Return torsional parameter 1 (V)
    double V() const;
    // Return torsional parameter 2 (U)
    double U() const;
    // Return integer representing geometry about the type (geom)
    int geom() const;
};
