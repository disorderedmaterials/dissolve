/*
	*** List Broadcaster
	*** src/templates/broadcastlist.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_BROADCASTLIST_H
#define DISSOLVE_BROADCASTLIST_H

#include "base/processpool.h"
#include "templates/mpilistitem.h"

// BroadcastList
template <class T> class BroadcastList
{
	/*
	 * Constructor-only template class which iterates over a supplied list, broadcasting the object from master
	 * to slave processes. The List must contain items which subclass MPIListItem, in order to provide the 'broadcast()' virtual.
	 */
	private:
	// Result of broadcast
	bool result_;

	public:
	// Constructor
	BroadcastList(ProcessPool& procPool, int root, List<T>& items, const CoreData& coreData)
	{
		result_ = false;
		int count;
		if (procPool.isMaster())
		{
			// Broadcast number of items in list, then list items...
			count = items.nItems();
			if (!procPool.broadcast(count, root)) return;
			for (MPIListItem<T>* item = items.first(); item != NULL; item = item->next()) if (!item->broadcast(procPool, root, coreData)) return;
		}
		else
		{
			// Get number of list items to expect
			if (!procPool.broadcast(count, root)) return;

			// Clear list and reconstruct
			items.clear();
			for (int n=0; n<count; ++n)
			{
				// Slaves must create a suitable structure first, and then join the broadcast
				T* item = items.add();
				if (!item->broadcast(procPool, root, coreData)) return;
			}
		}

		// All OK - success!
		result_ = true;
	}
	// Return whether broadcast was successful
	bool success()
	{
		return result_;
	}
	// Return whether broadcast failed
	bool failed()
	{
		return (!result_);
	}
};

#endif
