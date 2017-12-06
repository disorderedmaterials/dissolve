/*
	*** Generic Item Container - Array<T>
	*** src/templates/genericitemcontainer_array.h
	Copyright T. Youngs 2012-2017

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

#ifndef DUQ_GENERICITEMCONTAINER_ARRAY_H
#define DUQ_GENERICITEMCONTAINER_ARRAY_H

#include "templates/genericitemcontainer.h"
#include "templates/broadcastarray.h"

// GenericItemContainer< Array<T> >
template <class T> class GenericItemContainer< Array<T> > : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer< Array<T> >(const char* name, int flags = 0) : GenericItem(name, flags)
	{
		itemClass_ = GenericItem::ArrayIntClass;
	}
	// Data item
	Array<T> data;


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
	bool read(LineParser& parser)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		int nItems = parser.argi(0);
		data.createEmpty(nItems);

		// TODO - How can this be effectively templated through the class instances in the array
		return false;
// 		for (int n=0; n<nItems; ++n)
// 		{
// 			if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
// 			data.add(parser.argd(0));
// 		}
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
};

#endif
