/*
	*** Three-Dimensional Array
	*** src/templates/array3d.h
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

#ifndef DISSOLVE_ARRAY3D_H
#define DISSOLVE_ARRAY3D_H

#include "base/messenger.h"
#include "templates/list.h"
#include "templates/vector3.h"

// Array3D
template <class A> class Array3D
{
	public:
	// Constructor
	Array3D(int nX = 0, int nY = 0, int nZ = 0)
	{
		array_ = NULL;
		linearSize_ = 0;
		sliceOffsets_ = NULL;
		nX_ = 0;
		nY_ = 0;
		nZ_ = 0;
		if ((nX > 0) && (nY > 0) && (nZ > 0)) resize(nX, nY, nZ);
	}
	// Destructor
	~Array3D()
	{
		clear();
	}
	// Clear array data
	void clear()
	{
		if (array_ != NULL) delete[] array_;
		if (sliceOffsets_ != NULL) delete[] sliceOffsets_;
		sliceOffsets_ = NULL;
		array_ = NULL;
		linearSize_ = 0;
		nX_ = 0;
		nY_ = 0;
		nZ_ = 0;
	}
	// Copy Constructor
	Array3D(const Array3D<A>& source)
	{
		array_ = NULL;
		linearSize_ = 0;
		sliceOffsets_ = NULL;
		nX_ = 0;
		nY_ = 0;
		nZ_ = 0;
		(*this) = source;
	}
	// Assignment Operator
	void operator=(const A value)
	{
		// Copy source data elements
		for (int n=0; n<linearSize_; ++n) array_[n] = value;
	}
	// Assignment Operator
	void operator=(const Array3D<A>& source)
	{
		// Clear any existing data and reinitialise the array
		clear();
		initialise(source.nX_, source.nY_, source.nZ_);

		for (int n=0; n<linearSize_; ++n) array_[n] = source.array_[n];
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
	int nX_, nY_, nZ_;
	// XY slice offsets
	int* sliceOffsets_;

	private:
	// Resize array 
	void resize(int nX, int nY, int nZ)
	{
		// Clear old data
		clear();

		// Create new array
		nX_ = nX;
		nY_ = nY;
		nZ_ = nZ;
		linearSize_ = nX_*nY_*nZ_;
		array_ = new A[linearSize_];

		// Create slice offsets array
		sliceOffsets_ = new int[nZ_];
		for (int n=0; n<nZ_; ++n) sliceOffsets_[n] = n*nX_*nY_;
	}

	public:
	// Initialise array
	void initialise(int nX, int nY, int nZ)
	{
		clear();

		if ((nX > 0) && (nY > 0) && (nZ > 0)) resize(nX, nY, nZ);
// 		else printf("BAD_USAGE - Zero or negative row/column size(s) given to Array3D::initialise() (r=%i, c=%i)\n", nrows, ncolumns);
	}
	// Return specified element as modifiable reference
	A& at(int x, int y, int z)
	{
#ifdef CHECKS
		static A dummy;
		if ((x < 0) || (x >= nX_))
		{
			Messenger::print("OUT_OF_RANGE - X index (%i) is out of range in Array3D::at() (nX_ = %i).\n", x, nX_);
			return dummy;
		}
		if ((y < 0) || (y >= nY_))
		{
			Messenger::print("OUT_OF_RANGE - Y index (%i) is out of range in Array3D::at() (nY_ = %i).\n", y, nY_);
			return dummy;
		}
		if ((z < 0) || (z >= nZ_))
		{
			Messenger::print("OUT_OF_RANGE - Z index (%i) is out of range in Array3D::at() (nZ_ = %i).\n", z, nZ_);
			return dummy;
		}
#endif
		return array_[sliceOffsets_[z] + y*nX_ + x];
	}
	// Return specified element as const-reference
	A& constAt(int x, int y, int z) const
	{
#ifdef CHECKS
		static A dummy;
		if ((x < 0) || (x >= nX_))
		{
			Messenger::print("OUT_OF_RANGE - X index (%i) is out of range in Array3D::constAt() (nX_ = %i).\n", x, nX_);
			return dummy;
		}
		if ((y < 0) || (y >= nY_))
		{
			Messenger::print("OUT_OF_RANGE - Y index (%i) is out of range in Array3D::constAt() (nY_ = %i).\n", y, nY_);
			return dummy;
		}
		if ((z < 0) || (z >= nZ_))
		{
			Messenger::print("OUT_OF_RANGE - Z index (%i) is out of range in Array3D::constAt() (nZ_ = %i).\n", z, nZ_);
			return dummy;
		}
#endif
		return array_[sliceOffsets_[z] + y*nX_ + x];
	}
	// Return address of specified element
	A* ptr(int x, int y, int z)
	{
#ifdef CHECKS
		static A dummy;
		if ((x < 0) || (x >= nX_))
		{
			Messenger::print("OUT_OF_RANGE - X index (%i) is out of range in Array3D::ptr() (nX_ = %i).\n", x, nX_);
			return dummy;
		}
		if ((y < 0) || (y >= nY_))
		{
			Messenger::print("OUT_OF_RANGE - Y index (%i) is out of range in Array3D::ptr() (nY_ = %i).\n", y, nY_);
			return dummy;
		}
		if ((z < 0) || (z >= nZ_))
		{
			Messenger::print("OUT_OF_RANGE - Z index (%i) is out of range in Array3D::ptr() (nZ_ = %i).\n", z, nZ_);
			return dummy;
		}
#endif
		return &array_[sliceOffsets_[z] + y*nX_ + x];
	}
	// Return array size in x
	int nX() const
	{
		return nX_;
	}
	// Return array size in yu
	int nY() const
	{
		return nY_;
	}
	// Return array size in z
	int nZ() const
	{
		return nZ_;
	}
	// Return linear array size
	int linearArraySize() const
	{
		return linearSize_;
	}
	// Return linear array
	A* linearArray()
	{
		return array_;
	}
	// Return linear value
	A& linearValue(int index)
	{
#ifdef CHECKS
		static A dummy;
		if ((index < 0) || (index >= linearSize_))
		{
			Messenger::print("OUT_OF_RANGE - Index (%i) is out of range in Array3D::linearValue() (linearSize = %i).\n", index, linearSize_);
			return dummy;
		}
#endif
		return array_[index];
	}
	// Return linear value (const)
	A& constLinearValue(int index) const
	{
#ifdef CHECKS
		static A dummy;
		if ((index < 0) || (index >= linearSize_))
		{
			Messenger::print("OUT_OF_RANGE - Index (%i) is out of range in Array3D::constLinearValue() (linearSize = %i).\n", index, linearSize_);
			return dummy;
		}
#endif
		return array_[index];
	}


	/*
	 * Operators
	 */
	public:
	// Operator+= (add to all)
	void operator+=(const A value) { for (int n=0; n<linearSize_; ++n) array_[n] += value; }
	// Operator-= (subtract from all)
	void operator-=(const A value) { for (int n=0; n<linearSize_; ++n) array_[n] -= value; }
	// Operator*= (multiply all)
	void operator*=(const A value) { for (int n=0; n<linearSize_; ++n) array_[n] *= value; }
	// Operator/= (divide all)
	void operator/=(const A value) { for (int n=0; n<linearSize_; ++n) array_[n] /= value; }
};

// OffsetArray3D
template <class A> class OffsetArray3D
{
	public:
	// Constructor
	OffsetArray3D(int xMin, int xMax, int yMin, int yMax, int zMin, int zMax)
	{
		array_ = NULL;
		linearSize_ = 0;
		sliceOffsets_ = NULL;

		initialise(xMin, xMax, yMin, yMax, zMin, zMax);
	}
	// Destructor
	~OffsetArray3D()
	{
		clear();
	}
	// Clear array data
	void clear()
	{
		if (array_ != NULL) delete[] array_;
		if (sliceOffsets_ != NULL) delete[] sliceOffsets_;
		sliceOffsets_ = NULL;
		array_ = NULL;
		linearSize_ = 0;
		xMin_ = 0;
		yMin_ = 0;
		zMin_ = 0;
		xMax_ = 0;
		yMax_ = 0;
		zMax_ = 0;
		nX_ = 0;
		nY_ = 0;
		nZ_ = 0;
	}
	// Copy Constructor
	OffsetArray3D(const OffsetArray3D<A>& source)
	{
		array_ = NULL;
		linearSize_ = 0;
		sliceOffsets_ = NULL;
		xMin_ = 0;
		yMin_ = 0;
		zMin_ = 0;
		xMax_ = 0;
		yMax_ = 0;
		zMax_ = 0;
		nX_ = 0;
		nY_ = 0;
		nZ_ = 0;
		(*this) = source;
	}
	// Assignment Operator
	void operator=(const A value)
	{
		// Copy source data elements
		for (int n=0; n<linearSize_; ++n) array_[n] = value;
	}
	// Assignment Operator
	void operator=(const Array3D<A>& source)
	{
		// Clear any existing data and reinitialise the array
		clear();
		initialise(source.nX_, source.nY_, source.nZ_);

		for (int n=0; n<linearSize_; ++n) array_[n] = source.array_[n];
	}


	/*
	 * Data
	 */
	private:
	// Linear array of objects
	A* array_;
	// Size of linear array
	int linearSize_;
	// Array limits
	int xMin_, xMax_, yMin_, yMax_, zMin_, zMax_;
	// Array dimensions
	int nX_, nY_, nZ_;
	// XY slice offsets
	int* sliceOffsets_;

	public:
	// Initialise array
	void initialise(int xMin, int xMax, int yMin, int yMax, int zMin, int zMax)
	{
		clear();

		xMin_ = xMin;
		yMin_ = yMin;
		zMin_ = zMin;
		xMax_ = xMax;
		yMax_ = yMax;
		zMax_ = zMax;
		nX_ = (xMax_ - xMin_)+1;
		nY_ = (yMax_ - yMin_)+1;
		nZ_ = (zMax_ - zMin_)+1;
		linearSize_ = nX_*nY_*nZ_;

		if ((nX_ > 0) && (nY_ > 0) && (nZ_ > 0))
		{
			array_ = new A[linearSize_];

			// Create slice offsets array
			sliceOffsets_ = new int[nZ_];
			for (int n=0; n<nZ_; ++n) sliceOffsets_[n] = n*nX_*nY_;
		}
// 		else printf("BAD_USAGE - Zero or negative row/column size(s) given to Array3D::initialise() (r=%i, c=%i)\n", nrows, ncolumns);
	}
	// Return specified element as reference
	A& at(int x, int y, int z)
	{
#ifdef CHECKS
		static A dummy;
		if ((x < xMin_) || (x > xMax_))
		{
			Messenger::print("OUT_OF_RANGE - X index (%i) is out of range in OffsetArray3D::ref() (xMin_ = %i, xMax_ = %i).\n", x, xMin_, xMax_);
			return dummy;
		}
		if ((y < yMin_) || (y > yMax_))
		{
			Messenger::print("OUT_OF_RANGE - Y index (%i) is out of range in OffsetArray3D::ref() (yMin_ = %i, yMay_ = %i).\n", y, yMin_, yMax_);
			return dummy;
		}
		if ((z < zMin_) || (z > zMax_))
		{
			Messenger::print("OUT_OF_RANGE - Z index (%i) is out of range in OffsetArray3D::ref() (zMin_ = %i, zMaz_ = %i).\n", z, zMin_, zMax_);
			return dummy;
		}
#endif
		return array_[sliceOffsets_[z-zMin_] + (y-yMin_)*nX_ + (x-xMin_)];
	}
	// Return specified element as const reference
	A& constAt(int x, int y, int z) const
	{
#ifdef CHECKS
		static A dummy;
		if ((x < xMin_) || (x > xMax_))
		{
			Messenger::print("OUT_OF_RANGE - X index (%i) is out of range in OffsetArray3D::value() (xMin_ = %i, xMax_ = %i).\n", x, xMin_, xMax_);
			return dummy;
		}
		if ((y < yMin_) || (y > yMax_))
		{
			Messenger::print("OUT_OF_RANGE - Y index (%i) is out of range in OffsetArray3D::value() (yMin_ = %i, yMay_ = %i).\n", y, yMin_, yMax_);
			return dummy;
		}
		if ((z < zMin_) || (z > zMax_))
		{
			Messenger::print("OUT_OF_RANGE - Z index (%i) is out of range in OffsetArray3D::value() (zMin_ = %i, zMaz_ = %i).\n", z, zMin_, zMax_);
			return dummy;
		}
#endif
		return array_[sliceOffsets_[z-zMin_] + (y-yMin_)*nX_ + (x-xMin_)];
	}
	// Return address of specified element
	A* pointerAt(int x, int y, int z)
	{
#ifdef CHECKS
		static A dummy;
		if ((x < xMin_) || (x > xMax_))
		{
			Messenger::print("OUT_OF_RANGE - X index (%i) is out of range in OffsetArray3D::ptr() (xMin_ = %i, xMax_ = %i).\n", x, xMin_, xMax_);
			return dummy;
		}
		if ((y < yMin_) || (y > yMax_))
		{
			Messenger::print("OUT_OF_RANGE - Y index (%i) is out of range in OffsetArray3D::ptr() (yMin_ = %i, yMay_ = %i).\n", y, yMin_, yMax_);
			return dummy;
		}
		if ((z < zMin_) || (z > zMax_))
		{
			Messenger::print("OUT_OF_RANGE - Z index (%i) is out of range in OffsetArray3D::ptr() (zMin_ = %i, zMaz_ = %i).\n", z, zMin_, zMax_);
			return dummy;
		}
#endif
		return &array_[sliceOffsets_[z-zMin_] + (y-yMin_)*nX_ + (x-xMin_)];
	}
	// Return array size in x
	int nX() const
	{
		return nX_;
	}
	// Return array size in yu
	int nY() const
	{
		return nY_;
	}
	// Return array size in z
	int nZ() const
	{
		return nZ_;
	}
	// Return linear array size
	int linearArraySize() const
	{
		return linearSize_;
	}
	// Return linear array
	A* linearArray()
	{
		return array_;
	}
	// Return linear value
	A& linearValue(int index)
	{
#ifdef CHECKS
		static A dummy;
		if ((index < 0) || (index >= linearSize_))
		{
			Messenger::print("OUT_OF_RANGE - Index (%i) is out of range in OffsetArray3D::linearValue() (linearSize = %i).\n", index, linearSize_);
			return dummy;
		}
#endif
		return array_[index];
	}
	// Return linear value (const)
	A& constLinearValue(int index) const
	{
#ifdef CHECKS
		static A dummy;
		if ((index < 0) || (index >= linearSize_))
		{
			Messenger::print("OUT_OF_RANGE - Index (%i) is out of range in OffsetArray3D::constLinearValue() (linearSize = %i).\n", index, linearSize_);
			return dummy;
		}
#endif
		return array_[index];
	}


	/*
	 * Operators
	 */
	public:
	// Operator+= (add to all)
	void operator+=(const A value) { for (int n=0; n<linearSize_; ++n) array_[n] += value; }
	// Operator-= (subtract from all)
	void operator-=(const A value) { for (int n=0; n<linearSize_; ++n) array_[n] -= value; }
	// Operator*= (multiply all)
	void operator*=(const A value) { for (int n=0; n<linearSize_; ++n) array_[n] *= value; }
	// Operator/= (divide all)
	void operator/=(const A value) { for (int n=0; n<linearSize_; ++n) array_[n] /= value; }
};

#endif
