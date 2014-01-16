/*
	*** Configuration
	*** src/lib/classes/configuration.h
	Copyright T. Youngs 2012-2013

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

#ifndef DUQ_CONFIGURATION_H
#define DUQ_CONFIGURATION_H

#include "classes/molecule.h"
#include "templates/vector3.h"
#include "templates/list.h"
#include "templates/array.h"

// Forward Declarations
class Atom;
class Box;
class Grain;
class Cell;
class Species;

/*!
 * \short Configuration
 * \details A Configuration stores a List of Molecules, coupled with Atom and Grain arrays which fulful the requirements
 * of each Molecule.
 */
class Configuration : public ListItem<Configuration>
{
	public:
	// Constructor
	Configuration();
	// Destructor
	~Configuration();
	// Assignment operator
	void operator=(Configuration& source);
	// Clear all data
	void clear();


	/*!
	 * \name Contents
	 */
	///@{
	private:
	// Molecule list
	List<Molecule> molecules_;
	// Number of grains in array
	int nGrains_;
	// Grain array
	Grain* grains_;
	// Number of atoms in configuration / reference array
	int nAtoms_;
	// Array of atoms
	Atom* atoms_;
	// Change counter (incremented when Atom positions change)
	int changeCount_;


	public:
	// Add space for a molecule of the Species provided
	void addMolecule(Species* sp);
	// Return number of molecules
	int nMolecules() const;
	// Return first molecule
	Molecule* molecules();
	// Return specified molecule
	Molecule* molecule(int id);
	// Return number of grains
	int nGrains() const;
	// Return grain array
	Grain* grains();
	// Return nth grain
	Grain& grain(int n);
	// Return number of atoms
	int nAtoms() const;
	// Return atom reference array
	Atom* atoms();
	// Return nth atom
	Atom* atom(int n);
	// Setup arrays
	bool setupArrays();
	// Setup molecules
	bool setupMolecules(Species& sourceCoordinates);
	// Return current change count
	int changeCount();
	// Increment current change count
	void incrementChangeCount();
	///@}


	/*!
	 * \name Box and Cells
	 */
	///@{
	private:
	// Periodic Box definition for the Model
	Box* box_;
	// Cell divisions along each axis
	Vec3<int> divisions_;
	// Fractional Cell size
	Vec3<double> cellSize_;
	// Real Cell size
	Vec3<double> realCellSize_;
	// Cell extents out from given central cell
	Vec3<int> cellExtents_;
	// Total number of Cells in Box
	int nCells_;
	// Maximum number of Atoms per Cell
	int maxAtomsPerCell_;
	// Cell array (one-dimensional)
	Cell* cells_;
	// Cell modified/completed/calculated flag
	bool* cellFlag_;
	// Counter for distributed Cells
	int nCellsDistributed_;
	// Last Cell distributed
	int lastCellDistributed_;
	
	private:
	// Clear Cell arrays
	void clearCells();
	// Return whether the contents of two Cells should be mim'd in calculations
	bool minimumImageRequired(Cell* a, Cell* b) const;

	public:
	// Return Box
	const Box* box() const;
	// Setup periodic Box
	bool setupBox(double ppRange, Vec3<double> relativeLengths, Vec3<double> angles, double atomicDensity, double cellDensityMultiplier, bool nonPeriodic = false);
	// Generate Cells for Box
	bool generateCells(double cellSize, double pairPotentialRange, double atomicDensity, double cellDensityMultiplier);
	// Return number of Cells for box
	int nCells() const;
	// Return real Cell dimensions
	Vec3<double> realCellSize() const;
	// Retrieve Cell with (wrapped) grid reference specified
	Cell* cell(int x, int y, int z) const;
	// Retrieve Cell with id specified
	Cell* cell(int id) const;
	// Return Cell which contains specified coordinate
	Cell* cell(const Vec3<double> r) const;
	// Return maximum number of atoms per cell
	int maxAtomsPerCell();
	// Return whether two Cells need minimum image calculation
	bool useMim(Cell* a, Cell* b) const;
	// Initialise Cells for distribution
	void initialiseCellDistribution();
	// Return next available Cell for calculation
	int nextAvailableCell(bool willBeModified, bool allowRepeats);
	// Unlock Cell specified, once calculation is complete
	bool finishedWithCell(bool willBeModified, int cellId);
	// Update cell locations of all atoms
	bool updateAtomsInCells();
	// Update cell locations of specified atom index, and update neighbour
	bool updateAtomInCell(int id);
	///@}


	/*!
	 * \name Parallel Comms
	 */
	///@{
	public:
	// Broadcast coordinate data
	bool broadcastCoordinates();
	///@}
};

#endif
