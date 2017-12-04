/*
	*** UChroma Base - Collections
	*** src/gui/uchroma/uchromabase_collections.cpp
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

#include "gui/uchroma/uchromabase.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include <QStringList>

// Return unique name based on supplied baseName
const char* UChromaBase::uniqueCollectionName(const char* baseName)
{
	static CharString testName;

	testName = baseName;
	int index = 0;
	Collection* collection;
	do
	{
		// Add on suffix (if index > 0)
		if (index > 0) testName.sprintf("%s (%i)", baseName, index);
		++index;
		for (collection = collections_.first(); collection != NULL; collection = collection->next) if (DUQSys::sameString(collection->name(), testName)) break;
	} while (collection);

	return testName.get();
}

// Setup new, empty session
void UChromaBase::startNewSession(bool createDefaults)
{
	// Clear collections
	collections_.clear();

	// Clear layout
	viewLayout_.clear();
	currentViewPane_ = NULL;

	// Create defaults if requested
	if (createDefaults)
	{
		// Set basic view layout
		currentViewPane_ = viewLayout_.setDefault();

		// Add an empty collection, and add it to the current view pane
		currentViewPane_->addCollectionTarget(addCollection());
		currentViewPane_->translateView(0.0, 0.0, -15.0);
	}

	// Set current project data
	setAsNotModified();
	inputFile_.clear();
}

// Add new collection
Collection* UChromaBase::addCollection(const char* name, int listIndex)
{
	// Add an empty collection
	currentCollection_ = (listIndex == -1 ? collections_.add() : collections_.addAt(listIndex));

	// Create EditState data
	if (UChromaBase::addEditState(currentCollection_->objectInfo(), EditState::CollectionAddQuantity))
	{
		UChromaBase::addEditStateData(true, "collection", currentCollection_);
		UChromaBase::addEditStateData(true, "locator", currentCollection_->locator());
		UChromaBase::addEditStateData(true, "position", collectionIndex(currentCollection_));
	}

	// Set the title
	if (name == NULL) currentCollection_->setName( uniqueCollectionName("Empty Collection") );
	else currentCollection_->setName(uniqueCollectionName(name));

	// Send a signal out
	UChromaSignal::send(UChromaSignal::CollectionCreatedSignal, currentCollection_);

	setAsModified();

	return currentCollection_;
}

// Add new collection to the current view pane
Collection* UChromaBase::addCollectionToCurrentViewPane(const char* name, int identifier)
{
	// Create the collection, calling our addCollection() function to do the work
	Collection* collection = addCollection(name);
	collection->setIdentifier(identifier);

	if (currentViewPane_) currentViewPane_->addCollectionTarget(collection);
	else Messenger::warn("Can't add new Collection to current view pane, since the current view pane is NULL.\n");

	return collection;
}

// Add new collection at the specified location
Collection* UChromaBase::addCollectionFromLocator(const char* locator, Collection::CollectionType type, int listIndex)
{
	// First, split up the supplied string by the delimiter '//'
	QStringList locatorParts = QString(locator).split("//");

	// Remove the last one (the target collection itself)
	CharString collectionName = qPrintable(locatorParts.last());
	locatorParts.removeLast();

	// If there are no parts left in the list, then it must have been a master collection
	if (locatorParts.count() == 0)
	{
		Collection* collection = addCollection(collectionName, listIndex);
		printf("Added collectionFromLocator '%s', its objectId is %i\n", qPrintable(collectionName), collection->objectId());
		return collection;
	}

	// Loop over main collections, passing parts list
	for (Collection* collection = collections_.first(); collection != NULL; collection = collection->next)
	{
		Collection* foundCollection = collection->locateCollection(locatorParts, 0);
		if (foundCollection)
		{
			Collection* collection = NULL;
			if (type == Collection::FitCollection) collection = foundCollection->addFit(collectionName, listIndex);
			else if (type == Collection::ExtractedCollection) collection = foundCollection->addSlice(collectionName, listIndex);
			else printf("Don't know how to add currentSlice from locator.\n");
			return collection;
		}
	}
	return NULL;
}

// Add new Collection from Collection block data
Collection* UChromaBase::addCollectionFromBlock(const char* blockData)
{
	// Create a local LineParser and set the source string
	LineParser parser;
	parser.openInputString(blockData);

	// Store current collection pointer
	Collection* collection = currentCollection_;

	// Parse the block
	bool success = parseInputBlocks(parser);

	// Set Collection to return
	collection = (success && (collection != currentCollection_) ? currentCollection_ : NULL);

	return collection;
}

// Remove existing collection
void UChromaBase::removeCollection(Collection* collection)
{
	// Check current Collection
	if (!Collection::objectValid(collection, "collection in UChromaBase::removeCollection()")) return;

	// Check parent in order to work out where to delete the collection from
	if (collection->parent() == NULL)
	{
		// Set new currentCollection_
		if (collection->next) currentCollection_ = collection->next;
		else currentCollection_ = collection->prev;

		// Send a signal out before we finalise deletion
		UChromaSignal::send(UChromaSignal::CollectionDeletedSignal, collection);

		// Create EditState data
		if (UChromaBase::addEditState(collection->objectInfo(), EditState::CollectionRemoveQuantity))
		{
			UChromaBase::addEditStateData(false, "collection", collection);
			UChromaBase::addEditStateData(false, "locator", collection->locator());
			UChromaBase::addEditStateData(false, "position", collectionIndex(collection));
		}

		// Remove master collection
		collections_.remove(collection);
	}
	else
	{
		// Need to check whether this is a Fit or an ExtractedData
		if (collection->type() == Collection::FitCollection)
		{
			// Set new currentCollection_
			if (collection->next) currentCollection_ = collection->next;
			else currentCollection_ = collection->prev;
			if (currentCollection_ == NULL) currentCollection_ = collection->parent();

			// Remove collection
			collection->parent()->removeFit(collection);
		}
		else
		{
			// Set new currentCollection_
			if (collection->next) currentCollection_ = collection->next;
			else currentCollection_ = collection->prev;
			if (currentCollection_ == NULL) currentCollection_ = collection->parent();

			// Remove collection
			collection->parent()->removeSlice(collection);
		}
	}

	setAsModified();
}

// Remove current collection
void UChromaBase::removeCurrentCollection()
{
	removeCollection(currentCollection_);
}

// Return number of collections
int UChromaBase::nCollections()
{
	return collections_.nItems();
}

// Move collection focus to next in list
void UChromaBase::focusNextCollection()
{
	currentCollection_ = currentCollection_->nextCollection(true);

	// Safety check...
	if (currentCollection_ == NULL) currentCollection_ = collections_.first();

	updateGUI();
}

// Move collection focus to previous in list
void UChromaBase::focusPreviousCollection()
{
	currentCollection_ = currentCollection_->previousCollection(false);

	// Safety check...
	if (currentCollection_ == NULL) currentCollection_ = collections_.last()->previousCollection(true);

	updateGUI();
}

// Set current Collection
void UChromaBase::setCurrentCollection(Collection* collection)
{
	if (Collection::objectValid(collection, "collection in UChromaBase::setCurrentCollection()")) currentCollection_ = collection;
}

// Return nth collection in list
Collection* UChromaBase::collection(int index)
{
	return collections_[index];
}

// Return index of specified collection
int UChromaBase::collectionIndex(Collection* collection)
{
	return collections_.indexOf(collection);
}

// Locate collection
Collection* UChromaBase::locateCollection(const char* locator)
{
	// First, split up the supplied string by the delimiter '//'
	QStringList locatorParts = QString(locator).split("//");

	// Loop over main collections, passing parts list
	for (Collection* collection = collections_.first(); collection != NULL; collection = collection->next)
	{
		Collection* foundCollection = collection->locateCollection(locatorParts, 0);
		if (foundCollection) return foundCollection;
	}
	return NULL;
}

// Return first collection in list
Collection* UChromaBase::collections()
{
	return collections_.first();
}

// Return currently-selected Collection
Collection* UChromaBase::currentCollection()
{
	return currentCollection_;
}

// Return whether specified collection is the current collection
bool UChromaBase::isCurrentCollection(Collection* collection)
{
	return (currentCollection_ == collection);
}

// Identify collection
Collection* UChromaBase::identifyCollection(int id)
{
	// Loop over main collections, passing parts list
	for (Collection* collection = collections_.first(); collection != NULL; collection = collection->next) if (collection->identifier() == id) return collection;

	return NULL;
}

// Set collection visibility
bool UChromaBase::setCollectionVisible(const char* name, bool visible)
{
	// Identify collection
	Collection* collection = locateCollection(name);

	if (collection)
	{
		collection->setVisible(visible);

		updateGUI();

		return true;
	}
	else Messenger::warn("Collection with name '%s' could not be found.\n", name);

	return false;
}

// Refresh referenced DataSets in all Collections
void UChromaBase::refreshReferencedDataSets()
{
	// Loop over main collections, passing parts list
	for (Collection* collection = collections_.first(); collection != NULL; collection = collection->next)
	{
		collection->refreshDataSets();
	}
}
