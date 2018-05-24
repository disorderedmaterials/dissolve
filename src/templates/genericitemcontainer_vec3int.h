/*
	*** Generic Item Container - Vec3<Int>
	*** src/templates/genericitemcontainer_vec3int.h
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

#ifndef DISSOLVE_GENERICITEMCONTAINER_VEC3INT_H
#define DISSOLVE_GENERICITEMCONTAINER_VEC3INT_H

#include "templates/genericitemcontainer.h"

// GenericItemContainer< Vec3<int> >
template <> class GenericItemContainer< Vec3<int> > : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer< Vec3<int> >(const char* name, int flags = 0) : GenericItem(name, flags)
	{
	}
	// Data item
	Vec3<int> data;


	/*
	 * Item Class
	 */
	protected:
	// Create a new GenericItem containing same class as current type
	GenericItem* createItem(const char* className, const char* name, int flags = 0)
	{
		if (DissolveSys::sameString(className, itemClassName())) return new GenericItemContainer< Vec3<int> >(name, flags);
		return NULL;
	}

	public:
	// Return class name contained in item
	const char* itemClassName()
	{
		return "Vec3<int>";
	}


	/*
	 * I/O
	 */
	public:
	// Write data through specified parser
	bool write(LineParser& parser)
	{
		return parser.writeLineF("%i  %i  %i\n", data.x, data.y, data.z);
	}
	// Read data through specified parser
	bool read(LineParser& parser)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		data = parser.arg3i(0);
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
	// Check item equality
	bool equality(ProcessPool& procPool)
	{
		if (!procPool.equality(data.x)) return false;
		if (!procPool.equality(data.y)) return false;
		if (!procPool.equality(data.z)) return false;
		return true;
	}
};

#endif
