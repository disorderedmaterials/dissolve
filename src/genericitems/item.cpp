/*
	*** Generic Item
	*** src/genericitems/item.cpp
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

#include "genericitems/item.h"

// Static Members
List<GenericItem> GenericItem::itemClasses_;

// Constructor
GenericItem::GenericItem(const char* name, int flags) : ListItem<GenericItem>()
{
	name_ = name;
	flags_ = flags;
	version_ = 1;
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
	for (GenericItem* item = itemClasses_.first(); item != NULL; item = item->next())
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

// Return version of the item
int GenericItem::version() const
{
	return version_;
}

// Set version of the item
void GenericItem::setVersion(int version)
{
	version_ = version;
}

// Bump the version of the item
void GenericItem::bumpVersion()
{
	++version_;
}

// Set flags for item
void GenericItem::setFlags(int flags)
{
	flags_ = flags;
}

// Return flags
int GenericItem::flags() const
{
	return flags_;
}

// Return whether specified flag is set
bool GenericItem::hasFlag(GenericItem::ItemFlag flag) const
{
	return (flags_&flag);
}
