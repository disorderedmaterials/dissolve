/*
	*** Collection Group
	*** src/gui/uchroma/classes/collectiongroup.h
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

#ifndef DUQ_UCHROMACOLLECTIONGROUP_H
#define DUQ_UCHROMACOLLECTIONGROUP_H

#include "gui/uchroma/classes/colourdef.h"
#include "base/charstring.h"
#include "base/sysfunc.h"
#include "templates/list.h"
#include "templates/listitem.h"
#include "templates/reflist.h"

// Forward declarations
class Collection;

// Collection Group
class CollectionGroup : public ListItem<CollectionGroup>
{
	public:
	// Constructor
	CollectionGroup(const char* name, ColourDefinition::StockColour colour);

	private:
	// Name of the group
	CharString name_;
	// Collections using this group
	RefList<Collection,int> collections_;
	// Colour associated to this group
	ColourDefinition::StockColour stockColour_;
	// Colour definition for this group
	ColourDefinition colour_;
	// Shift in vertical axis to apply to Collections
	// TODO

	public:
	// Return name of group
	const char* name() const;
	// Return colour associated to the group
	ColourDefinition::StockColour stockColour() const;
	// Return colour definition for the group
	const ColourDefinition& colour() const;
	// Associate Collection to group (if it isn't already)
	void associateCollection(Collection* collection);
	// Remove Collection from group (if it exists)
	void removeCollection(Collection* collection);
	// Return whether the group is empty
	bool isEmpty();
};

// Collection Group Manager
class CollectionGroupManager
{
	public:
	// Constructor
	CollectionGroupManager();

	private:
	// List of current CollectionGroups in use
	static List<CollectionGroup> collectionGroups_;
	// Usage counters for stock colours
	static int* stockColourUsageCount_;

	public:
	// Add Collection to its specified group, creating / associating if necessary
	static CollectionGroup* addToGroup(Collection* collection);
	// Return named group, if it exists
	static CollectionGroup* group(const char* name);
	// Remove Collection from its specified group
	static void removeFromGroup(Collection* collection);
};

#endif
