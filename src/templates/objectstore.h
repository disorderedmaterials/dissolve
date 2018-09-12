/*
	*** Object Store
	*** src/templates/objectstore.h
	Copyright T. Youngs 2013-2018

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

#ifndef DISSOLVE_OBJECTSTORE_H
#define DISSOLVE_OBJECTSTORE_H

#include "base/charstring.h"
#include "base/sysfunc.h"
#include "templates/reflist.h"
#include <stdio.h>
#include <string.h>

// Object Info
// -- Object type_ should be set from a local enum, for instance, containing all object types relevant in the use case.
class ObjectInfo
{
	public:
	// Constructor
	ObjectInfo()
	{
		type_ = 0;
		id_ = -1;
	}
	// Object types
	enum ObjectType
	{
		NoObject = 0,
		Data1DObject,
		Data2DObject,
		Data3DObject,
		Histogram1DObject,
		Histogram2DObject,
		Histogram3DObject,
		UChromaAxesObject,
		UChromaCollectionObject,
		UChromaViewPaneObject,
		nObjectTypes
	};

	private:
	// Object target type
	int type_;
	// Target object id
	int id_;
	// Target object tag
	CharString tag_;

	public:
	// Set object target type and id
	void set(int type, int id)
	{
		type_ = type;
		id_ = id;
	}
	// Return object target type
	int type() const
	{
		return type_;
	}
	// Return target object id
	int id() const
	{
		return id_;
	}
	// Set object tag
	void setTag(const char* tag)
	{
		tag_ = tag;
	}
	// Return object tag
	const char* tag() const
	{
		return tag_.get();
	}
};

// Object Store
template <class T> class ObjectStore
{
	public:
	// Constructor
	ObjectStore<T>(T* object)
	{
		// If the passed pointer is NULL, do not add anything to the list (we were probably called from a copy constructor)
		if (object == NULL)
		{
			Messenger::error("ObjectStore was passed a NULL pointer...\n");
			object_ = NULL;
			return;
		}

		// Store the parent object pointer, and add it to the master list
		object_ = object;
		objectInfo_.set(objectType_, objectCount_++);
		setObjectTag("");
		objects_.add(object_, objectInfo_.id());
	}
	// Destructor
	~ObjectStore<T>()
	{
		// Remove our pointer from the master list
		if (object_) objects_.remove(object_);
	}
	// Copy Constructor
	ObjectStore<T>(const ObjectStore<T>& source)
	{
		// ObjectStore<T> members are *not* copied, since a named object should be unique.
	}
	// Assignment Operator
	void operator=(const ObjectStore<T>& source)
	{
		// ObjectStore<T> members are *not* copied, since a named object should be unique.
	}
	// Object type identifier
	static int objectType_;
	// Object type name
	static const char* objectTypeName_;


	/*
	 * Object Data
	 */
	private:
	// Pointer to object that this ObjectStore was created with
	T* object_;
	// Object info
	ObjectInfo objectInfo_;

	public:
	// Return object type
	int objectType()
	{
		return objectInfo_.type();
	}
	// Return object ID
	int objectId()
	{
		return objectInfo_.id();
	}
	// Return object information
	ObjectInfo objectInfo()
	{
		return objectInfo_;
	}
	// Set tag for this object, appending type name prefix
	void setObjectTag(const char* tag)
	{
		// Get actual tag of object - does the supplied tag already contain a type prefix?
		CharString objectTag;
		if (strchr(tag, '@'))
		{
			CharString typePrefix = DissolveSys::beforeChar(tag, '@');
			objectTag = DissolveSys::afterChar(tag, '@');

			// Do a sanity check on the type prefix...
			if (!DissolveSys::sameString(typePrefix, objectTypeName_)) Messenger::error("Setting an object tag (%s) with a string that contains the wrong type prefix ('%s', while this class is '%s').\n", tag, typePrefix.get(), objectTypeName_);
		}
		else objectTag = tag;
#ifdef CHECKS
		// Check for duplicate value already in list
		if (!objectTag.isEmpty())
		{
			T* object = findObject(CharString("%s@%s", objectTypeName_, objectTag.get()));
			if (object && (object != this))
			{
				Messenger::warn("ObjectStore<%s>::setObjectTag() - The object '%s@%s' already exists in the ObjectStore. Behaviour will be undefined...\n", objectTypeName_, objectTypeName_, objectTag.get());
			}
		}
#endif
		objectInfo_.setTag(CharString("%s@%s", objectTypeName_, objectTag.get()));
	}
	// Return object tag
	const char* objectTag() const
	{
		return objectInfo_.tag();
	}
	// Return whether object tag matches that supplied
	bool objectTagIs(const char* tag) const
	{
		return (DissolveSys::sameString(objectInfo_.tag(), tag, true));
	}


	/*
	 * Object List
	 */
	private:
	// Master list of available objects
	static RefList<T,int> objects_;
	// Integer count for object IDs
	static int objectCount_;

	public:
	// Return whether specified object still exists
	static bool objectValid(T* object)
	{
		if ((object == NULL) || (!objects_.contains(object))) return false;
		return true;
	}
	// Return whether specified object still exists, reporting errors if it does not
	static bool objectValid(T* object, const char* objectDescription)
	{
		if (object == NULL)
		{
			Messenger::printVerbose("Invalid Object: Specified %s is NULL.\n", objectDescription);
			return false;
		}
		else if (!objects_.contains(object))
		{
			Messenger::printVerbose("Invalid Object: Specified %s no longer exists (original pointer was %p).\n", objectDescription, object);
			return false;
		}
		return true;
	}
	// Return number of available objects
	static int nObjects()
	{
		return objects_.nItems();
	}
	// Return object with specified ID
	static T* object(int id)
	{
		for (RefListItem<T,int>* ri = objects_.first(); ri != NULL; ri = ri->next) if (ri->data == id) return ri->item;
		return NULL;
	}
	// Set id of specified object, returning if we were successful
	static bool setObjectId(T* target, int id)
	{
		// Find the RefItem object in the list
		RefListItem<T,int>* targetRefItem = objects_.contains(target);
		if (targetRefItem == NULL)
		{
			printf("Internal Error: Couldn't find specified object %p in object list.\n", target);
			return false;
		}

		// Can we find an object with the same id?
		RefListItem<T,int>* rj = objects_.containsData(id);
		if ((rj != NULL) && (rj != targetRefItem))
		{
			printf("Internal Error: Another object with id %i already exists in the ObjectStore, so refusing to duplicate it.\n", id);
			return false;
		}

		if (rj == targetRefItem)
		{
			printf("ObjectStore::setObjectId() - Target object already has id specified (%i).\n", id);
			return true;
		}

		target->objectInfo_.set(target->objectInfo_.type(), id);
		targetRefItem->data = id;

		return true;
	}
	// Return whether the specified object tag is an object of this type
	static bool isObject(const char* objectTag)
	{
		// Get part before '@', which denotes the type
		const char* prefix = DissolveSys::beforeChar(objectTag, '@');
		return DissolveSys::sameString(prefix, objectTypeName_);
	}
	// Find specified object by its tag
	static T* findObject(const char* objectTag)
	{
		// Does the supplied tag contain a type prefix? If so, check it and then strip it
		CharString typePrefix = DissolveSys::beforeChar(objectTag, '@');
		if (typePrefix.isEmpty())
		{
			// No type prefix, so add ours and do the search
			CharString tag("%s@%s", objectTypeName_, objectTag);
			for (RefListItem<T,int>* ri = objects_.first(); ri != NULL; ri = ri->next)
			{
				T* item = ri->item;
				if (item->objectTagIs(tag)) return item;
			}
		}
		else
		{
			// Check the type prefix
			if (!DissolveSys::sameString(typePrefix, objectTypeName_))
			{
				Messenger::error("Searched for object '%s' in a store containing objects of type '%s'.\n", typePrefix.get(), objectTypeName_);
				return false;
			}
			for (RefListItem<T,int>* ri = objects_.first(); ri != NULL; ri = ri->next)
			{
				T* item = ri->item;
				if (item->objectTagIs(objectTag)) return item;
			}
		}
		return NULL;
	}
};

#endif
