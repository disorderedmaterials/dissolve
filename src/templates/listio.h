/*
	*** Linked List Class
	*** src/templates/list.h
	Copyright T. Youngs 2012-2018

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

#ifndef DUQ_LISTIO_H
#define DUQ_LISTIO_H

#include "base/lineparser.h"
#include "templates/list.h"

// Forward Declarations
/* none */

/*
 * List Class
 * Linked list for user-defined classes. Any class which is required to be contained in a List must subclass ListItem.
*/
template <class T> class ListIO
{

	/*
	 * Read / Write
	 */
	public:
	// Write list items to specified LineParser
	static bool write(List<T>& list, LineParser& parser)
	{
		if (!parser.writeLineF("%i  # nItems\n", list.nItems())) return false;
		for (T* item = list.first(); item != NULL; item = item->next) if (!item->write(parser)) return false;

		return true;
	}
	// Read list items from specified LineParser
	static bool read(List<T>& list, LineParser& parser)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		list.clear();
		int nItems = parser.argi(0);
		for (int n=0; n<nItems; ++n)
		{
			T* item = list.add();
			if (!item->read(parser)) return false;
		}
		return true;
	}
};

#endif
