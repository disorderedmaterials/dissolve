/*
	*** Gauss-Jordan Matrix Inversion
	*** src/math/gj.h
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
	along wit Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DISSOLVE_GAUSSJORDAN_H
#define DISSOLVE_GAUSSJORDAN_H

// Forward Declarations
template <class A> class Array2D;

// Gauss-Jordan Inversion
class GaussJordan
{
	public:
	// Perform Gauss-Jordan inversion of the supplied Array2D<double>
	static bool invert(Array2D<double>& A);
};

#endif
