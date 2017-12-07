/*
	*** Generic Item Container - Array< Vec3<double> >
	*** src/templates/genericitemcontainer_arrayvec3double.h
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

#ifndef DUQ_GENERICITEMCONTAINER_ARRAYVEC3DOUBLE_H
#define DUQ_GENERICITEMCONTAINER_ARRAYVEC3DOUBLE_H

#include "templates/genericitemcontainer.h"

// GenericItemContainer< Array< Vec3<double> > >
template <> class GenericItemContainer< Array< Vec3<double> > > : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer< Array< Vec3<double> > >(const char* name, int flags = 0) : GenericItem(name, flags)
	{
	}
	// Data item
	Array< Vec3<double> > data;


	/*
	 * Item Class
	 */
	protected:
	// Create a new GenericItem containing same class as current type
	GenericItem* newItem(const char* name, int flags = 0)
	{
		return new GenericItemContainer< Array< Vec3<double> > >(name, flags);
	}

	public:
	// Return class name contained in item
	const char* itemClassName()
	{
		return "Array<Vec3<double>>";
	}


	/*
	 * I/O
	 */
	public:
	// Write data through specified parser
	bool write(LineParser& parser)
	{
		parser.writeLineF("%i\n", data.nItems());
		Vec3<double>* array = data.array();
		for (int n=0; n<data.nItems(); ++n)
		{
			if (!parser.writeLineF("%16.9e %16.9e %16.9e\n", array[n].x, array[n].y, array[n].z)) return false;
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
			data.add(parser.arg3d(0));
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
