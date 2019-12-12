/*
	*** Generic List
	*** src/genericitems/list.h
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

#ifndef DISSOLVE_GENERICLIST_H
#define DISSOLVE_GENERICLIST_H

#include "genericitems/item.h"
#include "base/sysfunc.h"
#include "templates/list.h"
#include "templates/reflist.h"

// Generic List
class GenericList
{
	/*
	 * List Contents
	 */
	private:
	// List of generic items
	List<GenericItem> items_;

	public:
	// Clear all items
	void clear();
	// Add specified item to list (from base class pointer)
	void add(GenericItem* item);
	// Create an item of the specified type
	GenericItem* create(const char* name, const char* itemClassName, int version = 0);
	// Return whether the named item is contained in the list
	bool contains(const char* name, const char* prefix = NULL);
	// Return if named item, if it exists, is of specified type
	bool isItemOfType(const char* type, const char* name, const char* prefix = NULL);
	// Return item list
	List<GenericItem>& items();
	// Return the named item from the list
	GenericItem* find(const char* name);
	// Return the named item from the list (with prefix)
	GenericItem* find(const char* name, const char* prefix);
	// Return the version of the named item from the list
	int version(const char* name, const char* prefix = NULL) const;
	// Return list of all items with specified prefix (before first '_')
	RefList<GenericItem> itemsWithPrefix(const char* prefix);
	// Return list of all items with specified class type
	RefList<GenericItem> itemsWithClassName(const char* className);
	// List all items
	void listItems();
	// Remove named item
	bool remove(const char* name, const char* prefix);
	// Rename item
	bool rename(const char* oldName, const char* oldPrefix, const char* newName, const char* newPrefix);
	// Prune all items with '@suffix'
	void pruneWithSuffix(const char* suffix);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast all data
	bool broadcast(ProcessPool& procPool, const int root, const CoreData& coreData);
	// Check equality of all data
	bool equality(ProcessPool& procPool);
};

#endif
