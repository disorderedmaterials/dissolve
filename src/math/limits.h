/*
	*** Limits
	*** src/math/limits.h
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


#ifndef DISSOLVE_LIMITS_H
#define DISSOLVE_LIMITS_H

#include "data1d.h"
// Forward Declarations
class Data1D;
class Data2D;

// Limits
class Limits
{
	public:
	// Return minimum from array provided
	static double min(const Array<double>& A);
	// Return maximum from array provided
	static double max(const Array<double>& A);
	// Return minimum from 2D array provided
	static double min(const Array2D<double>& A);
	// Return maximum from 2D array provided
	static double max(const Array2D<double>& A);
	// Return absolute minimum from array provided
	static double absMin(const Array<double>& A);
	// Return absolute maximum from array provided
	static double absMax(const Array<double>& A);
	// Return absolute minimum from 2D array provided
	static double absMin(const Array2D<double>& A);
	// Return absolute maximum from 2D array provided
	static double absMax(const Array2D<double>& A);
	
	
	
};

#endif
