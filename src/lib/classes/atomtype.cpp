/*
	*** AtomType Definition
	*** src/lib/classes/atomtype.cpp
	Copyright T. Youngs 2012-2013

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
#include "base/flag.h"
#include "base/comms.h"
#include <string.h>

/*!
 * \brief Constructor
 * \details Constructor for AtomType. 
 */
AtomType::AtomType() : MPIListItem<AtomType>()
{
	element_ = 0;
	parameters_ = NULL;
	name_ = "XX";
}

/*!
 * \brief Destructor
 * \details Constructor for AtomType. 
 */
AtomType::~AtomType()
{
}

/*
// Properties
*/

/*!
 * \brief Set name of AtomType
 */
void AtomType::setName(const char* name)
{
	name_ = name;
	
	SET_MODIFIED
}

/*!
 * \brief Return name of AtomType
 */
const char* AtomType::name() const
{
	return name_.get();
}

/*!
 * \brief Set atomic element
 */
void AtomType::setElement(int el)
{
	element_ = el;
	
	SET_MODIFIED
}

/*!
 * \brief Return atomic element
 */
int AtomType::element() const
{
	return element_;
}

/*!
 * \brief Set associated Parameters
 */
void AtomType::setParameters(Parameters* params)
{
	parameters_ = params;
	
	SET_MODIFIED
}

/*!
 * \brief Return associated Parameters
 */
Parameters* AtomType::parameters() const
{
	return parameters_;
}

/*
// Parallel Comms
*/

/*!
 * \brief Broadcast data from Master to all Slaves
 */
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
	return TRUE;
}
