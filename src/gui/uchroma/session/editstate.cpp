/*
	*** EditState
	*** src/gui/uchroma/session/editstate.cpp
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

#include "gui/uchroma/session/editstate.h"
#include "base/messenger.h"
#include "gui/uchroma/uchromabase.h"

// Constructor
EditState::EditState(ObjectInfo target, int quantity) : targetObject_(target)
{
	targetObject_ = target;
	targetQuantity_ = quantity;
	targetIndex_ = -1;
	targetSubIndex_ = -1;
}

/*
 * Target Data
 */

// Search for specified integer data in list
int EditState::integerData(const char* name, const List<EditStateData>& dataList, bool& result)
{
	result = true;
	for (EditStateData* data = dataList.first(); data != NULL; data = data->next)
	{
		if (data->type() != EditStateData::IntegerData) continue;
		if (data->isName(name)) return data->asInt();
	}
	printf("Internal Error: No stored integer data exists named '%s'.\n", name);
	result = false;
	return 0;
}

// Search for specified double data in list
double EditState::doubleData(const char* name, const List<EditStateData>& dataList, bool& result)
{
	result = true;
	for (EditStateData* data = dataList.first(); data != NULL; data = data->next)
	{
		if (data->type() != EditStateData::DoubleData) continue;
		if (data->isName(name)) return data->asDouble();
	}
	printf("Internal Error: No stored double data exists named '%s'.\n", name);
	result = false;
	return 0.0;
}

// Search for specified string data in list
const char* EditState::stringData(const char* name, const List<EditStateData>& dataList, bool& result)
{
	result = true;
	for (EditStateData* data = dataList.first(); data != NULL; data = data->next)
	{
		if (data->type() != EditStateData::StringData) continue;
		if (data->isName(name)) return data->asString();
	}
	printf("Internal Error: No stored string data exists named '%s'.\n", name);
	result = false;
	return "STRING NOT FOUND";
}

// Search for specified Collection data in list
Collection& EditState::collectionData(const char* name, const List<EditStateData>& dataList, bool& result)
{
	result = true;
	for (EditStateData* data = dataList.first(); data != NULL; data = data->next)
	{
		if (data->type() != EditStateData::CollectionData) continue;
		if (data->isName(name)) return data->asCollection();
	}
	printf("Internal Error: No stored Collection data exists named '%s'.\n", name);
	static Collection dummy;
	result = false;
	return dummy;
}

// Search for specified XYData data in list
XYData& EditState::data2DData(const char* name, const List<EditStateData>& dataList, bool& result)
{
	result = true;
	for (EditStateData* data = dataList.first(); data != NULL; data = data->next)
	{
		if (data->type() != EditStateData::XYDataData) continue;
		if (data->isName(name)) return data->asXYData();
	}
	printf("Internal Error: No stored XYData data exists named '%s'.\n", name);
	static XYData dummy;
	result = false;
	return dummy;
}

// Search for specified LineStyle data in list
LineStyle EditState::lineStyleData(const char* name, const List<EditStateData>& dataList, bool& result)
{
	result = true;
	for (EditStateData* data = dataList.first(); data != NULL; data = data->next)
	{
		if (data->type() != EditStateData::LineStyleData) continue;
		if (data->isName(name)) return data->asLineStyle();
	}
	printf("Internal Error: No stored LineStyle data exists named '%s'.\n", name);
	result = false;
	return LineStyle();
}

// Set target indices
void EditState::setTargetIndices(int index, int subIndex)
{
	targetIndex_ = index;
	targetSubIndex_ = subIndex;
}

// Add old data
EditStateData* EditState::addOldData()
{
	return oldData_.add();
}

// Add new data
EditStateData* EditState::addNewData()
{
	return newData_.add();
}

/*
 * Apply / Revert
 */

// Add : OLD=NULL, NEW=COLLECTION
// Remove : OLD=COLLECTION, NEW=NULL
// Apply change (old -> new)
bool EditState::makeChanges(UChromaBase* uChromaBase, bool revert)
{
	// What is the target data type?
	bool result = false;
	switch (targetObject_.type())
	{
		case (ObjectInfo::UChromaAxesObject):
			Messenger::printVerbose("EDITSTATE: Axes - %s change type %i.\n", revert ? "reversing" : "applying", targetQuantity_);
			result = changeAxes(revert ? oldData_ : newData_, revert);
			break;
		case (ObjectInfo::UChromaCollectionObject):
			Messenger::printVerbose("EDITSTATE: Collection - %s change type %i.\n", revert ? "reversing" : "applying", targetQuantity_);
			// Check whether we need to add or remove collection
			if ((targetQuantity_ == EditState::CollectionAddQuantity) || (targetQuantity_ == EditState::CollectionRemoveQuantity))
			{
				// If we are 'applying' the change, we need to add the Collection in the newData_ list back into UChroma
				// If we are 'reversing' the change, we must find and remove the Collection specified
				bool add = targetQuantity_ == EditState::CollectionAddQuantity;
				if ( (add && revert) || ((!add) && (!revert)) )
				{
					Messenger::printVerbose("EDITSTATE: Collection - remove collection with id %i\n", targetObject_.id());
					Collection* toDelete = Collection::object(targetObject_.id());
					if (toDelete == NULL)
					{
						printf("Internal Error - Couldn't find collection with id %i, in order to delete it.\n", targetObject_.id());
						return false;
					}
					Messenger::printVerbose("EDITSTATE: Collection - removing collection '%s'\n", qPrintable(toDelete->name()));
					uChromaBase->removeCollection(toDelete);
				}
				else
				{
					Messenger::printVerbose("EDITSTATE: Collection - add collection with id %i\n", targetObject_.id());
					// Create new Collection in the correct place, and copy our stored data to it...
					Collection& collection = collectionData("collection", add ? newData_ : oldData_, result);
					if (!result)
					{
						Messenger::printVerbose("Internal Error - Couldn't retrieve collection data for recreation.\n");
						return false;
					}
					int position;
					CharString locator = stringData("locator", add ? newData_ : oldData_, result);
					if (result) position = integerData("position", add ? newData_ : oldData_, result);
					if (!result)
					{
						Messenger::printVerbose("Internal Error - Couldn't retrieve locator string or position for recreation of collection.\n");
						return false;
					}
					Collection* newCollection = uChromaBase->addCollectionFromLocator(locator, collection.type(), position);
					(*newCollection) = collection;
					Messenger::printVerbose("EDITSTATE: Collection - changing object id to %i\n", targetObject_.id());
					Collection::setObjectId(newCollection, targetObject_.id());
					printf("Recreated collection is now id %i\n", newCollection->objectId());
				}
				result = true;
			}
			else result = changeCollection(revert ? oldData_ : newData_, revert);
			break;
		case (ObjectInfo::UChromaViewPaneObject):
			Messenger::printVerbose("EDITSTATE: ViewPane - %s change type %i.\n", revert ? "reversing" : "applying", targetQuantity_);
			result = changeViewPane(revert ? oldData_ : newData_, revert);
			break;
		default:
			Messenger::printVerbose("Internal Error: Type of targetObject_ (%i) in EditState::makeChanges() not accounted for.\n", targetObject_.type());
			return false;
	}
	return result;
}
