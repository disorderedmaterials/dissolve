/*
	*** AtomTypeData Definition
	*** src/classes/atomtypedata.h
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

#ifndef DUQ_ATOMTYPEDATA_H
#define DUQ_ATOMTYPEDATA_H

#include "templates/list.h"

// Forward Declarations
class AtomType;
class Isotope;

/*
 * AtomTypeData Definition
 */
class AtomTypeData : public ListItem<AtomTypeData>
{
	public:
	// Constructor
	AtomTypeData();
	// Copy Constructor
	AtomTypeData(const AtomTypeData& source);
	// Assignment Operator
	void operator=(const AtomTypeData& source);


	/*
	 * Properties
	 */
	private:
	// Reference AtomType
	AtomType* atomType_;
	// Isotopes
	Isotope* isotope_;
	// Integer populations of isotopes
	int population_;
	// Fractional populations of isotopes
	double fraction_;
	// Index of non-isotopic master type
	int masterIndex_;

	public:
	// Initialise
	bool initialise(AtomType* atomType, Isotope* topeA);
	// Add to population of Isotope
	void add(int nAdd);
	// Zero population
	void zero();
	// Return reference AtomType
	AtomType* atomType() const;
	// Finalise, calculating fractional populations etc.
	void finalise(int totalAtoms);
	// Return Isotope
	Isotope* isotope() const;
	// Return population
	int population() const;
	// Return fractional population
	double fraction() const;
	// Return referenced AtomType name
	const char* name();
	// Set index of non-isotopic master type
	void setMasterIndex(int id);
	// Return index of non-isotopic master type
	int masterIndex();
};

#endif
