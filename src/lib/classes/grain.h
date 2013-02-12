/*
	*** Grain
	*** src/lib/classes/grain.h
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

#ifndef DUQ_GRAIN_H
#define DUQ_GRAIN_H

#include "classes/atom.h"
#include "math/matrix3.h"
#include "templates/list.h"
#include "templates/reflist.h"

// Forward Declarations
class Angle;
class Box;
class Cell;
class GrainDefinition;
class PotentialMap;

/*!
 * \brief Grain
 * \details A Grain contains pointers to one or more Atoms that should be treated as an individual 'group'
 * when calculating neighbour lists, low-Q scattering etc. TODO
 * Grains represent proper instances of a GrainDefinition, and are used within a Model to provide the basic blocks which are
 * manipulated during the main calculation.
 */
class Grain
{
	public:
	// Constructor
	Grain();
	// Destructor
	~Grain();
	// Clear all data
	void clear();


	/*!
	 * \name Atoms / Character
	 */
	///@{
	private:
	// Source GrainDefinition
	GrainDefinition* grainDefinition_;
	// Parent Molecule
	Molecule* parent_;
	// Number of Atoms contained in this Grain
	int nAtoms_;
	// Array size
	int atomsArraySize_;
	// Pointers to Atoms
	Atom** atoms_;
	// Index of Grain
	int index_;

	public:
	// Initialise from GrainDefinition
	bool initialise(GrainDefinition* gd);
	// Initialise
	bool initialise(int maxAtoms, int newIndex = -1);
	// Set parent Molecule
	void setParent(Molecule* mol);
	// Return parent Molecule
	Molecule* parent() const;
	// Add Atom pointer to list
	bool addAtom(Atom* i);
	// Return number of Atoms in Grain
	int nAtoms() const;
	// Return nth Atom in Grain
	Atom* atom(int n) const;
	// Set index of Grain
	void setIndex(int index);
	// Return index of Grain
	int index() const;
	// Return name of Grain
	const char* name() const;
	///@}


	/*!
	 * \name Connections
	 * \details Each Grain maintains a list of connections it has with other Grains, which arise because of intramolecular atomic terms.
	 * In this way, once a Grain has been moved the resulting effect on the intramolecular terms present in the encompassing Molecule
	 * can be immediately evaluated, and the inter-Grain energy corrected for PairPotential interactions which should not have been added
	 * into the total inter-Grain energy in the first place (because two Atoms are bound, for example).
	 */
	///@{
	private:
	// Pointers to local intra-Grain Bonds
	RefList<Bond,int> internalBonds_;
	// Pointers to local intra-Grain Angles
	RefList<Angle,int> internalAngles_;
	// Pointers to local inter-Grain Bonds
	RefList<Bond,int> bondConnections_;
	// Pointers to local inter-Grain Angles
	RefList<Angle,int> angleConnections_;

	public:
	// Clear all intra- and inter-Grain terms
	void clearIntramolecular();
	// Add intra-Grain Bond
	void addInternalBond(Bond* b);
	// Return first local intra-Grain Bond
	RefListItem<Bond,int>* internalBonds() const;
	// Add intra-Grain Angle
	void addInternalAngle(Angle* a);
	// Return first local intra-Grain Angle
	RefListItem<Angle,int>* internalAngles() const;
	// Add Bond connection
	void addBondConnection(Bond* b);
	// Return first Bond connection
	RefListItem<Bond,int>* bondConnections() const;
	// Add Angle connection
	void addAngleConnection(Angle* a);
	// Return first Angle connection
	RefListItem<Angle,int>* angleConnections() const;
	///@}


	/*!
	 * \name Coordinates / Manipulation
	 */
	///@{
	private:
	// Centre of geometry of Grain
	Vec3<double> centre_;
	// Cell location
	Cell* cell_;
	// Local index of Grain in Cell's list
	int localIndex_;
	
	public:
	// Adjust centre
	void updateCentre(const Vec3<double>& atomDeltaR);
	// Return centre of geometry of Grain
	const Vec3<double>& centre() const;
	// Set Cell location
	void setCell(Cell* cell, int index);
	// Remove Grain from its current Cell
	void removeFromCell(Cell* caller);
	// Return Cell location
	Cell* cell() const;
	// Return local index of Grain in Cell's list
	int cellListIndex() const;
	// Move Grain centre
	void moveTo(const Vec3<double>& delta);
	// Translate Grain centre
	void translate(const Vec3<double>& delta);
	///@}
};

#endif
