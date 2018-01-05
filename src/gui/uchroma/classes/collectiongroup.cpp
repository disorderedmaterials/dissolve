/*
	*** Collection Group
	*** src/gui/uchroma/classes/collectiongroup.cpp
	Copyright T. Youngs 2013-2018

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
	hasVerticalShift_ = false;
	verticalShift_ = 0.0;

	// Set up the ColourDefinition to be single (stock) colour
	colour_.setColourSource(ColourDefinition::SingleColourSource);
	colour_.setColourScalePoint(ColourDefinition::SingleColourSource, ColourDefinition::stockColour(stockColour_));
}

// Set vertical shift in all Collections in the group via their transform equations
void CollectionGroup::setVerticalShiftInCollections()
{
	RefListIterator<Collection,int> collectionIterator(collections_);
	while (Collection* collection = collectionIterator.iterate())
	{
		collection->setTransformEnabled(1, hasVerticalShift_);
		collection->setTransformEquation(1, CharString("y+%f", verticalShift_));
	}
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

	// Apply vertical shift to the collection if necessary
	if (hasVerticalShift_)
	{
		collection->setTransformEnabled(1, true);
		collection->setTransformEquation(1, CharString("y+%f", verticalShift_));
	}
}

// Remove Collection from group (if it exists)
void CollectionGroup::removeCollection(Collection* collection)
{
	// Remove shift from the collection first, if one is being applied
	if (hasVerticalShift_ && collections_.contains(collection))
	{
		collection->setTransformEnabled(1, false);
		collection->setTransformEquation(1, "y");
	}

	collections_.remove(collection);
}

// Return whether the group is used by the specified collection
bool CollectionGroup::usedByCollection(Collection* collection)
{
	return collections_.contains(collection);
}

// Return whether the group is empty
bool CollectionGroup::isEmpty()
{
	return collections_.nItems() == 0;
}

// Whether vertical shifting is enabled in this group
bool CollectionGroup::setVerticalShift(bool enabled, double verticalShift)
{
	hasVerticalShift_ = enabled;
	verticalShift_ = verticalShift;

	setVerticalShiftInCollections();
}

// Whether vertical shifting is enabled in this group
bool CollectionGroup::hasVerticalShift()
{
	return hasVerticalShift_;
}

// Return shift (in vertical axis) to apply to Collections
double CollectionGroup::verticalShift()
{
	return verticalShift_;
}

/*
 * Collection Group Manager
 */

// Constructor
CollectionGroupManager::CollectionGroupManager()
{
	stockColourUsageCount_.initialise(ColourDefinition::nStockColours);
	verticalShift_ = NoVerticalShift;
}

// Add Collection to its specified group, creating / associating if necessary
CollectionGroup* CollectionGroupManager::addToGroup(Collection* collection)
{
	// If group name specified in Collection is empty, nothing more to do here
	if (!collection->hasGroupName()) return NULL;

	// Does a group with this name already exist?
	CollectionGroup* collectionGroup = group(collection->groupName());
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
		collectionGroup = new CollectionGroup(collection->groupName(), (ColourDefinition::StockColour) lowestId);
		collectionGroups_.own(collectionGroup);
		++stockColourUsageCount_[lowestId];

		// Need to update vertical shifts
		setCollectionGroupShifts();
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

// Return group for specified Collection, if one has been assigned
CollectionGroup* CollectionGroupManager::group(Collection* collection)
{
	for (CollectionGroup* group = collectionGroups_.first(); group != NULL; group = group->next) if (group->usedByCollection(collection)) return group;
	return NULL;
}

// Remove Collection from its specified group
void CollectionGroupManager::removeFromGroup(Collection* collection)
{
	// If group name specified in Collection is empty, nothing more to do here
	if (!collection->hasGroupName()) return;

	// Does a group with this name already exist?
	CollectionGroup* collectionGroup = group(collection->groupName());
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

// Vertical shifts
double VerticalShiftAmounts[] = { 0.0, 0.5, 1.0, 2.0 };

// Set vertical shifts for current CollectionGroups
void CollectionGroupManager::setCollectionGroupShifts()
{
	// Loop over CollectionGroups
	double verticalShift = 0.0;
	for (CollectionGroup* group = collectionGroups_.first(); group != NULL; group = group->next)
	{
		group->setVerticalShift(verticalShift_ > 0, verticalShift);

		// Increase shift amount for the next group
		verticalShift += VerticalShiftAmounts[verticalShift_];
	}
}

// Return colour definition for specified Collection
const ColourDefinition& CollectionGroupManager::colourDefinition(Collection* collection)
{
	CollectionGroup* collectionGroup = group(collection);
	return (collectionGroup ? collectionGroup->colour() : collection->colour());
}

// Cycle vertical shift applied to CollectionGroups
int CollectionGroupManager::cycleVerticalShifts()
{
	verticalShift_ = (CollectionGroupManager::VerticalShift) ((verticalShift_+1)%nVerticalShifts);

	setCollectionGroupShifts();
}

// Set vertical shift applied to CollectionGroups
void CollectionGroupManager::setVerticalShift(VerticalShift shiftType)
{
	verticalShift_ = shiftType;

	setCollectionGroupShifts();
}

// Return current vertical shift type
CollectionGroupManager::VerticalShift CollectionGroupManager::verticalShift() const
{
	return verticalShift_;
}

// Remove all vertical shifts from CollectionGroups
void CollectionGroupManager::removeVerticalShifts()
{
	verticalShift_ = NoVerticalShift;

	setCollectionGroupShifts();
}
