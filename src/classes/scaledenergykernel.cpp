/*
	*** ScaledEnergyKernel
	*** src/classes/scaledenergykernel.cpp
	Copyright T. Youngs 2012-2018

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "classes/scaledenergykernel.h"
#include "classes/potentialmap.h"

// Constructor
ScaledEnergyKernel::ScaledEnergyKernel(double ppRScale, ProcessPool& procPool, Configuration* config, const PotentialMap& potentialMap, double energyCutoff) : EnergyKernel(procPool, config, potentialMap, energyCutoff)
{
	pairPotentialRScale_ = ppRScale;
}

// Destructor
ScaledEnergyKernel::~ScaledEnergyKernel()
{
}

/*
 * Internal Routines
 */

// Return PairPotential energy between atoms provided as pointers, at the distance specified
double ScaledEnergyKernel::pairPotentialEnergy(const Atom* i, const Atom* j, double r)
{
	return potentialMap_.energy(i, j, r*pairPotentialRScale_);
}
