/*
	*** AtomType Definition
	*** src/classes/atomtype.cpp
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

#include "classes/atomtype.h"
#include "base/ptable.h"
#include "base/comms.h"
#include <string.h>

// Constructor
AtomType::AtomType() : MPIListItem<AtomType>()
{
	element_ = 0;
	parameters_ = NULL;
	name_ = "XX";
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
void AtomType::setElement(int el)
{
	element_ = el;
}

// Return atomic element
int AtomType::element() const
{
	return element_;
}

// Set associated Parameters
void AtomType::setParameters(Parameters* params)
{
	parameters_ = params;
}

// Return associated Parameters
Parameters* AtomType::parameters() const
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

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool AtomType::broadcast()
{
#ifdef PARALLEL
	int index;

	// Send name
	Comm.broadcast(name_);
	
	// Send element
	Comm.broadcast(&element_, 1);
	
	// Get index of Parameters, 
	if (Comm.master()) index = PeriodicTable::element(element_).indexOfParameters(parameters_);
	Comm.broadcast(&index, 1);
	parameters_ = PeriodicTable::element(element_).parameters(index);
#endif
	return true;
}
