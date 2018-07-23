/*
	*** Singular Value Decomposition
	*** src/math/svd.cpp
	Copyright T. Youngs 2018

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

#include "math/svd.h"
#include "templates/array.h"
#include "templates/array2d.h"
#include <algorithm>

static double sqrarg;
#define SQR(a) ((sqrarg = (a)) == 0.0 ? 0.0 : sqrarg * sqrarg)

// calculates sqrt( a^2 + b^2 ) with decent precision
double pythag(double a, double b) {
  double absa,absb;

  absa = fabs(a);
  absb = fabs(b);

  if(absa > absb)
    return(absa * sqrt(1.0 + SQR(absb/absa)));
  else
    return(absb == 0.0 ? 0.0 : absb * sqrt(1.0 + SQR(absa / absb)));
}

// Perform single value decomposition of the supplied matrix A, putting left-orthogonal (U), diagonal single-value (S), and right-orthogonal (V transpose) matrices into the supplied Arrays
bool SVD::decompose(const Array2D<double>& A, Array2D<double>& U, Array2D<double>& S, Array2D<double>& Vt)
{
	int flag,i,its,j,jj,k,l,nm;
	double anorm,c,f,g,h,s,scale,x,y,z;
	
	// Get matrix dimensions and create / initialise arrays
	const int nCols = A.nColumns();
	const int nRows = A.nRows();
  
	// Check for nRows >= nCols
	if (nRows < nCols) return Messenger::error("DissolveMath::svd() - nRows must be greater than nCols.\n");
	Array<double> rv1(nCols);
	rv1 = 0.0;
	S.initialise(nCols,nCols);
	S = 0.0;
	Vt.initialise(nCols,nCols);
	Vt = 0.0;
	U = A;

	g = 0.0;
	scale = 0.0;
	anorm = 0.0;
	for(i=0;i<nCols;i++)
	{
		l = i+1;
		rv1[i] = scale*g;
		g = 0.0;
		s = 0.0;
		scale = 0.0;
		if(i < nRows)
		{
			for(k=i;k<nRows;k++) scale += fabs(U.value(k,i));
			if(scale != 0.0)
			{
				for(k=i;k<nRows;k++)
				{
				U.ref(k,i) /= scale;
				s += U.value(k,i) * U.value(k,i);
				}

				f = U.value(i,i);
				g = -DissolveMath::sgn(sqrt(s),f);
				h = f * g - s;
				U.ref(i,i) = f - g;

				for(j=l;j<nCols;j++)
				{
					for(s=0.0,k=i;k<nRows;k++) s += U.value(k,i) * U.value(k,j);
					f = s / h;
					for(k=i;k<nRows;k++) U.ref(k,j) += f * U.value(k,i);
				}
				for(k=i;k<nRows;k++) U.ref(k,i) *= scale;
			}
		}
		S.ref(i,i) = scale * g;
		g = 0.0;
		s = 0.0;
		scale = 0.0;
		if(i < nRows && i != nCols-1)
		{
			for(k=l;k<nCols;k++) scale += fabs(U.value(i,k));
			if (scale)
			{
				for(k=l;k<nCols;k++)
				{
					U.ref(i,k) /= scale;
					s += U.value(i,k) * U.value(i,k);
				}
				f = U.value(i,l);
				g = - DissolveMath::sgn(sqrt(s),f);
				h = f * g - s;
				U.ref(i,l) = f - g;
				for(k=l;k<nCols;k++) rv1[k] = U.value(i,k) / h;
				if (i != (nRows - 1))
				{
					for(j=l;j<nRows;j++)
					{
						for(s=0.0,k=l;k<nCols;k++) s += U.value(j,k) * U.value(i,k);
						for(k=l;k<nCols;k++) U.ref(j,k) += s * rv1[k];
					}
				}
				for(k=l;k<nCols;k++) U.ref(i,k) *= scale;
			}
		}
		anorm = max(anorm, (fabs(S.value(i,i)) + fabs(rv1[i])));
	}

	// Accumulate right-hand transformation
	for(i=nCols-1;i>=0;i--)
	{
		if(i < nCols-1)
		{
			if (g)
			{
				for(j=l;j<nCols;j++) Vt.ref(j,i) = (U.value(i,j) / U.value(i,l)) / g;
				for(j=l;j<nCols;j++)
				{
					for(s=0.0,k=l;k<nCols;k++) s += U.value(i,k) * Vt.value(k,j);
					for(k=l;k<nCols;k++) Vt.ref(k,j) += s * Vt.value(k,i);
				}
			}

			for(j=l;j<nCols;j++)
			{
				Vt.ref(i,j) = 0.0;
				Vt.ref(j,i) = 0.0;
			}
		}
		Vt.ref(i,i) = 1.0;
		g = rv1[i];
		l = i;
	}

	// Assumulate left-hand transformation
	//   for(i=min(nRows,nCols) - 1;i >= 0;i--) {
	for(i=nCols - 1;i >= 0;i--)
	{
		l = i + 1;
		g = S.value(i,i);
		if (i < (nCols-1)) for(j=l;j<nCols;j++) U.ref(i,j) = 0.0;
		if (g)
		{
			g = 1.0 / g;
			if (i != (nCols-1))	// TGAY
			{
				for(j=l;j<nCols;j++)
				{
					for (s=0.0,k=l;k<nRows;k++) s += U.value(k,i) * U.value(k,j);
					f = (s / U.value(i,i)) * g;
					for (k=i;k<nRows;k++) U.ref(k,j) += f * U.value(k,i);
				}
			}
			for(j=i;j<nRows;j++) U.ref(j,i) *= g;
		}
		else
		{
			for(j=i;j<nRows;j++) U.ref(j,i) = 0.0;
		}

		++U.ref(i,i);
	}

	// Diagonalise the bidiagonal form
	for(k=nCols-1;k>=0;k--)
	{
		for(its=0;its<30;its++)
		{
			flag = 1;
			for(l=k;l>=0;l--)
			{
				nm = l-1;
				if((fabs(rv1[l]) + anorm) == anorm)
				{
					flag =  0;
					break;
				}
				if((fabs(S.value(nm,nm)) + anorm) == anorm) break;
			}
			if(flag)
			{
				c = 0.0;
				s = 1.0;
				for(i=l;i<=k;i++)
				{
					f = s * rv1[i];
				
				// 	  rv1[i] = c * rv1[i];
					if((fabs(f) + anorm) != anorm)
					{
						g = S.value(i,i);
						h = pythag(f,g);
						S.ref(i,i) = h;
						h = 1.0 / h;
						c = g * h;
						s = -f * h;
						for(j=0;j<nRows;j++)
						{
							y = U.value(j,nm);
							z = U.value(j,i);
							U.ref(j,nm) = y * c + z * s;
							U.ref(j,i) = z * c - y * s;
						}
					}
				}
			}
			z = S.value(k,k);
			if (l == k)
			{
				if(z < 0.0)
				{
					S.ref(k,k) = -z;
					for(j=0;j<nCols;j++) Vt.ref(j,k) = -Vt.value(j,k);
				}
				break;
			}
			if(its == 29) return Messenger::error("DissolveMath::svd() - No convergence in 30 iterations.\n");

			// Shift from bottom 2x2 minor
			x = S.value(l,l);
			nm = k-1;
			y = S.value(nm,nm);
			g = rv1[nm];
			h = rv1[k];
			f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
			g = pythag(f,1.0);
			f = ((x - z) * (x + z) + h * ((y / (f + DissolveMath::sgn(g,f))) - h)) / x;
			c = 1.0;
			s = 1.0;
			for(j=l;j<=nm;j++)
			{
				i = j+1;
				g = rv1[i];
				y = S.value(i,i);
				h = s * g;
				g = c * g;
				z = pythag(f,h);
				rv1[j] = z;
				c = f/z;
				s = h/z;
				f = x * c + g * s;
				g = g * c - x * s;
				h = y * s;
				y *= c;
				for(jj=0;jj<nCols;jj++)
				{
					x = Vt.value(jj,j);
					z = Vt.value(jj,i);
					Vt.ref(jj,j) = x * c + z * s;
					Vt.ref(jj,i) = z * c - x * s;
				}
				z = pythag(f,h);
				S.ref(j,j) = z;
				if(z)
				{
					z = 1.0 / z;
					c = f * z;
					s = h * z;
				}
				f = c * g + s * y;
				x = c * y - s * g;
				for(jj=0;jj < nRows;jj++)
				{
					y = U.value(jj,j);
					z = U.value(jj,i);
					U.ref(jj,j) = y * c + z * s;
					U.ref(jj,i) = z * c - y * s;
				}
			}
			rv1[l] = 0.0;
			rv1[k] = f;
			S.ref(k,k) = x;
		}
	}

	// Transpose Vt
	Vt.transpose();

	return true;
}

// Test SVD
void SVD::test()
{
	Array2D<double> A, U, V, S, I, Ut, Vt;

	/*
	 * Decompose the identity matrix I.
	 * Should result in U = -I, S = I, and V = -I.
	 */
	A.initialise(3,3);
	A = 0.0;
	A.ref(0,0) = 1.0;
	A.ref(1,1) = 1.0;
	A.ref(2,2) = 1.0;
	A.print("A");
	decompose(A, U, S, V);
	U.print("U");
	S.print("S");
	V.print("V");

	// Test for unitary matrices U and V (take transpose and multiply)
	Ut = U.transposed();
	Ut.print("Ut");
	I = U * Ut;
	I.print("U * Ut");

	Vt = V.transposed();
	Vt.print("Vt");
	I = V * Vt;
	I.print("V * Vt");

	/*
	 */
	A.initialise(5,4);
	A = 0.0;
	A.ref(0,0) = 1.0;
	A.ref(4,0) = 2.0;
	A.ref(2,1) = 3.0;
	A.ref(1,3) = 2.0;
	A.print("A");
	decompose(A, U, S, V);
	U.print("U");
	S.print("S");
	V.print("V");

	// Test for unitary matrices U and V (take transpose and multiply)
	Ut = U.transposed();
	Ut.print("Ut");
	I = U * Ut;
	I.print("U * Ut");

	Vt = V.transposed();
	Vt.print("Vt");
	I = V * Vt;
	I.print("V * Vt");
}

// Compute in-place pseudoinverse of supplied matrix
bool SVD::pseudoinverse(Array2D<double>& A)
{
	// First, compute SVD of the matrix A
	Array2D<double> U, S, Vt;
	if (!decompose(A, U, S, Vt)) return false;
// 	U.print("U");
// 	S.print("S");
// 	V.print("V");

	// Perform a quick sanity check on the decomposition
	Array2D<double> A2;
	A2 = U * S * Vt;
	for (int n=0; n<A.nRows(); ++n)
	{
		// TODO Need better double comparison here
		for (int m=0; m<A.nColumns(); ++m) if (fabs(A.value(n,m)-A2.value(n,m)) > 1.0e-9) return Messenger::error("DissolveMath::pseudoinverse() - SVD does not appear to be valid.\n");
	}
// 	A.print("Original A");
// 	A2.print("Recombined A");

	// Take the diagonal single-value matrix S and form its pseudoinverse.
	// This amounts to taking each non-zero diagonal element and replacing it with its reciprocal
	Array2D<double> Splus = S;
	for (int n=0; n<S.nRows(); ++n) if (fabs(Splus.value(n,n)) > 1.0e-16) Splus.ref(n,n) = 1.0 / Splus.value(n,n);

	// Transpose U and Vt to get Ut and V, and multiply
	A = Vt.transposed() * Splus * U.transposed();

	return true;
}
