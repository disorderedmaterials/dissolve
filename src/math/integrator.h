/*
	*** Integrator
	*** src/math/integrator.h
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

#ifndef DISSOLVE_INTEGRATOR_H
#define DISSOLVE_INTEGRATOR_H

// Forward Declarations
class Data1D;
class Data2D;
class Data3D;

// Integrator
class Integrator
{
	/*
	 * Static Functions
	 */
	public:
	// Compute integral of supplied data via trapezoid rule
	static double trapezoid(const Data1D& data);
	// Compute integral of supplied data via trapezoid rule between the specified limits
	static double trapezoid(const Data1D& data, double xMin, double xMax);
	// Compute absolute integral of supplied data via trapezoid rule
	static double absTrapezoid(const Data1D& data);
	// Return sum of all values in supplied data
	static double sum(const Data1D& data);
	// Return sum of all absolute values in supplied data
	static double absSum(const Data1D& data);
	// Return sum of squares of all values in supplied data
	static double sumOfSquares(const Data1D& data);
	// Return sum of all values in supplied data
	static double sum(const Data2D& data);
	// Return sum of all absolute values in supplied data
	static double absSum(const Data2D& data);
	// Return sum of all values in supplied data
	static double sum(const Data3D& data);
	// Return sum of all absolute values in supplied data
	static double absSum(const Data3D& data);
};

#endif
