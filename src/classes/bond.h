/*
	*** Bond Definition
	*** src/classes/bond.h
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

#ifndef DUQ_BOND_H
#define DUQ_BOND_H

#include "templates/dynamicarrayobject.h"
#include "templates/reflist.h"

// Forward Declarations
class Atom;
class Molecule;
class SpeciesBond;

// Bond Definition
class Bond : public DynamicArrayObject<Bond>
{
	public:
	// Constructor
	Bond();
	// Destructor
	~Bond();
	// Clear all data
	void clear();


	/*
	 * Parent Data
	 */
	private:
	// Molecule in which the Bond exists
	Molecule* molecule_;
	
	public:
	// Set Molecule in which the Bond exists
	void setMolecule(Molecule* parent);
	// Return Molecule in which the Bond exists
	Molecule* molecule() const;


	/*
	 * Atom Information
	 */
	private:
	// First Atom in Bond
	Atom* i_;
	// Second Atom in Bond
	Atom* j_;

	public:
	// Set Atoms involved in Bond
	void setAtoms(Atom* i, Atom* j);
	// Return first Atom involved in Bond
	Atom* i() const;
	// Return second Atom involved in Bond
	Atom* j() const;
	// Return the 'other' Atom in the Bond
	Atom* partner(Atom* i) const;
	// Return whether Atoms in Bond match those specified
	bool matches(Atom* i, Atom* j) const;


	/*
	 * Connectivity
	 */
	private:
	// Number of Atoms attached to termini
	int nAttached_[2];
	// Arrays of Atoms (in)directly attached to termini
	Atom** attached_[2];

	public:
	// Create attached Atom array
	void createAttachedAtomArray(int terminus, int size);
	// Set attached Atoms for terminus specified
	void setAttachedAtoms(int terminus, const RefList<Atom,int>& atoms);
	// Return number of attached Atoms for terminus specified
	int nAttached(int terminus) const;
	// Return array of attached Atoms for terminus specified
	Atom** attached(int terminus) const;


	/*
	 * Energy / Force
	 */
	private:
	// Species Bond from which parameters are to be taken
	SpeciesBond* speciesBond_;

	public:
	// Set SpeciesBond reference
	void setSpeciesBond(SpeciesBond* bondRef);
	// Return parent Species
	SpeciesBond* speciesBond() const;
	// Return energy for specified distance
	double energy(double distance) const;
	// Return force multiplier for specified distance
	double force(double distance) const;
};

#endif
