/*
	*** Math functions
	*** src/lib/math/mathfunc.h
	Copyright T. Youngs 2012-2014

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

#ifndef DUQ_MATHFUNC_H
#define DUQ_MATHFUNC_H

// Mathematical functions
class DUQMath
{
	// Error functions
	public:
	static double erfc(double);
	static double erf(double);

	// Random number generation
	public:
	static double random();
	static double randomPlusMinusOne();
	static int randomimax();
	static int randomi(int range);

	// Integer power function
	public:
	static int power(int i, int p);
	
	// Sign function
	static int sgn(int x);
};

#endif
