/*
	*** Intramolecular Base Class
	*** src/classes/intra.h
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

#ifndef DUQ_INTRA_H
#define DUQ_INTRA_H

#include "templates/dynamicarrayobject.h"
#include "templates/reflist.h"

// Forward Declarations
class Atom;
class Molecule;
class SpeciesAngle;

// Intra Definition
class Intra
{
	public:
	// Constructor
	Intra();
	// Destructor
	~Intra();


	/*
	 * Basic Data
	 */
	protected:
	// Molecule in which the term exists
	Molecule* molecule_;
	
	public:
	// Set Molecule in which the term exists
	void setMolecule(Molecule* parent);
	// Return Molecule in which the term exists
	Molecule* molecule() const;


	/*
	 * Connections
	 */
	private:
	// Number of Atoms attached to termini (number of items stored in attached_ arrays)
	int nAttached_[2];
	// Arrays of Atoms (in)directly attached to termini
	Atom** attached_[2];
	// Size of attached_ Atoms arrays (maximum number of items that may be stored)
	int arraySize_[2];

	public:
	// Clear and delete all arrays
	void deleteAttachedAtomArrays();
	// Set attached Atoms for terminus specified
	void setAttachedAtoms(int terminus, const RefList<Atom,bool>& atoms);
	// Set attached Atoms for terminus specified (single Atom)
	void setAttachedAtoms(int terminus, Atom* atom);
	// Return number of attached Atoms for terminus specified
	int nAttached(int terminus) const;
	// Return array of attached Atoms for terminus specified
	Atom** attached(int terminus) const;
};

#endif
