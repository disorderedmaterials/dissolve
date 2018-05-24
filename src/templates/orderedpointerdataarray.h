/*
	*** Simple Ordered Pointer Array Class
	*** src/templates/orderedpointerarray.h
	Copyright T. Youngs 2012-2018

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

#ifndef DISSOLVE_ORDEREDPOINTERDATAARRAY_H
#define DISSOLVE_ORDEREDPOINTERDATAARRAY_H

#include <stdlib.h>
#include <stdio.h>
#include "base/messenger.h"

// Forward Declarations
/* none */

/*
 * Simple Ordered Pointer Array Class
 * A contiguous pointer array class designed to be as lightweight as possible.
 * Each pointer may exist in the list only once.
 * Each pointer has associated with it a single data item.
 * Order of pointers, based on their increasing value, is maintained.
 */
template <class T, class D> class OrderedPointerDataArray
{
	public:
	// Constructor
	OrderedPointerDataArray<T,D>()
	{
		nItems_ = 0;
		arraySize_ = 0;
		items_ = NULL;
		data_ = NULL;
	}
	// Destructor
	~OrderedPointerDataArray()
	{
		clear();
	}
	// Copy Constructor
	OrderedPointerDataArray<T,D>(const OrderedPointerDataArray<T,D>& source)
	{
		nItems_ = 0;
		arraySize_ = 0;
		items_ = NULL;
		data_ = NULL;
		(*this) = source;
	}
	// Assignment operator
	OrderedPointerDataArray<T,D>& operator=(const OrderedPointerDataArray<T,D>& source)
	{
		// Clear any current data in the list...
		clear();

		initialise(source.nItems());

		for (int n=0; n<source.nItems(); ++n) add(source.value(n), source.data(n));

		return *this;
	}
	// Element access operator
	T* operator[](int index)
	{
#ifdef CHECKS
		if ((index < 0) || (index >= nItems_))
		{
			Messenger::error("OrderedPointerDataArray<T,D>::operator[](%i) - Array index out of bounds (%i items in array).\n", index, nItems_);
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
	// Array of data
	D* data_;
	// Size of items array
	int arraySize_;
	// Number of items in array
	int nItems_;

	public:
	// Clear the list
	void clear()
	{
		if (items_) delete[] items_;
		if (data_) delete[] data_;
		items_ = NULL;
		data_ = NULL;
		nItems_ = 0;
		arraySize_ = 0;
	}
	// Create empty list of specified size
	void initialise(int size)
	{
		clear();

		arraySize_ = size;

		if (size == 0)
		{
			items_ = NULL;
			data_ = NULL;
		}
		else
		{
			items_ = new T*[arraySize_];
			data_ = new D[arraySize_];
		}

		for (int n=0; n<arraySize_; ++n)
		{
			items_[n] = NULL;
			data_[n] = D();
		}
	}
	// Returns the number of items in the list
	int nItems() const
	{
		return nItems_;
	}
	// Const pointer access
	T* pointer(int index) const
	{
#ifdef CHECKS
		if ((index < 0) || (index >= nItems_))
		{
			Messenger::error("OrderedPointerDataArray<T,D>::value(%i) - Array index out of bounds (%i items in array).\n", index, nItems_);
			return NULL;
		}
#endif

		return items_[index];
	}
	// Const-data access
	D data(int index) const
	{
#ifdef CHECKS
		if ((index < 0) || (index >= nItems_))
		{
			Messenger::error("OrderedPointerDataArray<T,D>::data(%i) - Array index out of bounds (%i items in array).\n", index, nItems_);
			return D();
		}
#endif

		return data_[index];
	}
	// Return pointer array
	T** items()
	{
		return items_;
	}
	// Return data array
	D* data()
	{
		return data_;
	}

	/*
	 * Item Management
	 */
	public:
	// Add an item to the list
	void add(T* ptr, D ptrData = D())
	{
		// Loop over pointers, searching for the first existing item that has a higher pointer address.
		// If we find the same pointer address in the list, return immediately.
		int insertAt, n;
		for (insertAt = 0; insertAt<nItems_; ++insertAt)
		{
			if (items_[insertAt] == ptr) return;
			if (items_[insertAt] > ptr) break;
		}

		// The pointer is not currently in the list, and we have the position at which it should be inserted in 'insertAt'.
		// If there is no more space, resize the array and copy the old items, inserting our new pointer at the correct position.
		// If there is no space for the new item we must resize the array
		if (nItems_ == arraySize_)
		{
			// Create a new array with some more item space
			T** newItems = new T*[arraySize_+2];
			D* newData = new D[arraySize_+2];

			// Copy existing pointers/data up to 'insertAt'
			for (n=0; n<insertAt; ++n)
			{
				newItems[n] = items_[n];
				newData[n] = data_[n];
			}

			// Place our new pointer/data here
			newItems[insertAt] = ptr;
			newData[insertAt] = ptrData;

			// Copy the remaining pointers/data, shifting them one place up in the new list
			for (n=insertAt; n<nItems_; ++n)
			{
				newItems[n+1] = items_[n];
				newData[n+1] = data_[n];
			}

			// Delete the old arrays if they exist
			if (items_) delete[] items_;
			if (data_) delete[] data_;

			// Set the new arrays and array size
			items_ = newItems;
			data_ = newData;
			arraySize_ += 2;
		}
		else
		{
			// Shuffle items from the insertion point up one place in the array (to higher indices), working backwards to avoid overwriting data
			for (n=nItems_-1; n>=insertAt; --n)
			{
				items_[n+1] = items_[n];
				data_[n+1] = data_[n];
			}

			// Now put the new item into the 'insertAt' position
			items_[insertAt] = ptr;
			data_[insertAt] = ptrData;
		}

		++nItems_;
	}
	// Remove an item from the array, leaving the remaining items contiguous in memory
	bool remove(T* ptr)
	{
		// Step through the items until we find the specified pointer
		for (int n=0; n<nItems_; ++n)
		{
			if (items_[n] > ptr) return false;
			if (items_[n] == ptr)
			{
				// Found it. Move all items from this point forward back one place
				for (int m=n+1; m<nItems_; ++m)
				{
					items_[m-1] = items_[m];
					data_[m-1] = data_[m];
				}
				items_[nItems_-1] = NULL;
				data_[nItems_-1] = D();
				--nItems_;

				return true;
			}
		}
		Messenger::print("OrderedPointerDataArray<T,D>::remove(%p) - Couldn't find pointer in array.\n", ptr);
		return false;
	}
	// Return array index of pointer within the list
	int indexOf(T* ptr) const
	{
		for (int n=0; n<nItems_; ++n) if (items_[n] == ptr) return n;

		return -1;
	}
};

#endif
