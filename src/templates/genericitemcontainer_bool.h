/*
	*** Generic Item Container - Bool
	*** src/templates/genericitemcontainer_bool.h
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

#ifndef DUQ_GENERICITEMCONTAINER_BOOL_H
#define DUQ_GENERICITEMCONTAINER_BOOL_H

#include "templates/genericitemcontainer.h"
#include "base/sysfunc.h"

// GenericItemContainer<bool>
template <> class GenericItemContainer<bool> : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer<bool>(const char* name, int flags = 0) : GenericItem(name, flags)
	{
		itemClass_ = GenericItem::BoolClass;
	}
	// Data item
	bool data;


	/*
	 * Read / Write
	 */
	public:
	// Read item contents from specified LineParser
	bool read(LineParser& parser)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != 0) return false;
		data = parser.argb(0);
		return true;
	}
	// Write item contents to specified LineParser
	bool write(LineParser& parser)
	{
		return parser.writeLineF("%s\n", DUQSys::btoa(data));
	}


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast item contsnts
	bool broadcast(ProcessPool& procPool, int root)
	{
		return procPool.broadcast(data, root);
	}
};

#endif
