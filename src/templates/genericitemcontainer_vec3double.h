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
		itemClass_ = GenericItem::Vec3DoubleClass;
	}
	// Data item
	Vec3<double> data;


	/*
	 * Read / Write
	 */
	public:
	// Read item contents from specified LineParser
	bool read(LineParser& parser)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != 0) return false;
		data = parser.arg3d(0);
		return true;
	}
	// Write item contents to specified LineParser
	bool write(LineParser& parser)
	{
		return parser.writeLineF("%f  %f  %f\n", data.x, data.y, data.z);
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
