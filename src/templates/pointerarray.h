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
#include <algorithm>
#include <vector>
#include "base/messenger.h"

// Forward Declarations
/* none */

/*
 * Simple Pointer Array Class
 * A contiguous pointer array class designed to be as lightweight as possible.
 * Order of items must not be important, as the arrangement of pointers is subject to change.
 */
template <class T> class PointerArray : std::vector<T*>
{
	public:
	using std::vector<T*>::begin;
	using std::vector<T*>::clear;
	using std::vector<T*>::data;
	using std::vector<T*>::end;
	using std::vector<T*>::erase;
	using std::vector<T*>::operator[];
	using std::vector<T*>::push_back;
	using std::vector<T*>::rbegin;
	using std::vector<T*>::rend;
	using std::vector<T*>::reserve;
	using std::vector<T*>::size;
	// Const access operator
	T* at(int index) const
	{
		return operator[](index);
	}


	public:
	// Create empty list of specified size
	void initialise(int size)
	{
		clear();
		reserve(size);
	}
	// Returns the number of items in the list
	int nItems() const
	{
		return size();
	}
	// Const-value access
	T* value(int index) const
	{
		return operator[](index);
	}
	T** items()
	{
		return data();
	}


	/*
	 * Item Management
	 */
	public:
	// Append an item to the list
	void append(T* ptr)
	{
		push_back(ptr);
	}
	// Remove an item from the array, leaving the remaining items contiguous in memory
	void remove(T* ptr)
	{
		erase(find(begin(), end(), ptr));
	}
	// Remove indexed item from the array, leaving the remaining items contiguous in memory
	void remove(int index)
	{
		erase(begin()+index);
	}
	// Remove last item from the array
	void removeLast()
	{
		remove(size()-1);
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
		T* item = at(index);
		remove(index);

		return item;
	}
	// Return array index of specified pointer
	int indexOf(const T* ptr) const
	{
		auto it = find(begin(), end(), ptr);
		if (it == end()) return -1;
		return it - begin();
	}
	// Return whether the array contains the specified pointer
	bool contains(const T* ptr) const
	{
		return find(begin(), end(), ptr) != end();
	}
	// Return whether the array contains the specified pointer, searching from the last item backwards
	bool sniatnoc(const T* ptr) const
	{
		return find(rbegin(), rend(), ptr) != rend();
	}
};

#endif
