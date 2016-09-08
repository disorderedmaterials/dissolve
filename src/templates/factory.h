/*
	*** Object Factory
	*** src/templates/factory.h
	Copyright T. Youngs 2012-2014

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

#ifndef DUQ_FACTORY_H
#define DUQ_FACTORY_H

#include "templates/list.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define FACTORYCHUNKSIZE 512

/*!
 * \brief Object Chunk
 * \details Chunk of objects, maintained by an ObjectFactory
 */
template <class T> class ObjectChunk
{
	public:
	// Constructor
	ObjectChunk<T>();
	// Destructor
	~ObjectChunk();
	// List pointers
	ObjectChunk<T>* prev, *next;


	/*!
	 * \name Chunk Data
	 */
	private:
	// Size of individual object
	int objectSize_;
	// Object array
	T* objectArray_;
	// Object usage flags
	bool* objectUsed_;
	// Number of unused objects in chunk
	int nUnusedObjects_;
	// Index of next available object
	int nextAvailableObject_;

	private:
	// Determine array offset of object
	int objectOffset(T* object);

	public:
	// Return next available object
	T* nextAvailable();
	// Return specified object to pool
	bool returnObject(T* object);
	// Return whether object is part of this chunk
	bool ownsObject(T* object);
	// Return whether there are unused objects in the chunk
	bool hasUnusedObjects();
};

/*!
 * \brief Constructor
 * \details Constructor for ObjectChunk
 */
template <class T> ObjectChunk<T>::ObjectChunk()
{
	objectArray_ = new T[FACTORYCHUNKSIZE];
	objectUsed_ = new bool[FACTORYCHUNKSIZE];
	objectSize_ = sizeof(T);
	for (int n=0; n<FACTORYCHUNKSIZE; ++n) objectUsed_[n] = false;
	nextAvailableObject_ = 0;
	nUnusedObjects_ = FACTORYCHUNKSIZE;
	
	prev = NULL;
	next = NULL;
}

/*!
 * \brief Destructor
 * \details Destructor for ObjectChunk. 
 */
template <class T> ObjectChunk<T>::~ObjectChunk()
{
	delete[] objectArray_;
}

/*!
 * \brief Determine array offset of object
 */
template <class T> int ObjectChunk<T>::objectOffset(T* object)
{
// 	printf("in objectoffset: %li %li\n", intptr_t(object), intptr_t(&objectArray_[0]));
	intptr_t offset = intptr_t(object) - intptr_t(&objectArray_[0]);
// 	printf("Offset = %li\n", offset);
	if (offset < 0) return -1;
	int index = offset / objectSize_;
	return (index < FACTORYCHUNKSIZE ? index : -1);
}

/*!
 * \brief Return next available object
 */
template <class T> T* ObjectChunk<T>::nextAvailable()
{
	if (nextAvailableObject_ == -1) return NULL;
	T* object = &objectArray_[nextAvailableObject_];
	objectUsed_[nextAvailableObject_] = true;
	--nUnusedObjects_;

	// If there are no more unused objects, then we are done
	if (nUnusedObjects_ == 0)
	{
		nextAvailableObject_ = -1;
		return object;
	}

	// Search for next available object before we return the object
	int nextFree = nextAvailableObject_ + 1;
	// -- First part - search to end of current array
	while (nextFree < FACTORYCHUNKSIZE)
	{
		if (!objectUsed_[nextFree])
		{
			nextAvailableObject_ = nextFree;
			return object;
		}
		++nextFree;
	}
	// -- Second part - search beginning of array up to current position
	nextFree = 0;
	while (nextFree < nextAvailableObject_)
	{
		if (!objectUsed_[nextFree])
		{
			nextAvailableObject_ = nextFree;
			return object;
		}
		++nextFree;
	}

	// Shouldn't get here!
	printf("Internal Error - ObjectChunk.\n");
	return object;
}

/*!
 * \brief Return specified object to pool
 */
template <class T> bool ObjectChunk<T>::returnObject(T* object)
{
	// Get the item offset of the object
	int offset = objectOffset(object);
	if (offset == -1) return false;
	
	// Mark the object as unused, and increase the unused counter
	objectUsed_[offset] = false;

	++nUnusedObjects_;
	if (nextAvailableObject_ == -1) nextAvailableObject_ = offset;
	return true;
}

/*!
 * \brief Return whether object is part of this chunk
 */
template <class T> bool ObjectChunk<T>::ownsObject(T* object)
{
	// Calculate array offset of this object
	return (objectOffset(object) != -1);
}

/*!
 * \brief Return whether there are unused objects in the chunk
 */
template <class T> bool ObjectChunk<T>::hasUnusedObjects()
{
	return (nUnusedObjects_ != 0);
}

/*!
 * \brief Object Factory Class
 * \details 
 */
template <class T> class ObjectFactory
{
	public:
	// Constructor
	ObjectFactory<T>();
	// Destructor
	~ObjectFactory();


	/*!
	 * \name Object Store
	 */
	///@{
	private:
	// List of object chunks maintained by this factory
	List< ObjectChunk<T> > objectChunks_;
	// Current chunk from which objects are being taken
	ObjectChunk<T>* currentChunk_;
	///@}


	/*!
	 * \name Object Access
	 */
	///@{
	public:
	// Produce a new object
	T* produce();
	// Return specified object to factory
	void returnObject(T* object);
	///@}
};

/*!
 * \brief Constructor
 * \details Constructor for ObjectFactory
 */
template <class T> ObjectFactory<T>::ObjectFactory()
{
	currentChunk_ = NULL;
}

/*!
 * \brief Destructor
 * \details Destructor for ObjectFactory. 
 */
template <class T> ObjectFactory<T>::~ObjectFactory()
{
}

/*
 * Object Access
 */

/*!
 * \brief Produce a new object
 */
template <class T> T* ObjectFactory<T>::produce()
{
	if (currentChunk_ == NULL)
	{
		currentChunk_ = objectChunks_.add();
		return currentChunk_->nextAvailable();
	}
	else if (currentChunk_->hasUnusedObjects()) return currentChunk_->nextAvailable();
	else
	{
		// Must search current chunk list to see if any current chunks have available space. If not, we will create a new one
		for (ObjectChunk<T>* chunk = objectChunks_.first(); chunk != NULL; chunk = chunk->next)
		{
			if (chunk == currentChunk_) continue;
			if (chunk->hasUnusedObjects())
			{
				currentChunk_ = chunk;
				return currentChunk_->nextAvailable();
			}
		}

		// No dice - make a new chunk
		currentChunk_ = objectChunks_.add();
		return currentChunk_->nextAvailable();
	}

	// If we get here, then something has gone horribly wrong...
	printf("Internal Error - Couldn't find an empty chunk to return an object from.\n");
	return NULL;
}

/*!
 * \brief Return specified object to factory
 */
template <class T> void ObjectFactory<T>::returnObject(T* object)
{
	// Must find chunk which owns this object
	for (ObjectChunk<T>* chunk = objectChunks_.first(); chunk != NULL; chunk = chunk->next) if (chunk->returnObject(object)) return;

	// Couldn't find it!
	printf("Internal Error - Tried to return an object to an ObjectFactory which didn't produce it.\n");
}

#endif