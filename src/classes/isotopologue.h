/*
	*** Isotopologue Definition
	*** src/classes/isotopologue.h
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

#ifndef DUQ_ISOTOPOLOGUE_H
#define DUQ_ISOTOPOLOGUE_H

#include "base/dnchar.h"
#include "base/ptable.h"
#include "templates/reflist.h"

// Forward Declarations
class AtomType;
class Species;

/*
 * Isotopologue Definition
 */
class Isotopologue : public ListItem<Isotopologue>
{
	public:
	// Constructor
	Isotopologue();
	// Destructor
	~Isotopologue();


	/*
	 * Basic Information
	 */
	private:
	// Parent Species
	Species* parent_;
	// Descriptive name
	Dnchar name_;
	
	public:
	// Set parent Species
	void setParent(Species* parent);
	// Return parent Species
	Species* parent() const;
	// Set name of Isotopologue
	void setName(const char* name);
	// Return name of Isotopologue
	const char* name() const;
	
	
	/*
	 * Isotope Definition
	 */
	private:
	// List of AtomType references and their assigned Isotopes
	RefList<AtomType,Isotope*> isotopes_;
	
	public:
	// Update AtomType/Isotope RefList
	void update(const List<AtomType>& atomTypes);
	// Set AtomType/Isotope pair in list
	bool setAtomTypeIsotope(AtomType* at, Isotope* isotope);
	// Return Isotope for specified AtomType
	Isotope* atomTypeIsotope(AtomType* at) const;
	// Return first AtomType/Isotope pair
	RefListItem<AtomType,Isotope*>* isotopes() const;
	// Return nth Atom/Isotope pair
	RefListItem<AtomType,Isotope*>* isotope(int n);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(const List<AtomType>& atomTypes);
};

#endif
