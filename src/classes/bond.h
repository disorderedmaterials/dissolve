/*
	*** Bond Definition
	*** src/classes/bond.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_BOND_H
#define DISSOLVE_BOND_H

#include "classes/intra.h"
#include "templates/dynamicarrayobject.h"
#include "templates/reflist.h"

// Forward Declarations
class Atom;
class Molecule;
class SpeciesBond;

// Bond Definition
class Bond : public Intra, public DynamicArrayObject<Bond>
{
	public:
	// Constructor
	Bond();
	// Destructor
	~Bond();


	/*
	 * DynamicArrayObject Virtuals
	 */
	public:
	// Clear object, ready for re-use
	void clear();


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
	 * Energy / Force
	 */
	private:
	// Species Bond from which parameters are to be taken
	SpeciesBond* speciesBond_;

	public:
	// Set SpeciesBond reference
	void setSpeciesBond(SpeciesBond* bondRef);
	// Return SpeciesBond reference
	SpeciesBond* speciesBond() const;
	// Return energy for specified distance
	double energy(double distance) const;
	// Return force multiplier for specified distance
	double force(double distance) const;
};

#endif
