/*
	*** Isotopologue Set
	*** src/classes/isotopologueset.h
	Copyright T. Youngs 2012-2016

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

#ifndef DUQ_ISOTOPOLOGUESET_H
#define DUQ_ISOTOPOLOGUESET_H

#include "classes/atomtypelist.h"
#include "classes/isotopologuemix.h"
#include "templates/list.h"

// Forward Declarations
/* none */

// Isotopologue Set
class IsotopologueSet
{
	public:
	// Constructor
	IsotopologueSet();
	// Copy Constructor
	IsotopologueSet(const IsotopologueSet& source);
	// Assignment operator
	void operator=(const IsotopologueSet& source);


	/*
	 * Isotopologue Definition
	 */
	private:
	// List of IsotopologueMix-tures for Species
	List<IsotopologueMix> isotopologueMixtures_;
	// Type list derived from Species referenced in isotopologueMixtures_
	AtomTypeList atomTypes_;

	public:
	// Update IsotopologueMix data
	void updateIsotopologueMixtures(const List<Species>& species);
	// Return whether the set contains a mixtures definition for the provided Species
	IsotopologueMix* hasSpeciesIsotopologueMixture(Species* sp) const;
	// Add Isotopologue to the relevant mixture
	bool addIsotopologue(Species* sp, Isotopologue* iso, double relPop);
	// Return first IsotopologueMix
	IsotopologueMix* isotopologueMixtures() const;
	// Return nth IsotopologueMix
	IsotopologueMix* isotopologueMixture(int n);
	// Assign default (first) Isotopologues for all Species
	void assignDefaultIsotopes();

	public:
	// Create type list
	bool createTypeList(const List<Species>& allSpecies, const List<AtomType>& masterIndex);
	// Print full isotopologue information
	void print();
	// Return AtomTypeList
	AtomTypeList& atomTypes();
	// Return number of used AtomTypes
	int nUsedTypes();


	/*
	 * Parallel Comms
	 */

	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool, const List<Species>& species);
};

#endif
