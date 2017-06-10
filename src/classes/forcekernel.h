/*
	*** ForceKernel
	*** src/classes/forcekernel.h
	Copyright T. Youngs 2012-2017

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

#ifndef DUQ_FORCEKERNEL_H
#define DUQ_FORCEKERNEL_H

#include "classes/atom.h"
#include "classes/grain.h"
#include "templates/orderedpointerlist.h"
#include "base/processpool.h"

// Forward Declarations
class Cell;
class Box;
class Configuration;
class PotentialMap;
class ChangeStore;

// ForceKernel
class ForceKernel
{
	public:
	// Constructor
	ForceKernel(ProcessPool& procPool, const Configuration* cfg, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz, double energyCutoff = -1.0);
	// Destructor
	~ForceKernel();
	// Clear all data
	void clear();


	/*
	 * Source Data
	 */
	protected:
	// Source Configuration
	const Configuration* configuration_;
	// Source Box (from Configuration)
	const Box* box_;
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
	// Calculate inter-particle forces between Atom and Grain provided (no minimum image calculation)
	void forcesWithoutMim(const Atom* i, const Grain* grain, bool excludeIgtJ);
	// Calculate inter-particle forces between Grains provided (no minimum image calculation)
	void forcesWithoutMim(const Grain* grainI, const Grain* grainJ);
	// Calculate inter-particle forces between Atoms provided (minimum image calculation)
	void forcesWithMim(const Atom* i, const Atom* j, double scale = 1.0);
	// Calculate inter-particle forces between Atom and Grain provided (minimum image calculation)
	void forcesWithMim(const Atom* i, const Grain* grain, bool excludeIgtJ);
	// Calculate inter-particle forces between Grains provided (minimum image calculation)
	void forcesWithMim(const Grain* grainI, const Grain* grainJ);


	/*
	 * PairPotential Terms
	 */
	public:
	// Calculate inter-particle forces between Atoms provided
	void forces(const Atom* i, const Atom* j, bool applyMim, bool excludeIgtJ);
	// Calculate inter-particle forces between Atom and Grain provided
	void forces(const Atom* i, const Grain* grain, bool applyMim, bool excludeIgtJ);
	// Calculate inter-particle forces between Grains provided
	void forces(const Grain* grainI, const Grain* grainJ, bool applyMim, bool excludeIgtJ);
	// Calculate inter-particle forces between Atom and Cell contents
	void forces(const Atom* i, const Cell* cell, bool applyMim, bool excludeIgtJ);
	// Calculate inter-particle forces between Grain and Cell contents
	void forces(const Grain* grain, const Cell* cell, bool applyMim, bool excludeIgtJ);
	// Calculate inter-particle forces between Atom and list of Cells
	void forces(const Atom* i, int nNeighbours, Cell** neighbours, bool applyMim, bool excludeIgtJ, ProcessPool::LoopContext loopContext = ProcessPool::Individual);
	// Calculate inter-particle forces between Grain and list of Cells
	void forces(const Grain* grain, int nNeighbours, Cell** neighbours, bool applyMim, bool excludeIgtJ, ProcessPool::LoopContext loopContext = ProcessPool::Individual);


	/*
	 * Intramolecular Terms
	 */
	public:
	// Calculate Bond forces
	void forces(const Molecule* mol, const SpeciesBond* b);
	// Calculate Angle forces
	void forces(const Molecule* mol, const SpeciesAngle* a);


	/*
	 * Parallel Comms
	 */
	private:
	// Process pool over which this kernel operates
	ProcessPool& processPool_;
};

#endif
