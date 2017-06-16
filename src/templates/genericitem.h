/*
	*** Generic Item
	*** src/templates/genericitem.h
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

#ifndef DUQ_GENERICITEM_H
#define DUQ_GENERICITEM_H

#include "base/processpool.h"

/*
 * GenericItem - Base class for all GenericItemContainers
 */
class GenericItem
{
	public:
	// Constructor
	GenericItem(const char* name)
	{
		name_ = name;
		prev = NULL;
		next = NULL;
	}
	virtual ~GenericItem()
	{
	};
	// Item Flags
	enum Flags
	{
		ModuleOptionFlag	= 1	/* Specifies that the item represents a Module option (i.e. a PlainValue in Module::options_) */
	};
	// List Pointers
	GenericItem* prev, *next;

	private:
	// Name associated to the item
	CharString name_;
	// Description, if any, for the item
	CharString description_;
	// Flags
	int flags_;

	public:
	// Return item name
	const char* name()
	{
		return name_.get();
	}
	// Set item description
	void setDescription(const char* description)
	{
		description_ = description_;
	}
	// Return item description
	const char* description()
	{
		return description_.get();
	}


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast item contsnts
	virtual bool broadcast(ProcessPool& procPool, int root) = 0;
};

/*
 * GenericItemContainer Template Class
 */
template <class T> class GenericItemContainer : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer<T>(const char* name) : GenericItem(name)
	{
	}
	// Data item
	T data;


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast item contsnts
	bool broadcast(ProcessPool& procPool, int root)
	{
		return data.broadcast(procPool, root);
	}
};

/*
 * GenericItemContainer Specialisations
 */

// GenericItemContainer<bool>
template <> class GenericItemContainer<bool> : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer<bool>(const char* name) : GenericItem(name)
	{
	}
	// Data item
	bool data;


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

// GenericItemContainer<int>
template <> class GenericItemContainer<int> : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer<int>(const char* name) : GenericItem(name)
	{
	}
	// Data item
	int data;


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

// GenericItemContainer<double>
template <> class GenericItemContainer<double> : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer<double>(const char* name) : GenericItem(name)
	{
	}
	// Data item
	double data;


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

// GenericItemContainer<CharString>
template <> class GenericItemContainer<CharString> : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer<CharString>(const char* name) : GenericItem(name)
	{
	}
	// Data item
	CharString data;


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


// GenericItemContainer< Array<int> >
template <> class GenericItemContainer< Array<int> > : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer< Array<int> >(const char* name) : GenericItem(name)
	{
	}
	// Data item
	Array<int> data;


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

// GenericItemContainer< Array<double> >
template <> class GenericItemContainer< Array<double> > : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer< Array<double> >(const char* name) : GenericItem(name)
	{
	}
	// Data item
	Array<double> data;


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

// GenericItemContainer< Array< Vec3<double> > >
template <> class GenericItemContainer< Array< Vec3<double> > > : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer< Array< Vec3<double> > >(const char* name) : GenericItem(name)
	{
	}
	// Data item
	Array< Vec3<double> > data;


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

// GenericItemContainer< Array2D<double> >
template <> class GenericItemContainer< Array2D<double> > : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer< Array2D<double> >(const char* name) : GenericItem(name)
	{
	}
	// Data item
	Array2D<double> data;


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
