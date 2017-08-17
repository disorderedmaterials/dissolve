/*
	*** Generic Item Container - EnumContainer<U>
	*** src/templates/genericitemcontainer_enumcontainer.h
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

#ifndef DUQ_GENERICITEMCONTAINER_ENUMCONTAINER_H
#define DUQ_GENERICITEMCONTAINER_ENUMCONTAINER_H

#include "templates/genericitemcontainer.h"
#include "templates/enumhelpers.h"

// GenericItemContainer< EnumContainer<U> >
template <class U> class GenericItemContainer< EnumContainer<U> > : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer< EnumContainer<U> >(const char* name, int flags = 0) : GenericItem(name, flags)
	{
		itemClass_ = GenericItem::EnumClass;
	}
	// Data item
	EnumContainer<U> data;


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast item contents
	bool broadcast(ProcessPool& procPool, int root)
	{
		// Put the enum in a temporary variable for broadcast
		return procPool.broadcast(EnumCast<U>(data.value()), root);
	}
};

#endif
