/*
	*** Limits
	*** src/math/limits.cpp
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

#include "math/limits.h"
#include "templates/array.h"
#include "templates/array2d.h"


// Return minimum from 1D array provided
double Limits::min(const Array<double>& A)
{	
	double min = A.constAt(0);
	for(int i=0; i<A.nItems(); ++i)
	{
		if (A.constAt(i)<min)
			min = A.constAt(i);
	}
	return min;
}

// Return maximum from 1D array provided
double Limits::max(const Array<double>& A)
{
	double max = A.constAt(0);
	for(int i=0; i<A.nItems(); ++i)
	{
		if (A.constAt(i)>max)
			max = A.constAt(i);
	}
	return max;
}

// Return minimum from 2D array provided
double Limits::min(const Array2D<double>& A)
{
	double min = A.constLinearValue(0);
	for (int i=0; i<A.linearArraySize(); ++i)
	{
		if (A.constLinearValue(i) < min)
			min = A.constLinearValue(i);
	}
	return min;
}

// Return maximum from 2D array provided
double Limits::max(const Array2D<double>& A)
{
	double max = A.constLinearValue(0);
	for (int i=0; i<A.linearArraySize(); ++i)
	{
		if (A.constLinearValue(i) > max )
			max = A.constLinearValue(i);
	}
	return max;
}

// Return absolute minimum from array provided
double Limits::absMin(const Array<double>& A)
{	
	double absMin = abs(A.constAt(0));
	for(int i=0; i<A.nItems(); ++i)
	{
		if (abs(A.constAt(i)) < absMin)
			absMin = abs(A.constAt(i));
	}
	return absMin;
}

// Return absolute maximum from array provided
double Limits::absMax(const Array<double>& A)
{
	double absMax = abs(A.constAt(0));
	for(int i=0; i<A.nItems(); ++i)
	{
		if (abs(A.constAt(i)) > absMax)
			absMax = abs(A.constAt(i));
	}
	return absMax;
	
}

// Return absolute minimum from 2D array provided
double Limits::absMin(const Array2D<double>& A)
{
	double absMin = abs(A.constLinearValue(0));
	for (int i=0; i<A.linearArraySize(); ++i)
	{
		if (abs(A.constLinearValue(i)) < absMin)
			absMin = abs(A.constLinearValue(i));
	}
	return absMin;
}

// Return absolute maximum from 2D array provided
double Limits::absMax(const Array2D<double>& A)
{
	double absMax = abs(A.constLinearValue(0));
	for (int i=0; i<A.linearArraySize(); ++i)
	{
		if (abs(A.constLinearValue(i)) < absMax)
			absMax = abs(A.constLinearValue(i));
	}
	return absMax;
}
