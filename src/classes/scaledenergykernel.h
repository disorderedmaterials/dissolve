/*
	*** ScaledEnergyKernel
	*** src/classes/scaledenergykernel.h
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

#ifndef DUQ_SCALEDENERGYKERNEL_H
#define DUQ_SCALEDENERGYKERNEL_H

#include "classes/energykernel.h"

// Forward Declarations
/* none */

// Scaled Energy Kernel
class ScaledEnergyKernel : public EnergyKernel
{
	public:
	// Constructor
	ScaledEnergyKernel(double interMoleculeRScale, double intraMoleculeEScale, ProcessPool& procPool, Configuration* config, const PotentialMap& potentialMap, double energyCutoff = -1.0);
	// Destructor
	~ScaledEnergyKernel();
	// Clear all data
	void clear();


	/*
	 * Scale Factor
	 */
	private:
	// Scale factor to apply to Molecule distances used when calculating interatomic PairPotential energies
	double interMoleculeRScale_;
	// Scale factor to apply to intermolecular PairPotential energies arising from interatomic interactions within the same Molecule
	double intraMoleculeEScale_;


	/*
	 * Internal Routines
	 */
	private:
	// Return PairPotential energy between atoms provided as pointers, at the distance specified
	double pairPotentialEnergy(const Atom* i, const Atom* j, double r);
};

#endif
