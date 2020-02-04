/*
	*** Simple Pointer Array Class
	*** src/templates/pointerarray.h
	Copyright T. Youngs 2012-2020

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

#ifndef DISSOLVE_POINTERARRAY_H
#define DISSOLVE_POINTERARRAY_H

#include <stdlib.h>
#include <stdio.h>
#include "base/messenger.h"

// Forward Declarations
/* none */

/*
 * Simple Pointer Array Class
 * A contiguous pointer array class designed to be as lightweight as possible.
 * Order of items must not be important, as the arrangement of pointers is subject to change.
 */
template <class T> class PointerArray
{
	public:
	// Constructor
	PointerArray<T>()
	{
		nItems_ = 0;
		arraySize_ = 0;
		items_ = NULL;
	}
	// Destructor
	~PointerArray()
	{
		clear();
	}
	// Copy Constructor
	PointerArray<T>(const PointerArray<T>& source)
	{
		nItems_ = 0;
		arraySize_ = 0;
		items_ = NULL;
		(*this) = source;
	}
	// Assignment operator
	PointerArray<T>& operator=(const PointerArray<T>& source)
	{
		// Clear any current data in the list...
		clear();

		initialise(source.nItems());

		for (int n=0; n<source.nItems(); ++n) append(source.value(n));

		return *this;
	}
	// Element access operator
	T* operator[](int index)
	{
#ifdef CHECKS
		if ((index < 0) || (index >= nItems_))
		{
			Messenger::error("PointerArray<T>::operator[](%i) - Array index out of bounds (%i items in array).\n", index, nItems_);
			return NULL;
		}
#endif
		return items_[index];
	}
	// Const access operator
	T* at(int index) const
	{
#ifdef CHECKS
		if ((index < 0) || (index >= nItems_))
		{
			Messenger::error("PointerArray<T>::at(%i) - Array index out of bounds (%i items in array).\n", index, nItems_);
			return NULL;
		}
#endif
		return items_[index];
	}


	/*
	 * Basic Data
	 */
	protected:
	// Array of items
	T** items_;
	// Size of items array
	int arraySize_;
	// Number of items in array
	int nItems_;

	public:
	// Clear the list
	void clear()
	{
		if (items_) delete[] items_;
		items_ = NULL;
		nItems_ = 0;
		arraySize_ = 0;
	}
	// Create empty list of specified size
	void initialise(int size)
	{
		clear();

		arraySize_ = size;

		if (size == 0) items_ = NULL;
		else items_ = new T*[arraySize_];

		for (int n=0; n<arraySize_; ++n) items_[n] = NULL;
	}
	// Returns the number of items in the list
	int nItems() const
	{
		return nItems_;
	}
	// Const-value access
	T* value(int index) const
	{
#ifdef CHECKS
		if ((index < 0) || (index >= nItems_))
		{
			Messenger::error("PointerArray<T>::index(%i) - Array index out of bounds (%i items in array).\n", index, nItems_);
			return NULL;
		}
#endif

		return items_[index];
	}
	// Return pointer array
	T** items()
	{
		return items_;
	}


	/*
	 * Item Management
	 */
	public:
	// Append an item to the list
	void append(T* ptr)
	{
		// If there is no space for the new item we must resize the array
		if (nItems_ == arraySize_)
		{
			// Create a new array with some more item space
			// TODO This is memory efficient, but v slow for multiple additions.  Add chunkSize?
			T** newArray = new T*[arraySize_+2];

			// Copy the existing pointers over
			for (int n=0; n<arraySize_; ++n) newArray[n] = items_[n];

			// Delete the old array
			delete[] items_;

			// Set the new array
			items_ = newArray;
			arraySize_ += 2;
		}

		items_[nItems_] = ptr;
		++nItems_;
	}
	// Remove an item from the array, leaving the remaining items contiguous in memory
	void remove(T* ptr)
	{
		// Step through the items until we find the specified pointer
		for (int n=0; n<nItems_; ++n)
		{
			if (items_[n] == ptr)
			{
				// Found it. If it is at the end of the list, just decrease nItems_.
				// If not, switch the current last item into this slot, and then decrease nItems_.
				if (n < (nItems_-1))
				{
					items_[n] = items_[nItems_-1];
					items_[nItems_-1] = NULL;
				}
				--nItems_;
				return;
			}
		}
		Messenger::print("PointerArray<T>::remove(%p) - Couldn't find pointer in array.\n", ptr);
	}
	// Remove indexed item from the array, leaving the remaining items contiguous in memory
	void remove(int index)
	{
#ifdef CHECKS
		if ((index < 0) || (index >= nItems_))
		{
			Messenger::error("Can't remove index '%i' from PointerArray<T>, since it is out of range (nItems = %i).\n", index, nItems_);
			return;
		}
#endif
		// If it is at the end of the list, just decrease nItems_.
		// If not, switch the current last item into this slot, and then decrease nItems_.
		if (index < (nItems_-1))
		{
			items_[index] = items_[nItems_-1];
			items_[nItems_-1] = NULL;
		}
		--nItems_;
	}
	// Remove last item from the array
	void removeLast()
	{
		if (nItems_ == 0) return;

		--nItems_;

		items_[nItems_] = NULL;
	}
	// Remove item from array and return it
	T* take(int index)
	{
#ifdef CHECKS
		if ((index < 0) || (index >= nItems_))
		{
			Messenger::error("PointerArray<T>::at(%i) - Array index out of bounds (%i items in array).\n", index, nItems_);
			return NULL;
		}
#endif
		T* item = items_[index];
		remove(index);

		return item;
	}
	// Return array index of specified pointer
	int indexOf(const T* ptr) const
	{
		for (int n=0; n<nItems_; ++n) if (items_[n] == ptr) return n;

		return -1;
	}
	// Return whether the array contains the specified pointer
	bool contains(const T* ptr) const
	{
		for (int n=0; n<nItems_; ++n) if (items_[n] == ptr) return true;

		return false;
	}
	// Return whether the array contains the specified pointer, searching from the last item backwards
	bool sniatnoc(const T* ptr) const
	{
		for (int n=nItems_-1; n>=0; --n) if (items_[n] == ptr) return true;

		return false;
	}
};

#endif
