/*
	*** Array Classes
	*** src/templates/array.h
	Copyright T. Youngs 2012-2016

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

#ifndef DUQ_ARRAY_H
#define DUQ_ARRAY_H

#define CHUNKSIZE 128

#include "base/messenger.h"
#include "templates/list.h"
#include "templates/vector3.h"

// Array
template <class A> class Array : public ListItem< Array<A> >
{
	public:
	// Constructor
	Array(int initialSize = 0) : ListItem< Array<A> >()
	{
		array_ = NULL;
		size_ = 0;
		nItems_ = 0;
		if (initialSize > 0) initialise(initialSize);
	}
	// Destructor
	~Array()
	{
		if (array_ != NULL) delete[] array_;
	}
	// Copy Constructor
	Array(const Array<A>& source)
	{
		array_ = NULL;
		size_ = 0;
		nItems_ = 0;
		initialise(source.size_);
		nItems_ = source.nItems_;
		for (int n=0; n<nItems_; ++n) array_[n] = source.array_[n];
	}
	// Assignment Operator
	void operator=(const Array<A>& source)
	{
		clear();
		resize(source.size_);
		nItems_ = source.nItems_;
		for (int n=0; n<nItems_; ++n) array_[n] = source.array_[n];
	}
	// Conversion operator (to standard array)
	operator A*()
	{
		return array_;
	}


	/*
	 * Array Data
	 */
	private:
	// Current size of Array
	int size_;
	// Array data
	A* array_;
	// Number of data actually in Array
	int nItems_;

	private:
	// Resize array 
	void resize(int newSize)
	{
		// Array large enough already?
		if ((newSize-size_) <= 0) return;

		// Copy old data to temporary array
		A* oldItems = NULL;
		if (nItems_ > 0)
		{
			oldItems = new A[nItems_];
			for (int n=0; n<nItems_; ++n) oldItems[n] = array_[n];
		}

		// Delete old, and create new array
		if (array_ != NULL) delete[] array_;
		size_ = newSize;
		array_ = new A[size_];

		// Copy old data into new array
		if (nItems_ > 0)
		{
			for (int n=0; n<nItems_; ++n) array_[n] = oldItems[n];
			delete[] oldItems;
		}
	}

	public:
	// Return number of items in array
	int nItems() const
	{
		return nItems_;
	}
	// Return current maximum size of array
	int size() const
	{
		return size_;
	}
	// Return data array
	A* array()
	{
		return array_;
	}
	// Forget data (set nItems to zero) leaving array intact
	void forgetData()
	{
		nItems_ = 0;
	}
	// Clear array
	void clear()
	{
		nItems_ = 0;
		if (array_ != NULL) delete[] array_;
		array_ = NULL;
		size_ = 0;
	}
	// Create empty array of specified size
	void initialise(int size, int value = 0)
	{
		// First, resize array...
		resize(size);
		
		// ...then set number of items to specified size...
		nItems_ = size;
		
		// ...and finally set all elements to specified value
		for (int n=0; n<nItems_; ++n) array_[n] = value;
	}


	/*
	 * Set/Get
	 */
	public:
	// Add new element to array
	void add(A data)
	{
		// Is current array large enough?
		if (nItems_ == size_) resize(size_+CHUNKSIZE);

		// Store new value
		array_[nItems_++] = data;
	}
	// Return nth item in array
	A& operator[](int n)
	{
#ifdef CHECKS
		if ((n < 0) || (n >= nItems_))
		{
			static A dummy;
			Messenger::print("OUT_OF_RANGE - Array index %i is out of range in Array::operator[] (nItems = %i).\n", n, nItems_);
			return dummy;
		}
#endif
		return array_[n];
	}
	// Return single value
	A value(int n) const
	{
#ifdef CHECKS
		if ((n < 0) || (n >= nItems_))
		{
			Messenger::print("OUT_OF_RANGE - Array index %i is out of range in Array::value() (nItems = %i).\n", n, nItems_);
			return A();
		}
#endif
		return array_[n];
	}
	// Operator= (set all)
	void operator=(const double value) { for (int n=0; n<nItems_; ++n) array_[n] = value; }
	void operator=(const int value) { for (int n=0; n<nItems_; ++n) array_[n] = value; }
	void operator=(const Vec3<double>& value) { for (int n=0; n<nItems_; ++n) array_[n] = value; }
	// Operator+= (add to all)
	void operator+=(const double value) { for (int n=0; n<nItems_; ++n) array_[n] += value; }
	void operator+=(const int value) { for (int n=0; n<nItems_; ++n) array_[n] += value; }
	void operator+=(const Vec3<double>& value) { for (int n=0; n<nItems_; ++n) array_[n] += value; }
	void operator+=(const Array<A> array) { for (int n=0; n<nItems_; ++n) array_[n] += array.value(n); }
	// Operator-= (subtract from all)
	void operator-=(const double value) { for (int n=0; n<nItems_; ++n) array_[n] -= value; }
	void operator-=(const int value) { for (int n=0; n<nItems_; ++n) array_[n] -= value; }
	void operator-=(const Vec3<double>& value) { for (int n=0; n<nItems_; ++n) array_[n] -= value; }
	// Operator*= (multiply all)
	void operator*=(const double value) { for (int n=0; n<nItems_; ++n) array_[n] *= value; }
	void operator*=(const int value) { for (int n=0; n<nItems_; ++n) array_[n] *= value; }
	void operator*=(const Vec3<double>& value) { for (int n=0; n<nItems_; ++n) array_[n] *= value; }
	// Operator/= (divide all)
	void operator/=(const double value) { for (int n=0; n<nItems_; ++n) array_[n] /= value; }
	void operator/=(const int value) { for (int n=0; n<nItems_; ++n) array_[n] /= value; }
	void operator/=(const Vec3<double>& value) { for (int n=0; n<nItems_; ++n) array_[n] /= value; }
	// Operator- (subtraction)
	Array<A> operator-(const double value) { Array<A> result = *this; result -= value; return result; }
	Array<A> operator-(const int value) { Array<A> result = *this; result -= value; return result; }
	Array<A> operator-(const Array<A> array) { Array<A> result(nItems_); for (int n=0; n<nItems_; ++n) result[n] = array_[n] - array.value(n); return result; }
	// Operator+ (addition)
	Array<A> operator+(const double value) { Array<A> result = *this; result += value; return result; }
	Array<A> operator+(const int value) { Array<A> result = *this; result += value; return result; }
	Array<A> operator+(const Array<A> array) { Array<A> result(nItems_); for (int n=0; n<nItems_; ++n) result[n] = array_[n] + array.value(n); return result; }
	// Operator* (multiplication)
	Array<A> operator*(const double value) { Array<A> result = *this; result *= value; return result; }
	Array<A> operator*(const int value) { Array<A> result = *this; result *= value; return result; }
	// Return first value in array
	A first()
	{
		if (nItems_ == 0)
		{
			Messenger::print("OUT_OF_RANGE - No first item to return in Array.\n");
			return A();
		}
		return array_[0];
	}
	// Return last value in array
	A last()
	{
		if (nItems_ == 0)
		{
			Messenger::print("OUT_OF_RANGE - No last item to return in Array.\n");
			return A();
		}
		return array_[nItems_-1];
	}


	/*
	 * Functions
	 */
	public:
	// Return sum of elements in array
	A sum()
	{
		A result = 0.0;
		for (int n=0; n<nItems_; ++n) result += array_[n];
		return result;
	}
	
};

// Array2D
template <class A> class Array2D
{
	public:
	// Constructor
	Array2D(int nrows = 0, int ncolumns = 0, bool half = false)
	{
		array_ = NULL;
		linearSize_ = 0;
		rowOffsets_ = NULL;
		nRows_ = 0;
		nColumns_ = 0;
		half_ = half;
		if ((nrows > 0) && (ncolumns > 0)) resize(nrows, ncolumns);
	}
	// Destructor
	~Array2D()
	{
		clear();
	}
	// Clear array data
	void clear()
	{
		if (array_ != NULL) delete[] array_;
		if (rowOffsets_ != NULL) delete[] rowOffsets_;
		rowOffsets_ = NULL;
		array_ = NULL;
		linearSize_ = 0;
		nRows_ = 0;
		nColumns_ = 0;
	}
	// Copy Constructor
	Array2D(const Array2D<A>& source)
	{
		(*this) = source;
	}
	// Assignment Operator
	void operator=(const A value)
	{
		// Copy source data elements
		for (int row=0; row<nRows_; ++row)
		{
			if (half_) for (int column=row; column<nColumns_; ++column) array_[rowOffsets_[row] + column - row] = value;
			else for (int column=0; column<nColumns_; ++column) array_[rowOffsets_[row] + column] = value;
		}
	}
	// Assignment Operator
	void operator=(const Array2D<A>& source)
	{
		// Clear any existing data and reinitialise the array
		clear();
		initialise(source.nRows_, source.nColumns_, source.half_);

		// Copy source data elements
		for (int row=0; row<nRows_; ++row)
		{
			if (half_) for (int column=row; column<nColumns_; ++column) array_[rowOffsets_[row] + column - row] = source.array_[rowOffsets_[row] + column - row];
			else for (int column=0; column<nColumns_; ++column) array_[rowOffsets_[row] + column] = source.array_[rowOffsets_[row] + column];
		}
	}


	/*
	 * Data
	 */
	private:
	// Linear array of objects
	A* array_;
	// Size of linear array
	int linearSize_;
	// Array dimensions
	int nRows_, nColumns_;
	// Half-matrix mode
	bool half_;
	// Row offsets
	int* rowOffsets_;

	private:
	// Resize array 
	void resize(int nrows, int ncolumns)
	{
		// Clear old data
		clear();

		// If we're only interested in half the matrix then it must be square
		if (half_ && (nrows != ncolumns))
		{
			Messenger::print("BAD_USAGE - Requested half-matrix mode on a non-square matrix in Array2D::resize().\n");
		}

		// Create new array
		nRows_ = nrows;
		nColumns_ = ncolumns;
		rowOffsets_ = new int[nRows_];
		if (half_)
		{
			// Half-array, with element (i,j) == (j,i)
			int n;
			linearSize_ = 0;
			for (n=nRows_; n>0; --n)
			{
				rowOffsets_[nRows_-n] = linearSize_;
				linearSize_ += n;
			}
			array_ = new A[linearSize_];
		}
		else
		{
			linearSize_ = nRows_*nColumns_;
			array_ = new A[linearSize_];
			for (int n=0; n<nRows_; ++n) rowOffsets_[n] = n*nColumns_;
		}
	}

	public:
	// Initialise array
	void initialise(int nrows, int ncolumns, bool half = false)
	{
		half_ = half;
		if ((nrows > 0) && (ncolumns > 0)) resize(nrows, ncolumns);
// 		else printf("BAD_USAGE - Zero or negative row/column size(s) given to Array2D::initialise() (r=%i, c=%i)\n", nrows, ncolumns);
	}
	// Return specified element as reference
	A& ref(int row, int column)
	{
#ifdef CHECKS
		static A dummy;
		if ((row < 0) || (row >= nRows_))
		{
			Messenger::print("OUT_OF_RANGE - Row number (%i) is out of range in Array2D::ref() (nRows = %i).\n", row, nRows_);
			return dummy;
		}
		if ((column < 0) || (column >= nColumns_))
		{
			Messenger::print("OUT_OF_RANGE - Column number (%i) is out of range in Array2D::ref() (nColumns = %i).\n", column, nColumns_);
			return dummy;
		}
#endif
		if (half_) 
		{
			if (row > column) return array_[rowOffsets_[column] + row - column];
			else return array_[rowOffsets_[row] + column - row];
		}
		else return array_[rowOffsets_[row] + column];
	}
	// Return specified element as value
	A value(int row, int column) const
	{
#ifdef CHECKS
		static A dummy;
		if ((row < 0) || (row >= nRows_))
		{
			Messenger::print("OUT_OF_RANGE - Row number (%i) is out of range in Array2D::value() (nRows = %i).\n", row, nRows_);
			return dummy;
		}
		if ((column < 0) || (column >= nColumns_))
		{
			Messenger::print("OUT_OF_RANGE - Column number (%i) is out of range in Array2D::value() (nColumns = %i).\n", column, nColumns_);
			return dummy;
		}
#endif
		if (half_) 
		{
			if (row > column) return array_[rowOffsets_[column] + row - column];
			else return array_[rowOffsets_[row] + column - row];
		}
		else return array_[rowOffsets_[row] + column];
	}
	// Return address of specified element
	A* ptr(int row, int column)
	{
#ifdef CHECKS
		static A dummy;
		if ((row < 0) || (row >= nRows_))
		{
			Messenger::print("OUT_OF_RANGE - Row number (%i) is out of range in Array2D::ptr() (nRows = %i).\n", row, nRows_);
			return &dummy;
		}
		if ((column < 0) || (column >= nColumns_))
		{
			Messenger::print("OUT_OF_RANGE - Column number (%i) is out of range in Array2D::ptr() (nColumns = %i).\n", column, nColumns_);
			return &dummy;
		}
#endif
		if (half_) 
		{
			if (row > column) return &array_[rowOffsets_[column] + row - column];
			else return &array_[rowOffsets_[row] + column - row];
		}
		else return &array_[rowOffsets_[row] + column];
	}
	// Returun whether the array is halved
	bool halved() const
	{
		return half_;
	}
	// Return number of rows
	int nRows() const
	{
		return nRows_;
	}
	// Return number of columns
	int nColumns() const
	{
		return nColumns_;
	}
	// Return linear array size
	int linearArraySize()
	{
		return linearSize_;
	}
	// Return linear array
	A* linearArray()
	{
		return array_;
	}
};
	
#endif
