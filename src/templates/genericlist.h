/*
	*** Generic List
	*** src/templates/genericlist.h
	Copyright T. Youngs 2012-2016

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

#ifndef DUQ_GENERICLIST_H
#define DUQ_GENERICLIST_H

#include "base/processpool.h"
#include "base/sysfunc.h"
#include "templates/list.h"

// Base class for all GenericItemContainers
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
	// List pointers
	GenericItem* prev, *next;

	private:
	// Name associated to the item
	CharString name_;
	// Description, if any, for the item
	CharString description_;

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

// GenericClassItem Template Class
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
	// Broadcast item contsnts
	bool broadcast(ProcessPool& procPool, int root)
	{
		return data.broadcast(procPool, root);
	}
};

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

// Generic List
class GenericList
{
	private:
	// List of generic items
	List<GenericItem> items_;

	public:
	// Add specified item to list (from base class pointer)
	void add(GenericItem* item)
	{
		items_.own(item);
	}
	// Return whether the named item is contained in the list
	bool contains(const char* name, const char* prefix = NULL)
	{
		// Construct full name
		CharString varName;
		if (DUQSys::isEmpty(prefix)) varName = name;
		else varName.sprintf("%s_%s", prefix, name);

		for (GenericItem* item = items_.first(); item != NULL; item = item->next) if (DUQSys::sameString(item->name(), varName.get())) return true;
		return false;
	}
	// Return the named item from the list
	GenericItem* find(const char* name)
	{
		for (GenericItem* item = items_.first(); item != NULL; item = item->next) if (DUQSys::sameString(item->name(), name)) return item;
		return NULL;
	}
	// Broadcast all data
	bool broadcast(ProcessPool& procPool, int root)
	{
		for (GenericItem* item = items_.first(); item != NULL; item = item->next) if (!item->broadcast(procPool, root)) return false;
		return true;
	}
};

// Generic List Helper
template <class T> class GenericListHelper
{
	public:
	// Add new named item of template-guided type to specified list
	static T& add(GenericList& targetList, const char* name, const char* prefix = NULL)
	{
		// Construct full name
		CharString varName;
		if (DUQSys::isEmpty(prefix)) varName = name;
		else varName.sprintf("%s_%s", prefix, name);

		// Does the named variable already exist in the list?
		GenericItem* existingItem = targetList.find(varName);
		if (existingItem)
		{
			printf("WARNING - Item '%s' already exists in the list - a dummy value will be returned instead.\n", varName.get());
			static T dummy;
			return dummy;
		}

		// Create new item
		GenericItemContainer<T>* newItem = new GenericItemContainer<T>(varName);
		targetList.add(newItem);
		return newItem->data;
	}
	// Retrieve named item from specified list as template-guided type
	static T& retrieve(GenericList& sourceList, const char* name, const char* prefix = NULL, T defaultValue = T())
	{
		// Construct full name
		CharString varName;
		if (DUQSys::isEmpty(prefix)) varName = name;
		else varName.sprintf("%s_%s", prefix, name);

		// Find item in the list
		GenericItem* item = sourceList.find(varName);
		if (!item)
		{
			Messenger::printVerbose("No item named '%s' in list - default value item will be returned.\n", varName.get());
			static T dummy;
			dummy = defaultValue;
			return dummy;
		}

		// Cast to correct type
		GenericItemContainer<T>* castItem = (GenericItemContainer<T>*) item;
		if (!castItem)
		{
			printf("That didn't work, because its of the wrong type.\n");
			static T dummy;
			return dummy;
		}

		return castItem->data;
	}
	// Retrieve (or create and retrieve) named item from specified list as template-guided type
	static T& realise(GenericList& sourceList, const char* name, const char* prefix = NULL, bool* created = NULL)
	{
		// Construct full name
		CharString varName;
		if (DUQSys::isEmpty(prefix)) varName = name;
		else varName.sprintf("%s_%s", prefix, name);

		// Find item in the list - if it isn't there, create it and return
		GenericItem* item = sourceList.find(varName);
		if (!item)
		{
			if (created != NULL) (*created) = true;
			return add(sourceList, name, prefix);
		}

		// Cast to correct type
		GenericItemContainer<T>* castItem = (GenericItemContainer<T>*) item;
		if (!castItem)
		{
			printf("That didn't work, because its of the wrong type.\n");
			static T dummy;
			return dummy;
		}

		if (created != NULL) (*created) = false;
		return castItem->data;
	}
};

#endif
