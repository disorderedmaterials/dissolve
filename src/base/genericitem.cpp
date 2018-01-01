/*
	*** Generic Item
	*** src/base/genericitem.cpp
	Copyright T. Youngs 2012-2018

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

#include "base/genericitem.h"

// Static Members
List<GenericItem> GenericItem::itemClasses_;

// Constructor
GenericItem::GenericItem(const char* name, int flags)
{
	name_ = name;
	flags_ = flags;
	prev = NULL;
	next = NULL;
}

GenericItem::~GenericItem()
{
};

/*
 * Item Class
 */

// Add class to list of representative itemClasses_
void GenericItem::addItemClass(GenericItem* item)
{
	itemClasses_.own(item);
}

// Return new, empty GenericItem containing the class specified
GenericItem* GenericItem::newItem(const char* className, const char* name, int flags)
{
	// Search through registered item classes list for one matching the class name provided
	for (GenericItem* item = itemClasses_.first(); item != NULL; item = item->next)
	{
		// See if the item can create a GenericItem of the desired type. If it can't, move on
		GenericItem* newItem = item->createItem(className, name, flags);
		if (newItem != NULL) return newItem;
	}

	return NULL;
}

/*
 * Item Contents
 */

// Set item name
void GenericItem::setName(const char* name)
{
	name_ = name;
}

// Return item name
const char* GenericItem::name()
{
	return name_.get();
}

// Set item description
void GenericItem::setDescription(const char* description)
{
	description_ = description;
}

// Return item description
const char* GenericItem::description()
{
	return description_.get();
}

// Set flags for item
void GenericItem::setFlags(int flags)
{
	flags_ = flags;
}

// Return flags
int GenericItem::flags()
{
	return flags_;
}

