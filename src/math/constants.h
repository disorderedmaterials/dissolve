/*
	*** Numerical constants
	*** src/math/constants.h
	Copyright T. Youngs 2012-2018

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "base/constants.h"

#ifndef DISSOLVE_CONSTANTS_H
#define DISSOLVE_CONSTANTS_H

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
#define DEGRAD 57.295779578552

// Angstroms per Bohr
#define ANGBOHR 0.529177249

// Bohr radius (m)
#define BOHRRADIUS 5.2917720859E-11

// Boltzmann's constant (m2 kg s-2 K-1)
#define BOLTZMANN 1.3806503E-23

// Planck's constant (m2 kg s-1)
#define PLANCK 6.626068E-34

// Planck's constant divided by 2PI (m2 kg s-1)
#define HBAR 1.054571628E10-34

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

#endif
