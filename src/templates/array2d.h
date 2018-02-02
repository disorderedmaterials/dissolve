/*
	*** Two-Dimensional Array
	*** src/templates/array2d.h
	Copyright T. Youngs 2012-2018

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

#ifndef DUQ_ARRAY2D_H
#define DUQ_ARRAY2D_H

#include "base/messenger.h"
#include "templates/list.h"
#include "templates/vector3.h"

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
		array_ = NULL;
		linearSize_ = 0;
		rowOffsets_ = NULL;
		nRows_ = 0;
		nColumns_ = 0;
		half_ = false;
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
		clear();

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


	/*
	 * Functions
	 */
	public:
	// Invert matrix
	bool invert()
	{
		// Perform Gauss-Jordan inversion of the matrix.
		// Matrix must be square, and not stored as a half-matrix
		if (nRows_ != nColumns_)
		{
			Messenger::error("Can't invert this matrix since it is not square.\n");
			return false;
		}
		if (half_)
		{
			Messenger::error("Can't invert this matrix since it is stored as a half-matrix.\n");
			return false;
		}

		const int rank = nRows_;

		int pivotrows[rank], pivotcols[rank], pivotrow = 0, pivotcol = 0;
		bool pivoted[rank];
		int row, col, n, m;
		double large, element;
		for (n=0; n<rank; ++n)
		{
			pivotrows[n] = 0;
			pivotcols[n] = 0;
			pivoted[n] = false;
		}
		// Loop over columns to be reduced
		for (n=0; n<rank; ++n)
		{
			// Locate suitable pivot element - find largest value in the matrix A
			large = 0.0;
			for (row=0; row<rank; ++row)
			{
				// Only search this row if it has not previously contained a pivot element
				if (pivoted[row]) continue;
				for (col=0; col<rank; ++col)
				{
					// Similarly, only look at the column element if the column hasn't been pivoted yet.
					if (pivoted[col]) continue;
					// Check the size of the element...
					element = fabs(array_[row*rank+col]);
					if (element > large)
					{
						large = element;
						pivotrow = row;
						pivotcol = col;
					}
				}
			}
			
			// Mark the pivot row/column as changed
			pivoted[pivotcol] = true;
			pivotrows[n] = pivotrow;
			pivotcols[n] = pivotcol;
			
			// Exchange rows to put pivot element on the diagonal
			if (pivotrow != pivotcol)
			{
				for (m=0; m<rank; ++m)
				{
					element = array_[pivotrow*rank+m];
					array_[pivotrow*rank+m] = array_[pivotcol*rank+m];
					array_[pivotcol*rank+m] = element;
				}
			}
			
			// Now ready to divide through row elements.
			element = 1.0 / array_[pivotcol*rank+pivotcol];
			array_[pivotcol*rank+pivotcol] = 1.0;
			for (m=0; m<rank; ++m) array_[pivotcol*rank+m] *= element;
			
			// Divide through other rows by the relevant multiple of the pivot row
			for (row=0; row<rank; ++row)
			{
				if (row == pivotcol) continue;
				element = array_[row*rank + pivotcol];
				array_[row*rank + pivotcol] = 0.0;
				for (m=0; m<rank; ++m) array_[row*rank+m] = array_[row*rank+m] - array_[pivotcol*rank+m] * element;
			}
		}

		// Rearrange columns to undo row exchanges performed earlier
		for (n=rank-1; n>=0; --n)
		{
			if (pivotrows[n] != pivotcols[n]) for (m=0; m<rank; ++m)
			{
				element = array_[m*rank+pivotrows[n]];
				array_[m*rank+pivotrows[n]] = array_[m*rank+pivotcols[n]];
				array_[m*rank+pivotcols[n]] = element;
			}
		}
	
		return true;
	}
};

#endif
