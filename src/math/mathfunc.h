/*
	*** Math Functions
	*** src/math/mathfunc.h
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

#ifndef DISSOLVE_MATHFUNC_H
#define DISSOLVE_MATHFUNC_H

// Forward Declarations
template <class A> class Array2D;

// Mathematical functions
class DissolveMath
{
	/*
	 * Error Functions
	 */
	public:
	static double erfc(double);
	static double erf(double);


	/*
	 * Random Number Generation
	 */
	public:
	static double random();
	static double randomPlusMinusOne();
	static int randomimax();
	static int randomi(int range);


	/*
	 * Integer Functions
	 */
	public:
	// Raise the integer 'i' to the power 'p'
	static int power(int i, int p);
	// Sign function
	static int sgn(int x);
	// Sign function
	static int sgn(double x);
	// Apply sign of second argument to first
	static double sgn(double a, double signOf);
	// Return the cyclic permutation of the integer 'i', span 3
	static int cp3(int i);
};

#endif
