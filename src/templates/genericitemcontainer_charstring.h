/*
	*** Generic Item Container - CharString
	*** src/templates/genericitemcontainer_charstring.h
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

#ifndef DUQ_GENERICITEMCONTAINER_CHARSTRING_H
#define DUQ_GENERICITEMCONTAINER_CHARSTRING_H

#include "templates/genericitemcontainer.h"

// GenericItemContainer<CharString>
template <> class GenericItemContainer<CharString> : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer<CharString>(const char* name, int flags = 0) : GenericItem(name, flags)
	{
		itemClass_ = GenericItem::CharStringClass;
	}
	// Data item
	CharString data;


	/*
	 * I/O
	 */
	public:
	// Write data through specified parser
	bool write(LineParser& parser)
	{
		return parser.writeLineF("%s\n", data.get());
	}
	// Read data through specified parser
	bool read(LineParser& parser)
	{
		if (parser.readNextLine(LineParser::Defaults) == LineParser::Success) return false;
		data = parser.line();
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
