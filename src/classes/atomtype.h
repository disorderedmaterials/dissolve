/*
	*** AtomType Definition
	*** src/classes/atomtype.h
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

#ifndef DISSOLVE_ATOMTYPE_H
#define DISSOLVE_ATOMTYPE_H

#include "data/ff.h"
#include "base/charstring.h"
#include "base/parameters.h"
#include "templates/listitem.h"

// Forward Declarations
class Element;

// AtomType Definition
class AtomType : public ListItem<AtomType>
{
	public:
	// Constructor
	AtomType();
	// Destructor
	~AtomType();


	/*
	 * Character
	 */
	private:
	// Name
	CharString name_;
	// Associated Element
	Element* element_;

	public:
	// Set name of AtomType
	void setName(const char* name);
	// Return name of AtomType
	const char* name() const;
	// Set atomic element
	void setElement(Element* el);
	// Return atomic Element
	Element* element() const;


	/*
	 * Interaction Parameters
	 */
	private:
	// Short-range interaction type
	Forcefield::ShortRangeType shortRangeType_;
	// Interatomic interaction parameters (including charge)
	InteractionParameters parameters_;
	// Whether this AtomType is exchangeable
	bool exchangeable_;
	// Index of this type in the master type index
	int index_;

	public:
	// Set short-range interaction type
	void setShortRangeType(Forcefield::ShortRangeType srType);
	// Return short-range interaction type
	Forcefield::ShortRangeType shortRangeType() const;
	// Return interatomic interaction interaction parameters
	InteractionParameters& parameters();
	// Set index of this type in the master type index
	void setIndex(int id);
	// Return index of this type in the master type index
	int index() const;
};

#endif
