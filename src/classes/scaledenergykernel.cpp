/*
	*** ScaledEnergyKernel
	*** src/classes/scaledenergykernel.cpp
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

#include "classes/scaledenergykernel.h"
#include "classes/potentialmap.h"
#include "classes/atom.h"
#include "classes/molecule.h"
#include "classes/box.h"

// Constructor
ScaledEnergyKernel::ScaledEnergyKernel(double interMoleculeRScale, double intraMoleculeEScale, ProcessPool& procPool, Configuration* config, const PotentialMap& potentialMap, double energyCutoff) : EnergyKernel(procPool, config, potentialMap, energyCutoff)
{
	interMoleculeRScale_ = interMoleculeRScale;
	intraMoleculeEScale_ = intraMoleculeEScale;
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
	/*
	 * Check the Molecules of the supplied Atoms - if they exist within different Molecules we scale the distance
	 * between the Atoms, effectively reproducing a scaling of the positions of all Molecular centres in the Box.
	 */
	if (i->molecule() != j->molecule())
	{
		// Get COG of Molecules
		Vec3<double> cogI = i->molecule()->centreOfGeometry(box_);
		Vec3<double> cogJ = j->molecule()->centreOfGeometry(box_);
		double rIJ = box_->minimumDistance(cogI, cogJ);

		return potentialMap_.energy(i, j, r + (rIJ * interMoleculeRScale_ - rIJ));
	}

	return potentialMap_.energy(i, j, r) * intraMoleculeEScale_;
}
