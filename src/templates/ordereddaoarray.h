/*
	*** Index-Ordered DynamicArrayObject Array Class
	*** src/templates/ordereddaoarray.h
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

#ifndef DISSOLVE_ORDEREDDAOARRAY_H
#define DISSOLVE_ORDEREDDAOARRAY_H

#include "templates/dynamicarrayobject.h"
#include "templates/orderedpointerarray.h"

// Forward Declarations
/* none */

// Ordered DynamicArrayObject Array
template <class T> class OrderedDAOArray : public OrderedPointerArray<T>
{
	public:
	// Constructor
	OrderedDAOArray<T>() : OrderedPointerArray<T>()
	{
	}
	// Destructor
	~OrderedDAOArray()
	{
		OrderedPointerArray<T>::clear();
	}
	// Copy Constructor
	OrderedDAOArray<T>(const OrderedDAOArray<T>& source) : OrderedPointerArray<T>(*this)
	{
	}


	/*
	 * Item Management
	 */
	protected:
	// Compare the two objects, returning if their 'indices' are equal
	inline bool itemIndicesEqual(DynamicArrayObject<T>* a, DynamicArrayObject<T>* b)
	{
		return (a->arrayIndex() == b->arrayIndex());
	}
	// Compare the two objects, returning if the 'index' of a is greater than that of b
	inline bool itemIndexGreaterThan(T* a, T* b)
	{
		return (a->arrayIndex() < b->arrayIndex());
	}
	// Compare the two objects, returning if the 'index' of a is less than that of b
	inline bool itemIndexLessThan(T* a, T* b)
	{
		return (a->arrayIndex() > b->arrayIndex());
	}
};

#endif
