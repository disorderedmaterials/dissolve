/*
	*** Generic Item Container - Array< Vec3<int> >
	*** src/templates/genericitemcontainer_arrayvec3int.h
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

#ifndef DUQ_GENERICITEMCONTAINER_ARRAYVEC3INT_H
#define DUQ_GENERICITEMCONTAINER_ARRAYVEC3INT_H

#include "templates/genericitemcontainer.h"

// GenericItemContainer< Array< Vec3<int> > >
template <> class GenericItemContainer< Array< Vec3<int> > > : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer< Array< Vec3<int> > >(const char* name, int flags = 0) : GenericItem(name, flags)
	{
		itemClass_ = GenericItem::ArrayVec3IntClass;
	}
	// Data item
	Array< Vec3<int> > data;


	/*
	 * I/O
	 */
	public:
	// Write data through specified parser
	bool write(LineParser& parser)
	{
		parser.writeLineF("%i\n", data.nItems());
		Vec3<int>* array = data.array();
		for (int n=0; n<data.nItems(); ++n)
		{
			if (!parser.writeLineF("%i %i %i\n", array[n].x, array[n].y, array[n].z)) return false;
		}
		return true;
	}
	// Read data through specified parser
	bool read(LineParser& parser)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		int nItems = parser.argi(0);
		data.createEmpty(nItems);
		for (int n=0; n<nItems; ++n)
		{
			if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
			data.add(parser.arg3i(0));
		}
		return true;
	}


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast item contents
	bool broadcast(ProcessPool& procPool, int root)
	{
		return procPool.broadcast(data, root);
	}
};

#endif
