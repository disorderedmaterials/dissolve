/*
	*** SpeciesAngle Definition
	*** src/classes/speciesangle.h
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

#ifndef DUQ_SPECIESANGLE_H
#define DUQ_SPECIESANGLE_H

#include "templates/list.h"
#include "templates/listitem.h"
#include "templates/reflist.h"

// Forward Declarations
class SpeciesAtom;
class Species;
class ProcessPool;

/*
 * Angle Definition
 */
class SpeciesAngle : public ListItem<SpeciesAngle>
{
	public:
	// Constructor
	SpeciesAngle();
	// Destructor
	~SpeciesAngle();


	/*
	 * Basic Data
	 */
	private:
	// Parent Species
	Species* parent_;
	
	public:
	// Set parent Species
	void setParent(Species* parent);
	// Return parent Species
	Species* parent() const;


	/*
	 * Atom Information
	 */
	private:
	// First Atom in Angle
	SpeciesAtom* i_;
	// Second (central) Atom in Angle
	SpeciesAtom* j_;
	// Third Atom in Angle
	SpeciesAtom* k_;

	public:
	// Set Atoms involved in Angle
	void setAtoms(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k);
	// Return first Atom involved in Angle
	SpeciesAtom* i() const;
	// Return second (central) Atom involved in Angle
	SpeciesAtom* j() const;
	// Return third Atom involved in Angle
	SpeciesAtom* k() const;
	// Return index (in parent Species) of first Atom
	int indexI() const;
	// Return index (in parent Species) of second (central) Atom
	int indexJ() const;
	// Return index (in parent Species) of third Atom
	int indexK() const;
	// Return whether Atoms in Angle match those specified
	bool matches(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k) const;


	/*
	 * Interaction Parameters
	 */
	private:
	// Equilibrium angle
	double equilibrium_;
	// Force constant for angle
	double forceConstant_;
	// Number of Atoms attached to termini
	int nAttached_[2];
	// Arrays of SpeciesAtoms (in)directly attached to termini
	SpeciesAtom** attachedAtoms_[2];
	// Arrays of atom indices (in)directly attached to termini
	int* attachedAtomIndices_[2];

	public:
	// Set equilibrium angle
	void setEquilibrium(double rEq);
	// Return equlibrium angle
	double equilibrium() const;
	// Set force constant
	void setForceConstant(double k);
	// Return force constant
	double forceConstant() const;
	// Create attached Atom array
	void createAttachedAtomArrays(int terminus, int size);
	// Set attached Atoms for terminus specified
	void setAttachedAtoms(int terminus, const RefList<SpeciesAtom,int>& atoms);
	// Return number of attached Atoms for terminus specified
	int nAttached(int terminus) const;
	// Return array of attached SpeciesAtoms for terminus specified
	SpeciesAtom** attachedAtoms(int terminus) const;
	// Return array of attached indices for terminus specified
	int* attachedIndices(int terminus) const;
	// Return energy for specified angle
	double energy(double angleInDegrees) const;
	// Return force multiplier for specified angle
	double force(double angleInDegrees) const;


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool, const List<SpeciesAtom>& atoms);
};

#endif
