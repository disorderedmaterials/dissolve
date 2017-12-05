/*
	*** Collection Group
	*** src/gui/uchroma/classes/collectiongroup.cpp
	Copyright T. Youngs 2013-2017

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/uchroma/classes/collectiongroup.h"
#include "gui/uchroma/classes/collection.h"

/*
 * Collection Group
 */

// Constructor
CollectionGroup::CollectionGroup(const char* name, ColourDefinition::StockColour colour) : ListItem<CollectionGroup>()
{
	name_ = name;
	stockColour_ = colour;

	// Set up the ColourDefinition to be single (stock) colour
	colour_.setColourSource(ColourDefinition::SingleColourSource);
	colour_.setColourScalePoint(ColourDefinition::SingleColourSource, ColourDefinition::stockColour(stockColour_));
}

// Return name of group
const char* CollectionGroup::name() const
{
	return name_.get();
}

// Return colour associated to the group
ColourDefinition::StockColour CollectionGroup::stockColour() const
{
	return stockColour_;
}

// Return colour definition for the group
const ColourDefinition& CollectionGroup::colour() const
{
	return colour_;
}

// Associate Collection to group (if it isn't already)
void CollectionGroup::associateCollection(Collection* collection)
{
	collections_.addUnique(collection);
}

// Remove Collection from group (if it exists)
void CollectionGroup::removeCollection(Collection* collection)
{
	collections_.remove(collection);
}

// Return whether the group is empty
bool CollectionGroup::isEmpty()
{
	return collections_.nItems() == 0;
}

/*
 * Collection Group Manager
 */

// Static Members
List<CollectionGroup> CollectionGroupManager::collectionGroups_;
int* CollectionGroupManager::stockColourUsageCount_ = new int[ColourDefinition::nStockColours];

// Constructor
CollectionGroupManager::CollectionGroupManager()
{
}

// Add Collection to its specified group, creating / associating if necessary
CollectionGroup* CollectionGroupManager::addToGroup(Collection* collection)
{
	// If group name specified in Collection is empty, nothing more to do here
	if (!collection->hasGroup()) return NULL;

	// Does a group with this name already exist?
	CollectionGroup* collectionGroup = group(collection->group());
	if (!collectionGroup)
	{
		// No existing group, so must add a new one
		// First, find the StockColour with the lowest usage count
		int lowestId = 0;
		for (int colourId = 0; colourId < ColourDefinition::nStockColours; ++colourId)
		{
			if (stockColourUsageCount_[colourId] < stockColourUsageCount_[lowestId]) lowestId = colourId;
		}

		// TODO This will create inconsistencies in colouring if we do not EditState this change
		collectionGroup = new CollectionGroup(collection->group(), (ColourDefinition::StockColour) lowestId);
		collectionGroups_.own(collectionGroup);
		++stockColourUsageCount_[lowestId];
	}

	// Add unique Collection reference to the group
	collectionGroup->associateCollection(collection);
	return collectionGroup;
}

// Return named group, if it exists
CollectionGroup* CollectionGroupManager::group(const char* name)
{
	for (CollectionGroup* group = collectionGroups_.first(); group != NULL; group = group->next) if (DUQSys::sameString(group->name(), name)) return group;
	return NULL;
}

// Remove Collection from its specified group
void CollectionGroupManager::removeFromGroup(Collection* collection)
{
	// If group name specified in Collection is empty, nothing more to do here
	if (!collection->hasGroup()) return;

	// Does a group with this name already exist?
	CollectionGroup* collectionGroup = group(collection->group());
	if (!collectionGroup) return;

	collectionGroup->removeCollection(collection);

	// If the group is now empty, we can delete it
	if (collectionGroup->isEmpty())
	{
		// TODO This will create inconsistencies in colouring if we do not EditState this change
		--stockColourUsageCount_[collectionGroup->stockColour()];
		collectionGroups_.remove(collectionGroup);
	}
}
