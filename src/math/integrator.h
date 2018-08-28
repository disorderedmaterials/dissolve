/*
	*** Integrator
	*** src/math/integrator.h
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

#ifndef DISSOLVE_INTEGRATOR_H
#define DISSOLVE_INTEGRATOR_H

// Forward Declarations
class XYData;

// Integrator
class Integrator
{
	/*
	 * Static Functions
	 */
	public:
	// Compute integral of supplied data
	static double integral(const XYData& data);
	// Compute absolute integral of supplied data
	static double absIntegral(const XYData& data);
	// Return sum of squares of all y values in supplied data
	static double sumOfSquares(const XYData& data);
};

#endif
