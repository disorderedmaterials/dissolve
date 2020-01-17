/*
	*** Range Class
	*** src/math/range.h
	Copyright T. Youngs 2013-2020

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

#ifndef DISSOLVE_RANGE_H
#define DISSOLVE_RANGE_H

#include "math/matrix4.h"

// Range
class Range
{
	public:
	// Constructor / Destructor
	Range(double minimum = 0.0, double maximum = 0.0);


	/*
	 * Data
	 */
	private:
	// Minimum and maximum for range
	double minimum_, maximum_;

	public:
	// Set range
	void set(double minimum, double maximum);
	// Set minimum for range
	void setMinimum(double minimum);
	// Return minimum for range
	double minimum() const;
	// Set maximum for range
	void setMaximum(double maximum);
	// Return maximum for range
	double maximum() const;
};

#endif

