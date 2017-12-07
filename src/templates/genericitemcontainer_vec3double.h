/*
	*** Generic Item Container - Vec3<double>
	*** src/templates/genericitemcontainer_vec3double.h
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

#ifndef DUQ_GENERICITEMCONTAINER_VEC3DOUBLE_H
#define DUQ_GENERICITEMCONTAINER_VEC3DOUBLE_H

#include "templates/genericitemcontainer.h"

// GenericItemContainer< Vec3<double> >
template <> class GenericItemContainer< Vec3<double> > : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer< Vec3<double> >(const char* name, int flags = 0) : GenericItem(name, flags)
	{
		// Add reference GenericItem class to GenericItem::itemClasses_ list to enable recognition-by-name of class type
		static bool addedToItemClasses_ = false;
		if (!addedToItemClasses_)
		{
			addedToItemClasses_ = true;
			itemClasses_.own(newItem(itemClassName()));
		}
	}
	// Data item
	Vec3<double> data;


	/*
	 * Item Class
	 */
	protected:
	// Create a new GenericItem containing same class as current type
	GenericItem* newItem(const char* name, int flags = 0)
	{
		return new GenericItemContainer< Vec3<double> >(name, flags);
	}

	public:
	// Return class name contained in item
	const char* itemClassName()
	{
		return "Vec3<double>";
	}


	/*
	 * I/O
	 */
	public:
	// Write data through specified parser
	bool write(LineParser& parser)
	{
		return parser.writeLineF("%16.9e %16.9e %16.9e\n", data.x, data.y, data.z);
	}
	// Read data through specified parser
	bool read(LineParser& parser)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		data = parser.arg3d(0);
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
