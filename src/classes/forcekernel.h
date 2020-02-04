/*
	*** ForceKernel
	*** src/classes/forcekernel.h
	Copyright T. Youngs 2012-2020

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

#ifndef DISSOLVE_FORCEKERNEL_H
#define DISSOLVE_FORCEKERNEL_H

#include "classes/kernelflags.h"
#include "templates/orderedpointerlist.h"
#include "base/processpool.h"

// Forward Declarations
class Atom;
class Box;
class Cell;
class ChangeStore;
class Configuration;
class PotentialMap;
class SpeciesAngle;
class SpeciesBond;
class SpeciesTorsion;

// ForceKernel
class ForceKernel
{
	public:
	// Constructor
	ForceKernel(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz, double cutoffDistance = -1.0);
	// Destructor
	~ForceKernel();


	/*
	 * Source Data
	 */
	protected:
	// Source Configuration
	const Configuration* configuration_;
	// Source Box (from Configuration)
	const Box* box_;
	// Source CellArray (from Configuration)
	const CellArray& cells_;
	// Potential map to use
	const PotentialMap& potentialMap_;
	// Squared cutoff distance to use in calculation
	double cutoffDistanceSquared_;
	// Force array for x component
	Array<double>& fx_;
	// Force array for y component
	Array<double>& fy_;
	// Force array for z component
	Array<double>& fz_;


	/*
	 * Internal Force Calculation
	 */
	private:
	// Calculate inter-particle forces between Atoms provided (no minimum image calculation)
	void forcesWithoutMim(const Atom* i, const Atom* j, double scale = 1.0);
	// Calculate inter-particle forces between Atoms provided (minimum image calculation)
	void forcesWithMim(const Atom* i, const Atom* j, double scale = 1.0);


	/*
	 * PairPotential Terms
	 */
	public:
	// Calculate forces between atoms provided
	void forces(const Atom* i, const Atom* j, bool applyMim, bool excludeIgeJ);
	// Calculate forces between two cells
	void forces(Cell* cell, Cell* otherCell, bool applyMim, bool excludeIgeJ, ProcessPool::DivisionStrategy strategy);
	// Calculate forces between Cell and its neighbours
	void forces(Cell* cell, bool excludeIgeJ, ProcessPool::DivisionStrategy strategy);
	// Calculate forces between Atom and Cell
	void forces(const Atom* i, Cell* cell, int flags, ProcessPool::DivisionStrategy strategy);
	// Calculate forces between atom and world
	void forces(const Atom* i, ProcessPool::DivisionStrategy strategy);


	/*
	 * Intramolecular Terms
	 */
	public:
	// Calculate SpeciesBond forces
	void forces(const SpeciesBond* b, const Atom* i, const Atom* j);
	// Calculate SpeciesBond forces for specified Atom only
	void forces(const Atom* onlyThis, const SpeciesBond* b, const Atom* i, const Atom* j);
	// Calculate SpeciesAngle forces
	void forces(const SpeciesAngle* a, const Atom* i, const Atom* j, const Atom* k);
	// Calculate SpeciesAngle forces for specified Atom only
	void forces(const Atom* onlyThis, const SpeciesAngle* a, const Atom* i, const Atom* j, const Atom* k);
	// Calculate SpeciesTorsion forces
	void forces(const SpeciesTorsion* t, const Atom* i, const Atom* j, const Atom* k, const Atom* l);
	// Calculate SpeciesTorsion forces for specified Atom only
	void forces(const Atom* onlyThis, const SpeciesTorsion* t, const Atom* i, const Atom* j, const Atom* k, const Atom* l);


	/*
	 * Parallel Comms
	 */
	private:
	// Process pool over which this kernel operates
	ProcessPool& processPool_;
};

#endif
