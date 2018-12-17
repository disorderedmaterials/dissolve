/*
	*** Generic Item Container - Array<T>
	*** src/genericitems/array.h
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

#ifndef DISSOLVE_GENERICITEMCONTAINER_ARRAY_H
#define DISSOLVE_GENERICITEMCONTAINER_ARRAY_H

#include "genericitems/container.h"
#include "templates/broadcastarray.h"

// GenericItemContainer< Array<T> >
template <class T> class GenericItemContainer< Array<T> > : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer< Array<T> >(const char* name, int flags = 0) : GenericItem(name, flags)
	{
	}
	// Data item
	Array<T> data;


	/*
	 * Item Class
	 */
	protected:
	// Create a new GenericItem containing same class as current type
	GenericItem* createItem(const char* className, const char* name, int flags = 0)
	{
		if (DissolveSys::sameString(className, itemClassName())) return new GenericItemContainer< Array<T> >(name, flags);
		return NULL;
	}

	public:
	// Return class name contained in item
	const char* itemClassName()
	{
		static CharString className("Array<%s>", T::itemClassName());
		return className.get();
	}


	/*
	 * I/O
	 */
	public:
	// Write data through specified parser
	bool write(LineParser& parser)
	{
		parser.writeLineF("%i\n", data.nItems());
		T* array = data.array();
		for (int n=0; n<data.nItems(); ++n)
		{
			if (!array[n].write(parser)) return false;
		}
		return true;
	}
	// Read data through specified parser
	bool read(LineParser& parser, const CoreData& coreData)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		int nItems = parser.argi(0);
		data.initialise(nItems);

		for (int n=0; n<nItems; ++n) if (!data[n].read(parser, coreData)) return false;
		return true;
	}


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast item contents
	bool broadcast(ProcessPool& procPool, int root)
	{
		bool success;
		BroadcastArray<T>(procPool, root, data, success);
		return success;
	}
	// Return equality between items
	bool equality(ProcessPool& procPool)
	{
		return false;
	}
};

#endif
