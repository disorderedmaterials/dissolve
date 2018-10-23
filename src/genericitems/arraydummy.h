/*
	*** Generic Item Container - Array<DummyClass>
	*** src/genericitems/arraydummy.h
	Copyright T. Youngs 2012-2018

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

#ifndef DISSOLVE_GENERICITEMCONTAINER_ARRAYDUMMY_H
#define DISSOLVE_GENERICITEMCONTAINER_ARRAYDUMMY_H

#include "genericitems/container.h"
#include "base/dummyclass.h"

// GenericItemContainer< Array<DummyClass> >
template <> class GenericItemContainer< Array<DummyClass> > : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer< Array<DummyClass> >(const char* name, int flags = 0) : GenericItem(name, flags)
	{
	}


	/*
	 * Item Class
	 */
	protected:
	// Create a new GenericItem containing same class as current type
	GenericItem* createItem(const char* className, const char* name, int flags = 0)
	{
		if (DissolveSys::sameString(className, "Array<Data1D>")) return new GenericItemContainer< Array<Data1D> >(name, flags);
		return NULL;
	}

	public:
	// Return class name contained in item
	const char* itemClassName()
	{
		return "Array<DummyClass>";
	}


	/*
	 * I/O
	 */
	public:
	// Write data through specified parser
	bool write(LineParser& parser)
	{
		return false;
	}
	// Read data through specified parser
	bool read(LineParser& parser)
	{
		return false;
	}


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast item contents
	bool broadcast(ProcessPool& procPool, int root)
	{
		return false;
	}
	// Return equality between items
	bool equality(ProcessPool& procPool)
	{
		return false;
	}
};

#endif
