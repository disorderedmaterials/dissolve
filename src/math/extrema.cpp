/*
	*** Extrema
	*** src/math/extrema.cpp
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

#include "math/extrema.h"
#include "templates/array.h"
#include "templates/array2d.h"

// Return minimum from 1D array provided
double Extrema::min(const Array<double>& A)
{	
	if(A.nItems()>0)
	{
		double min = A.constAt(0);
		for(int i=0; i<A.nItems(); ++i)
		{
			if (A.constAt(i)<min)
				min = A.constAt(i);
		}
		return min;
	}
	else
		return 0;
	
}

// Return maximum from 1D array provided
double Extrema::max(const Array<double>& A)
{
	if(A.nItems()>0)
	{
		double max = A.constAt(0);
		for(int i=0; i<A.nItems(); ++i)
		{
			if (A.constAt(i)>max)
				max = A.constAt(i);
		}
		return max;
	}
	else
		return 0;
}

// Return minimum from 2D array provided
double Extrema::min(const Array2D<double>& A)
{
	if(A.linearArraySize()>0)
	{
			double min = A.constLinearValue(0);
		for (int i=0; i<A.linearArraySize(); ++i)
		{
			if (A.constLinearValue(i) < min)
				min = A.constLinearValue(i);
		}
		return min;
	}
	else 
		return 0;
}

// Return maximum from 2D array provided
double Extrema::max(const Array2D<double>& A)
{
	if(A.linearArraySize()>0)
	{
		double max = A.constLinearValue(0);
		for (int i=0; i<A.linearArraySize(); ++i)
		{
			if (A.constLinearValue(i) > max )
				max = A.constLinearValue(i);
		}
		return max;
	}
	else
		return 0;
}

// Return absolute minimum from array provided
double Extrema::absMin(const Array<double>& A)
{	
	if (A.nItems() > 0)
	{
		double absMin = abs(A.constAt(0));
		for(int i=0; i<A.nItems(); ++i)
		{
			if (abs(A.constAt(i)) < absMin)
				absMin = abs(A.constAt(i));
		}
		return absMin;
	}
	else
		return 0;
	
}

// Return absolute maximum from array provided
double Extrema::absMax(const Array<double>& A)
{
	if (A.nItems() >0)
	{
		double absMax = abs(A.constAt(0));
		for(int i=0; i<A.nItems(); ++i)
		{
			if (abs(A.constAt(i)) > absMax)
				absMax = abs(A.constAt(i));
		}
		return absMax;
	}
	else 
		return 0;
		
}

// Return absolute minimum from 2D array provided
double Extrema::absMin(const Array2D<double>& A)
{
	if(A.linearArraySize()>0)
	{
		double absMin = abs(A.constLinearValue(0));
		for (int i=0; i<A.linearArraySize(); ++i)
		{
			if (abs(A.constLinearValue(i)) < absMin)
				absMin = abs(A.constLinearValue(i));
		}
		return absMin;
	}
	else return 0;
}

// Return absolute maximum from 2D array provided
double Extrema::absMax(const Array2D<double>& A)
{
	if(A.linearArraySize()>0)
	{
		double absMax = abs(A.constLinearValue(0));
		for (int i=0; i<A.linearArraySize(); ++i)
		{
			if (abs(A.constLinearValue(i)) < absMax)
				absMax = abs(A.constLinearValue(i));
		}
		return absMax;
	}
	else 
		return 0;
	
}
