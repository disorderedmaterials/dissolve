/*
	*** Single-Value Decomposition
	*** src/math/svd.h
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

#ifndef DISSOLVE_SVD_H
#define DISSOLVE_SVD_H

// Forward Declarations
template <class A> class Array2D;

// Single Value Decomposition
class SVD
{
	public:
	// Perform single value decomposition of the supplied matrix A, putting left-orthogonal (U), diagonal single-value (S), and right-orthogonal (V) matrices into the supplied Arrays
	static bool decompose(const Array2D<double>& A, Array2D<double>& U, Array2D<double>& S, Array2D<double>& Vt);
	// Test SVD
	static bool test();
	// Compute in-place pseudoinverse of supplied matrix
	static bool pseudoinverse(Array2D<double>& A);
};

#endif
