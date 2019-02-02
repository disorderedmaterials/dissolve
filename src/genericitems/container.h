/*
	*** Generic Item Container
	*** src/genericitems/container.h
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

#ifndef DISSOLVE_GENERICITEMCONTAINER_H
#define DISSOLVE_GENERICITEMCONTAINER_H

#include "base/processpool.h"
#include "base/genericitem.h"

// GenericItemContainer Template Class
template <class T> class GenericItemContainer : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer<T>(const char* name, int flags = 0) : GenericItem(name, flags)
	{
	}
	// Data item
	T data;


	/*
	 * Item Class
	 */
	protected:
	// Create a new GenericItem containing same class as current type
	GenericItem* createItem(const char* className, const char* name, int flags = 0)
	{
		if (DissolveSys::sameString(className, itemClassName())) return new GenericItemContainer<T>(name, flags);
		return NULL;
	}

	public:
	// Return class name contained in item
	virtual const char* itemClassName()
	{
		return T::itemClassName();
	}


	/*
	 * I/O
	 */
	public:
	// Write data through specified parser
	bool write(LineParser& parser)
	{
		return data.write(parser);
	}
	// Read data through specified parser
	bool read(LineParser& parser, const CoreData& coreData)
	{
		return data.read(parser, coreData);
	}


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast item contents
	bool broadcast(ProcessPool& procPool, int root)
	{
		return data.broadcast(procPool, root);
	}
	// Check for equality
	bool equality(ProcessPool& procPool)
	{
		return data.equality(procPool);
	}
};

#endif
