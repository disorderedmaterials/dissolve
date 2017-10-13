/*
	*** Double with Exponent class
	*** src/math/doubleexp.h
	Copyright T. Youngs 2012-2017

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

#ifndef DUQ_DOUBLEEXP_H
#define DUQ_DOUBLEEXP_H

#include "base/charstring.h"

// Mantissa/exponent class
class DoubleExp
{
	public:
	// Constructor / Destructor
	DoubleExp(double mantissa = 0.0, int exponent = 0);
	// Operator =
	void operator=(double d);
	// Conversion (double)
	operator double();

	/*
	 * Data
	 */
	private:
	// Mantissa
	double mantissa_;
	// Exponent
	int exponent_;
	// Value
	double value_;

	/*
	 * Functions
	 */
	private:
	// Recalculate value from stored mantissa and exponent
	void recalculate();

	public:
	// Retrieve full, real value
	double value() const;
	// Set mantissa and exponent
	void set(double mantissa, int exponent);
	// Set from normal value
	void set(double value);
	// Set mantissa
	void setMantissa(double mantissa);
	// Return mantissa
	double mantissa() const;
	// Set exponent
	void setExponent(int exponent);
	// Return exponent
	int exponent() const;
	// Return value as string
	CharString asString();
};

#endif

