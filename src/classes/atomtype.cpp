/*
	*** AtomType Definition
	*** src/classes/atomtype.cpp
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

#include "classes/atomtype.h"
#include "data/elements.h"
#include "base/processpool.h"
#include <string.h>

// Constructor
AtomType::AtomType() : ListItem<AtomType>()
{
	element_ = NULL;
	name_ = "XX";
	exchangeable_ = false;
}

// Destructor
AtomType::~AtomType()
{
}

/*
 * Properties
 */

// Set name of AtomType
void AtomType::setName(const char* name)
{
	name_ = name;
}

// Return name of AtomType
const char* AtomType::name() const
{
	return name_.get();
}

// Set atomic element
void AtomType::setElement(Element* el)
{
	element_ = el;
}

// Return atomic element
Element* AtomType::element() const
{
	return element_;
}

// Return interaction Parameters
Parameters& AtomType::parameters()
{
	return parameters_;
}

// Set index of this type in the main type index
void AtomType::setIndex(int id)
{
	index_ = id;
}

// Return index of this type in the main type index
int AtomType::index() const
{
	return index_;
}
