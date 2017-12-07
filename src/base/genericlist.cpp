/*
	*** Generic List
	*** src/base/genericlist.cpp
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

#include "base/genericlist.h"

// Add specified item to list (from base class pointer)
void GenericList::add(GenericItem* item)
{
	items_.own(item);
}

// Create an item of the specified type
GenericItem* GenericList::create(const char* name, const char* itemClassName)
{
	// Check for existing item with this name
	GenericItem* newItem = find(name);
	if (newItem)
	{
		Messenger::printVerbose("Item named '%s' already exists in the list, so returning it instead of creating a new one.\n", name);
		return newItem;
	}

	// Create a new item of the specified class type
	newItem = GenericItem::newItem(itemClassName, name);
	if (!newItem)
	{
		Messenger::error("GenericList::create() doesn't know how to create an item of type '%s'\n", itemClassName);
		return NULL;
	}

	// Add the new item to our list
	add(newItem);

	return newItem;
}

// Return whether the named item is contained in the list
bool GenericList::contains(const char* name, const char* prefix)
{
	// Construct full name
	CharString varName;
	if (DUQSys::isEmpty(prefix)) varName = name;
	else varName.sprintf("%s_%s", prefix, name);

	for (GenericItem* item = items_.first(); item != NULL; item = item->next) if (DUQSys::sameString(item->name(), varName.get())) return true;
	return false;
}

// Return head of the list
GenericItem* GenericList::items()
{
	return items_.first();
}

// Return the named item from the list
GenericItem* GenericList::find(const char* name)
{
	for (GenericItem* item = items_.first(); item != NULL; item = item->next) if (DUQSys::sameString(item->name(), name)) return item;
	return NULL;
}

// Return the named item from the list (with prefix)
GenericItem* GenericList::find(const char* name, const char* prefix)
{
	// Construct full name
	CharString varName;
	if (DUQSys::isEmpty(prefix)) varName = name;
	else varName.sprintf("%s_%s", prefix, name);

	for (GenericItem* item = items_.first(); item != NULL; item = item->next) if (DUQSys::sameString(item->name(), varName.get())) return item;
	return NULL;
}

// Return list of all items with specified prefix (before first '_')
RefList<GenericItem,bool> GenericList::listWithPrefix(const char* prefix)
{
	RefList<GenericItem,bool> items;
	CharString itemUniqueName;
	for (GenericItem* item = items_.first(); item != NULL; item = item->next)
	{
		itemUniqueName = DUQSys::beforeChar(item->name(), '_');
		if (itemUniqueName == prefix) items.add(item);
	}

	return items;
}

// List all items
void GenericList::listItems()
{
	int count = 0;
	for (GenericItem* item = items_.first(); item != NULL; item = item->next, ++count) Messenger::print("  %3i  %s", count, item->name());
}

// Remove named item
bool GenericList::remove(const char* name, const char* prefix)
{
	// First, find the named item
	GenericItem* item = find(name, prefix);
	if (!item)
	{
		Messenger::printVerbose("Couldn't find named item '%s' (prefix = '%s') in the list, so can't remove it.\n", name, prefix);
		return false;
	}

	items_.remove(item);
	return true;
}

// Rename item
bool GenericList::rename(const char* oldName, const char* oldPrefix, const char* newName, const char* newPrefix)
{
	// First, find the named item
	GenericItem* item = find(oldName, oldPrefix);
	if (!item)
	{
		Messenger::printVerbose("Couldn't find named item '%s' (prefix = '%s') in the list, so can't rename it.\n", oldName, oldPrefix);
		return false;
	}

	item->setName(CharString("%s_%s", newPrefix, newName));
	return true;
}

// Broadcast all data
bool GenericList::broadcast(ProcessPool& procPool, int root)
{
	for (GenericItem* item = items_.first(); item != NULL; item = item->next) 
	{
		Messenger::printVerbose("Broadcasting data '%s' (%s)...\n", item->name(), item->itemClassName());
		if (!item->broadcast(procPool, root)) return false;
	}
	return true;
}

