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

#include "classes/bond.h"
#include "classes/angle.h"
#include "classes/molecule.h"
#include "templates/vector3.h"
#include "templates/list.h"

// Forward Declarations
class Atom;
class Box;
class Grain;
// class Molecule;
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
	void operator=(Configuration &source);
	// Clear all data
	void clear();


	/*!
	 * \name Contents
	 */
	///@{
	private:
	// Periodic Box definition for the Model
	Box* box_;
	// Molecule list
	List<Molecule> molecules_;
	// Number of Grains in array
	int nGrains_;
	// Grain array
	Grain* grains_;
	// Number of Atoms in array
	int nAtoms_;
	// Atom array
	Atom* atoms_;
	// Change counter (incremented when Atom positions change)
	int changeCount_;


	public:
	// Return Box
	const Box* box();
	// Add space for a Molecule of the Species provided
	void addMolecule(Species* sp);
	// Return number of Molecules
	int nMolecules() const;
	// Return first Molecule
	Molecule* molecules();
	// Return number of Grains
	int nGrains() const;
	// Return Grain array
	Grain* grains();
	// Return nth Grain
	Grain& grain(int n);
	// Return number of Atoms
	int nAtoms() const;
	// Return Atom array
	Atom* atoms();
	// Return nth Atom
	Atom& atom(int n);
	// Setup periodic Box
	bool setupBox(double ppRange, Vec3<double> relativeLengths, Vec3<double> angles, double atomicDensity, bool nonPeriodic = FALSE);
	// Instantiate Configuration
	bool instantiate();
	// Identify inter-Grain terms
	void identifyInterGrainTerms();
	// Generate random configuration
	bool randomise();
	// Load starting coordinates from file
	bool loadInitialCoordinates(const char* fileName);
	// Return current change count
	int changeCount();
	// Increment current change count
	void incrementChangeCount();
	///@}


	/*!
	 * \name Intramolecular Terms
	 */
	///@{
	private:
	// List of Bonds
	List<Bond> bonds_;
	// List of Angles
	List<Angle> angles_;

	public:
	// Return number of Bonds
	int nBonds() const;
	// Return first Bond in list
	Bond* bonds();
	// Return nth Bond
	Bond* bond(int n);
	// Return number of Angles
	int nAngles() const;
	// Return first Angle in list
	Angle* angles();
	// Return nth Angle
	Angle* angle(int n);
	///@}


	/*!
	 * \name Box Pass-Thru
	 */
	///@{
	public:
	// Initialise Cells for distribution
	void initialiseCellDistribution();
	// Return next available Cell for calculation
	int nextAvailableCell(bool willBeModified, bool allowRepeats);
	// Unlock Cell specified, once calculation is complete
	bool finishedWithCell(bool willBeModified, int id);
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
