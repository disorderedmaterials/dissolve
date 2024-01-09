// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/atomicMasses.h"
#include "base/messenger.h"
#include <vector>

namespace AtomicMass
{

/*
 * Atomic Mass Data
 *
 * Source: CIAAW. Atomic weights of the elements 2017. Available online at www.ciaaw.org
 * (http://www.ciaaw.org/atomic-weights.htm)
 *
 * Where a maximum and minimum mass are available, the average value is taken.
 */

constexpr std::pair<int, double> massData_[] = {
    {Elements::Unknown, 0.0},     // Unknown element
    {Elements::H, 1.00798175},    // Uncertainty = (1), VSMOW
    {Elements::He, 4.002602},     // Uncertainty = (2)
    {Elements::Li, 6.9675},       // Average of {6.938, 6.997}
    {Elements::Be, 9.0121831},    // Uncertainty = (5)
    {Elements::B, 10.8135},       // Average of {10.806, 10.821}
    {Elements::C, 12.0106},       // Average of {12.0096, 12.0116}
    {Elements::N, 14.0069},       // Average of {14.00643, 14.00728}
    {Elements::O, 15.9994},       // Average of {15.99903, 15.99977}
    {Elements::F, 18.998403163},  // Uncertainty = (6)
    {Elements::Ne, 20.1797},      // Uncertainty = (6)
    {Elements::Na, 22.98976928},  // Uncertainty = (2)
    {Elements::Mg, 24.3055},      // Average of {24.304, 24.307}
    {Elements::Al, 26.9815384},   // Uncertainty = (3)
    {Elements::Si, 28.085},       // Average of {28.084, 28.086}
    {Elements::P, 30.973761998},  // Uncertainty = (5)
    {Elements::S, 32.0675},       // Average of {32.059, 32.076}
    {Elements::Cl, 35.4515},      // Average of {35.446, 35.457}
    {Elements::Ar, 39.8775},      // Average of {39.792, 39.963}
    {Elements::K, 39.0983},       // Uncertainty = (1)
    {Elements::Ca, 40.078},       // Uncertainty = (4)
    {Elements::Sc, 44.955908},    // Uncertainty = (5)
    {Elements::Ti, 47.867},       // Uncertainty = (1)
    {Elements::V, 50.9415},       // Uncertainty = (1)
    {Elements::Cr, 51.9961},      // Uncertainty = (6)
    {Elements::Mn, 54.938043},    // Uncertainty = (2)
    {Elements::Fe, 55.845},       // Uncertainty = (2)
    {Elements::Co, 58.933194},    // Uncertainty = (3)
    {Elements::Ni, 58.6934},      // Uncertainty = (4)
    {Elements::Cu, 63.546},       // Uncertainty = (3)
    {Elements::Zn, 65.38},        // Uncertainty = (2)
    {Elements::Ga, 69.723},       // Uncertainty = (1)
    {Elements::Ge, 72.630},       // Uncertainty = (8)
    {Elements::As, 74.921595},    // Uncertainty = (6)
    {Elements::Se, 78.971},       // Uncertainty = (8)
    {Elements::Br, 79.904},       // Average of {79.901, 79.907}
    {Elements::Kr, 83.798},       // Uncertainty = (2)
    {Elements::Rb, 85.4678},      // Uncertainty = (3)
    {Elements::Sr, 87.62},        // Uncertainty = (1)
    {Elements::Y, 88.90584},      // Uncertainty = (1)
    {Elements::Zr, 91.224},       // Uncertainty = (2)
    {Elements::Nb, 92.90637},     // Uncertainty = (1)
    {Elements::Mo, 95.95},        // Uncertainty = (1)
    {Elements::Tc, 98.0},         // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Ru, 101.07},       // Uncertainty = (2)
    {Elements::Rh, 102.90549},    // Uncertainty = (2)
    {Elements::Pd, 106.42},       // Uncertainty = (1)
    {Elements::Ag, 107.8682},     // Uncertainty = (2)
    {Elements::Cd, 112.414},      // Uncertainty = (4)
    {Elements::In, 114.818},      // Uncertainty = (1)
    {Elements::Sn, 118.710},      // Uncertainty = (7)
    {Elements::Sb, 121.760},      // Uncertainty = (1)
    {Elements::Te, 127.60},       // Uncertainty = (3)
    {Elements::I, 126.90447},     // Uncertainty = (3)
    {Elements::Xe, 131.293},      // Uncertainty = (6)
    {Elements::Cs, 132.90545196}, // Uncertainty = (6)
    {Elements::Ba, 137.327},      // Uncertainty = (7)
    {Elements::La, 138.90547},    // Uncertainty = (7)
    {Elements::Ce, 140.116},      // Uncertainty = (1)
    {Elements::Pr, 140.90766},    // Uncertainty = (1)
    {Elements::Nd, 144.242},      // Uncertainty = (3)
    {Elements::Pm, 145.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Sm, 150.36},       // Uncertainty = (2)
    {Elements::Eu, 151.964},      // Uncertainty = (1)
    {Elements::Gd, 157.25},       // Uncertainty = (3)
    {Elements::Tb, 158.925354},   // Uncertainty = (8)
    {Elements::Dy, 162.500},      // Uncertainty = (1)
    {Elements::Ho, 164.930328},   // Uncertainty = (7)
    {Elements::Er, 167.259},      // Uncertainty = (3)
    {Elements::Tm, 168.934218},   // Uncertainty = (6)
    {Elements::Yb, 173.045},      // Uncertainty = (10)
    {Elements::Lu, 174.9668},     // Uncertainty = (1)
    {Elements::Hf, 178.49},       // Uncertainty = (2)
    {Elements::Ta, 180.94788},    // Uncertainty = (2)
    {Elements::W, 183.84},        // Uncertainty = (1)
    {Elements::Re, 186.207},      // Uncertainty = (1)
    {Elements::Os, 190.23},       // Uncertainty = (3)
    {Elements::Ir, 192.217},      // Uncertainty = (2)
    {Elements::Pt, 195.084},      // Uncertainty = (9)
    {Elements::Au, 196.966570},   // Uncertainty = (4)
    {Elements::Hg, 200.592},      // Uncertainty = (3)
    {Elements::Tl, 204.383},      // Average of {204.382, 204.385}
    {Elements::Pb, 207.2},        // Uncertainty = (1)
    {Elements::Bi, 208.98040},    // Uncertainty = (1)
    {Elements::Po, 209.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::At, 210.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Rn, 222.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Fr, 223.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Ra, 226.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Ac, 227.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Th, 232.0377},     // Uncertainty = (4)
    {Elements::Pa, 231.03588},    // Uncertainty = (1)
    {Elements::U, 238.02891},     // Uncertainty = (3)
    {Elements::Np, 237.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Pu, 244.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Am, 243.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Cm, 247.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Bk, 247.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Cf, 251.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Es, 252.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Fm, 257.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Md, 258.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::No, 259.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Lr, 262.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Rf, 267.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Db, 268.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Sg, 269.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Bh, 270.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Hs, 269.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Mt, 278.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Ds, 281.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Rg, 280.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Cn, 285.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Nh, 286.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Fl, 289.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Mc, 289.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Lv, 293.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Ts, 294.0},        // Assumed mass (http://www.rsc.org/periodic-table)
    {Elements::Og, 294.0}         // Assumed mass (http://www.rsc.org/periodic-table)
};

// Return atomic mass for specified Z
double mass(Elements::Element Z) { return massData_[Z].second; }

// Return reduced mass for specified pair of Z's
double reducedMass(Elements::Element Z1, Elements::Element Z2) { return (mass(Z1) * mass(Z2)) / (mass(Z1) + mass(Z2)); }

} // namespace AtomicMass
