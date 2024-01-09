// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

// Permittivity of free space (m-3 kg-1 s4 A2)
#define EPSILON0 8.85418782E-12

// Elementary Charge (C, Amp s)
#define ECHARGE 1.60217646E-19

// Avogadro's Number
#define AVOGADRO 6.0221415E23

// Pi
#define PI 3.141592653589793
#define TWOPI 6.283185307179586
#define SQRTPI 1.772453850905515
#define HALFPI 1.570796326794896

// Degrees per Radian
const double DEGRAD = 180.0 / M_PI;

// Angstroms per Bohr
#define ANGBOHR 0.529177249

// Bohr radius (m)
#define BOHRRADIUS 5.2917720859E-11

// Boltzmann's constant (m2 kg s-2 K-1)
#define BOLTZMANN 1.3806503E-23

// Planck's constant (m2 kg s-1)
#define PLANCK 6.626068E-34

// Planck's constant divided by 2PI (m2 kg s-1)
#define HBAR 1.054571628E10 - 34

// Speed of light (m s-1)
#define SPEEDOFLIGHT 299792458

// 2 * sqrt(2 log 2)
#define TWOSQRT2LN2 2.354820045

/*
 * Conversion from atomic units to kJ/mol for Electrostatic Energy / Forces
 *
 * COULCONVERT =      q * q
 *		 ----------------
 *		 4 * pi * e0 * r2
 */
#define COULCONVERT 1389.35444426359172669289
