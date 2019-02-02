/*
	*** Generic List
	*** src/base/genericlist.cpp
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

#include "base/genericlist.h"

// Clear all items
void GenericList::clear()
{
	items_.clear();
}

// Add specified item to list (from base class pointer)
void GenericList::add(GenericItem* item)
{
	items_.own(item);
}

// Create an item of the specified type
GenericItem* GenericList::create(const char* name, const char* itemClassName, int version)
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

	// Set its version
	newItem->setVersion(version);

	return newItem;
}

// Return whether the named item is contained in the list
bool GenericList::contains(const char* name, const char* prefix)
{
	// Construct full name
	CharString varName;
	if (DissolveSys::isEmpty(prefix)) varName = name;
	else varName.sprintf("%s_%s", prefix, name);

	for (GenericItem* item = items_.first(); item != NULL; item = item->next) if (DissolveSys::sameString(item->name(), varName.get())) return true;
	return false;
}

// Return if named named item, if it exists, is of specified type
bool GenericList::isItemOfType(const char* type, const char* name, const char* prefix)
{
	GenericItem* item = find(name, prefix);
	if (!item) return false;

	return DissolveSys::sameString(type, item->itemClassName());
}

// Return head of the list
GenericItem* GenericList::items()
{
	return items_.first();
}

// Return the named item from the list
GenericItem* GenericList::find(const char* name)
{
	for (GenericItem* item = items_.first(); item != NULL; item = item->next) if (DissolveSys::sameString(item->name(), name)) return item;
	return NULL;
}

// Return the named item from the list (with prefix)
GenericItem* GenericList::find(const char* name, const char* prefix)
{
	// Construct full name
	CharString varName;
	if (DissolveSys::isEmpty(prefix)) varName = name;
	else varName.sprintf("%s_%s", prefix, name);

	for (GenericItem* item = items_.first(); item != NULL; item = item->next) if (DissolveSys::sameString(item->name(), varName.get())) return item;
	return NULL;
}

// Return the version of the named item from the list
int GenericList::version(const char* name, const char* prefix) const
{
	// Construct full name
	CharString varName;
	if (DissolveSys::isEmpty(prefix)) varName = name;
	else varName.sprintf("%s_%s", prefix, name);

	for (GenericItem* item = items_.first(); item != NULL; item = item->next) if (DissolveSys::sameString(item->name(), varName.get())) return item->version();

	return -99;
}

// Return list of all items with specified prefix (before first '_')
RefList<GenericItem,bool> GenericList::listWithPrefix(const char* prefix)
{
	RefList<GenericItem,bool> items;
	CharString itemUniqueName;
	for (GenericItem* item = items_.first(); item != NULL; item = item->next)
	{
		itemUniqueName = DissolveSys::beforeChar(item->name(), '_');
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

	if (DissolveSys::isEmpty(newPrefix)) item->setName(newName);
	else item->setName(CharString("%s_%s", newPrefix, newName));

	return true;
}

/*
 * Parallel Comms
 */

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

// Check equality of all data
bool GenericList::equality(ProcessPool& procPool)
{
	// Loop over processes - the target process will loop over its module data, sending out the name to the slaves.
	// If found, we note that we've already done this data, check it for equality, and move on.
	// If we can't find the data, we'll complain but move on.
	RefList<GenericItem,bool> checkedItems;
	CharString itemName, itemClassName;
	int nFailed = 0;
	for (int n=0; n<procPool.nProcesses(); ++n)
	{
		// The master process - rank 'n' - will control the loop
		if (procPool.poolRank() == n)
		{
			// Loop over GenericItems in list
			for (GenericItem* item = items_.first(); item != NULL; item = item->next)
			{
				// If we have already checked this item, move on...
				if (checkedItems.contains(item)) continue;

				// Found an item that hasn't already been checked, so flag to the slaves that we will be sending another name to check
				if (!procPool.decideTrue(n)) return false;

				Messenger::printVerbose("Checking equality of item '%s'...\n", item->name());

				// Send name of item - slaves will try to find it, so we need to check that everybody does...
				itemName = item->name();
				if (!procPool.broadcast(itemName, n)) return false;
				if (!procPool.allTrue(true))
				{
					Messenger::error("GenericList equality check failed - item '%s' is not present on all processes.\n", item->name());
					++nFailed;
					continue;
				}

				// Send class name of item - slaves will check it, and everybody must agree
				itemClassName = item->itemClassName();
				if (!procPool.broadcast(itemClassName, n)) return false;
				if (!procPool.allTrue(true))
				{
					Messenger::error("GenericList equality check failed - item '%s' is not of the same type (%s) on all processes.\n", item->name(), item->itemClassName());
					++nFailed;
					continue;
				}

				// Check the item for equality
				if (!item->equality(procPool)) return Messenger::error("Failed equality check for module data '%s'.\n", item->name());

				// Add the item to our checked list
				checkedItems.add(item);
			}

			// No more items, so send the 'false' signal
			if (!procPool.decideFalse(n)) return false;
		}
		else while (procPool.decision(n))
		{
			// Receive the name of the item we're targetting
			if (!procPool.broadcast(itemName, n)) return false;

			Messenger::printVerbose("Checking equality of item '%s'...\n", itemName.get());

			// Do we have an item of this name?
			GenericItem* item = find(itemName);
			if (!procPool.allTrue(item))
			{
				Messenger::error("GenericList equality check failed - item '%s' not found on process %i.\n", item->name(), procPool.poolRank());
				++nFailed;
				continue;
			}

			// Receive the class name of the item and check it against ours
			if (!procPool.broadcast(itemClassName, n)) return false;
			if (!procPool.allTrue(DissolveSys::sameString(item->itemClassName(), itemClassName)))
			{
				Messenger::error("GenericList equality check failed - item '%s' not of the correct type (is %s) on process %i.\n", item->name(), item->itemClassName(), procPool.poolRank());
				++nFailed;
				continue;
			}

			// Check the item for equality
			if (!item->equality(procPool)) return Messenger::error("Failed equality check for module data '%s'.\n", item->name());

			// Add the item to our checked list
			checkedItems.add(item);
		}
	}

	// Did we succeed?
	return (nFailed == 0);
}
