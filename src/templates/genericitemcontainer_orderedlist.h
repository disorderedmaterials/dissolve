/*
	*** Generic Item Container - OrderedList<U>
	*** src/templates/genericitemcontainer_orderedlist.h
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

#ifndef DUQ_GENERICITEMCONTAINER_ORDEREDLIST_H
#define DUQ_GENERICITEMCONTAINER_ORDEREDLIST_H

#include "templates/genericitemcontainer.h"
#include "templates/orderedlist.h"

// GenericItemContainer< OrderedList<U> >
template <class U> class GenericItemContainer< OrderedList<U> > : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer< OrderedList<U> >(const char* name, int flags = 0) : GenericItem(name, flags)
	{
		itemClass_ = GenericItem::OrderedListClass;
	}
	// Data item
	OrderedList<U> data;


	/*
	 * Read / Write
	 */
	public:
	// Read item contents from specified LineParser
	bool read(LineParser& parser)
	{
		Messenger::error("Reading data into GenericItemContainer< OrderedList<U> > not implemented.\n");
		return false;
	}
	// Write item contents to specified LineParser
	bool write(LineParser& parser)
	{
		Messenger::error("Writing data from GenericItemContainer< OrderedList<U> > not implemented.\n");
		return false;
	}


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast item contents
	bool broadcast(ProcessPool& procPool, int root)
	{
		bool result;
		BroadcastList<U> listBroadcaster(procPool, data, result, root);
		return result;
	}
};

#endif
