/*
	*** AtomTypeIndex Definition
	*** src/classes/atomtypeindex.h
	Copyright T. Youngs 2012-2014

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

#ifndef DUQ_ATOMTYPEINDEX_H
#define DUQ_ATOMTYPEINDEX_H

#include "classes/atomtypedata.h"
#include "templates/list.h"
#include "templates/array.h"

// Forward Declarations
class AtomType;
class Isotope;

/*
 * AtomTypeIndex Definition
 */
class AtomTypeIndex
{
	public:
	// Constructor
	AtomTypeIndex();
	// Destructor
	~AtomTypeIndex();
	// Copy Constructor
	AtomTypeIndex(const AtomTypeIndex& source);
	// Assignment Operator
	void operator=(const AtomTypeIndex& source);


	/*
	 * Type List
	 */
	private:
	// List of AtomTypeData
	List<AtomTypeData> types_;

	public:
	// Clear all data
	void clear();
	// Add/increase this AtomType/Isotope pair, returning the index of the AtomType in the list
	int add(AtomType* atomType, Isotope* tope, int popAdd = 0);
	// Return number of AtomType/Isotopes in list
	int nItems() const;
	// Return first item in list
	AtomTypeData* first() const;
	// Print AtomType populations
	void print();


	/*
	 * Access
	 */
	public:
	// Return index of AtomType/Isotope in list
	int indexOf(AtomType* atomtype) const;
	// Return total population of all types in list
	int totalPopulation() const;
	// Finalise list, calculating fractional populations etc.
	void finalise();
	// Return nth referenced AtomType
	AtomType* atomType(int n);
	// Array access operator
	AtomTypeData* operator[](int n);
};

#endif
