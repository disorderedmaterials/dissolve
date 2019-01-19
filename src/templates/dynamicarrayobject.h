/*
	*** Dynamic Array Object
	*** src/templates/dynamicarrayobject.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_DYNAMICARRAYOBJECT_H
#define DISSOLVE_DYNAMICARRAYOBJECT_H

// Forward Declarations
template <class T> class DynamicArray;

// Dynamic Array Object Class
template <class T> class DynamicArrayObjectBase
{
	public:
	// Constructor
	DynamicArrayObjectBase<T>()
	{
		arrayIndex_ = -1;
	}

	protected:
	// Index of object within parent DynamicArray
	int arrayIndex_;

	public:
	// Return index of object within parent DynamicArray
	int arrayIndex() const
	{
		return arrayIndex_;
	}
};

// Dynamic Array Object Class
template <class T> class DynamicArrayObject : public DynamicArrayObjectBase<T>
{
	friend class DynamicArray<T>;

	protected:
	// Clear object, ready for re-use
	virtual void clear() = 0;
	// Return index of object within parent DynamicArray
	void setArrayIndex(int index)
	{
		DynamicArrayObjectBase<T>::arrayIndex_ = index;
	}
};

#endif
