/*
	*** Object Store
	*** src/templates/objectstore.h
	Copyright T. Youngs 2013-2017

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

#ifndef DUQ_OBJECTSTORE_H
#define DUQ_OBJECTSTORE_H

#include "templates/reflist.h"
#include "base/sysfunc.h"
#include <stdio.h>

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
		XYDataObject,
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
	// Target object name
	CharString name_;

	public:
	// Set object target type and id
	void set(int type, int id)
	{
		type_ = type;
		id_ = id;
	}
	// Return object target type
	int type()
	{
		return type_;
	}
	// Return target object id
	int id()
	{
		return id_;
	}
	// Set object name
	void setName(const char* name)
	{
		name_ = name;
	}
	// Return object name
	const char* name()
	{
		return name_.get();
	}
};

// Object Store
template <class T> class ObjectStore
{
	public:
	// Constructor
	ObjectStore<T>(T* object = NULL)
	{
		// If the passed pointer is NULL, do not add anything to the list (we were probably called from a copy constructor)
		if (object != NULL)
		{
			// Store the parent object pointer, and add it to the master list
			object_ = object;
			objectInfo_.set(objectType_, objectCount_++);
			setObjectName("");
			objects_.add(object_, objectInfo_.id());
		}
	}
	// Destructor
	~ObjectStore<T>()
	{
		// Remove our pointer from the master list
		objects_.remove(object_);
	}
	// Object type identifier
	static int objectType_;
	// Object type name
	static const char* objectTypeName_;


	/*
	 * Object Pointer
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
	// Set name for this object, appending type name prefix
	void setObjectName(const char* name)
	{
		// TODO Check for duplicates here?
		objectInfo_.setName(CharString("%s@%s", objectTypeName_, name));
	}
	// Return object name
	const char* objectName()
	{
		return objectInfo_.name();
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
			printf("Invalid Object: Specified %s is NULL.\n", objectDescription);
			return false;
		}
		else if (!objects_.contains(object))
		{
			printf("Invalid Object: Specified %s no longer exists (original pointer was %p).\n", objectDescription, object);
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
	// Find specified resource
	static T* findResource(const char* resourceName)
	{
		for (RefListItem<T,int>* ri = objects_.first(); ri != NULL; ri = ri->next)
		{
			T* item = ri->item;
			if (DUQSys::sameString(item->objectInfo()->resourceName(), resourceName, true)) return item;
		}
		return NULL;
	}
};

#endif
