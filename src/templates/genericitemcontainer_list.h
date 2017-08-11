/*
	*** Generic Item Container - List<U>
	*** src/templates/genericitemcontainer_list.h
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

#ifndef DUQ_GENERICITEMCONTAINER_LIST_H
#define DUQ_GENERICITEMCONTAINER_LIST_H

#include "templates/genericitemcontainer.h"
#include "templates/broadcastlist.h"

// GenericItemContainer< List<U> >
template <class U> class GenericItemContainer< List<U> > : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer< List<U> >(const char* name, int flags = 0) : GenericItem(name, flags)
	{
		itemClass_ = GenericItem::ListClass;
	}
	// Data item
	List<U> data;


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
