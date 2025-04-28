// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

// Avogadro's Number
#define AVOGADRO 6.0221415E23

// Pi
#define PI 3.141592653589793
#define TWOPI 6.283185307179586

// Degrees per Radian
const double DEGRAD = 180.0 / M_PI;

// Boltzmann's constant (m2 kg s-2 K-1)
#define BOLTZMANN 1.3806503E-23

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
