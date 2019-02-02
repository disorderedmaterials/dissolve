/*
	*** Array Broadcaster
	*** src/templates/broadcastarray.h
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

#ifndef DISSOLVE_BROADCASTARRAY_H
#define DISSOLVE_BROADCASTARRAY_H

#include "base/processpool.h"
#include "templates/mpilistitem.h"

// BroadcastArray
template <class T> class BroadcastArray
{
	/*
	 * Constructor-only template class which iterates over a supplied Array, broadcasting the object from master
	 * to slave processes. The Array items must be trivially copyable (via an assignment operator) and broadcastable.
	 */
	public:
	// Constructor
	BroadcastArray(ProcessPool& procPool, int root, Array<T>& array, bool& result)
	{
		result = false;
		int count;
		if (procPool.isMaster())
		{
			// Broadcast number of items in list, then list items...
			count = array.nItems();
			if (!procPool.broadcast(count, root)) return;
			for (int n=0; n<count; ++n) array[count].broadcast(procPool, root);
		}
		else
		{
			// Get number of list items to expect
			if (!procPool.broadcast(count, root)) return;

			// Clear list and reconstruct
			array.initialise(count);
			for (int n=0; n<count; ++n) array[n].broadcast(procPool, root);
		}

		// All OK - success!
		result = true;
	};
};

#endif
