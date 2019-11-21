/*
	*** Range
	*** src/math/range.cpp
	Copyright T. Youngs 2013-2019

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

#include "math/range.h"

// Constructor
Range::Range(double minimum, double maximum)
{
	minimum_ = minimum;
	maximum_ = maximum;
}

/*
 * Data
 */

// Set range
void Range::set(double minimum, double maximum)
{
	minimum_ = minimum;
	maximum_ = maximum;
}

// Set minimum for range
void Range::setMinimum(double minimum)
{
	minimum_ = minimum;
}

// Return minimum for range
double Range::minimum() const
{
	return minimum_;
}

// Set maximum for range
void Range::setMaximum(double maximum)
{
	maximum_ = maximum;
}

// Return maximum for range
double Range::maximum() const
{
	return maximum_;
}
