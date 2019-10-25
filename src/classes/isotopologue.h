/*
	*** Isotopologue Definition
	*** src/classes/isotopologue.h
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

#ifndef DISSOLVE_ISOTOPOLOGUE_H
#define DISSOLVE_ISOTOPOLOGUE_H

#include "base/charstring.h"
#include "data/elements.h"
#include "templates/list.h"
#include "templates/refdatalist.h"

// Forward Declarations
class AtomType;
class Isotope;
class ProcessPool;
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
	CharString name_;
	
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
	RefDataList<AtomType,Isotope*> isotopes_;
	
	public:
	// Update AtomType/Isotope RefList
	void update();
	// Set AtomType/Isotope pair in list
	bool setAtomTypeIsotope(AtomType* at, Isotope* isotope);
	// Return Isotope for specified AtomType
	Isotope* atomTypeIsotope(AtomType* at) const;
	// Return AtomType/Isotope pairs list
	const RefDataList<AtomType,Isotope*>& isotopes() const;
	// Return nth Atom/Isotope pair
	RefDataItem<AtomType,Isotope*>* isotope(int n);
};

#endif
