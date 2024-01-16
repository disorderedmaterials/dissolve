// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "math/svd.h"
#include "base/messenger.h"
#include "templates/array2D.h"

namespace SVD
{
static double sqrarg;
#define SQR(a) ((sqrarg = (a)) == 0.0 ? 0.0 : sqrarg * sqrarg)

// calculates sqrt( a^2 + b^2 ) with decent precision
double pythag(double a, double b)
{
    double absa, absb;

    absa = fabs(a);
    absb = fabs(b);

    if (absa > absb)
        return (absa * sqrt(1.0 + SQR(absb / absa)));
    else
        return (absb == 0.0 ? 0.0 : absb * sqrt(1.0 + SQR(absa / absb)));
}

// Perform single value decomposition of the supplied matrix A, putting left-orthogonal (U), diagonal single-value (S), and
// right-orthogonal (V transpose) matrices into the supplied Arrays
bool decompose(const Array2D<double> &A, Array2D<double> &U, Array2D<double> &S, Array2D<double> &Vt)
{
    int flag, i, its, j, jj, k, l, nm;
    double anorm, c, f, g, h, s, scale, x, y, z;

    // Get matrix dimensions and create / initialise arrays
    const auto nCols = A.nColumns();
    const auto nRows = A.nRows();

    // Check for nRows >= nCols
    if (nRows < nCols)
        return Messenger::error("DissolveMath::svd() - nRows must be greater than nCols.\n");
    std::vector<double> rv1(nCols, 0.0);
    S.initialise(nCols, nCols);
    S = 0.0;
    Vt.initialise(nCols, nCols);
    Vt = 0.0;
    U = A;

    g = 0.0;
    scale = 0.0;
    anorm = 0.0;
    for (i = 0; i < nCols; i++)
    {
        l = i + 1;
        rv1[i] = scale * g;
        g = 0.0;
        s = 0.0;
        scale = 0.0;
        if (i < nRows)
        {
            for (k = i; k < nRows; k++)
                scale += fabs(U[{k, i}]);
            if (scale != 0.0)
            {
                for (k = i; k < nRows; k++)
                {
                    U[{k, i}] /= scale;
                    s += U[{k, i}] * U[{k, i}];
                }

                f = U[{i, i}];
                g = -DissolveMath::sgn(sqrt(s), f);
                h = f * g - s;
                U[{i, i}] = f - g;

                for (j = l; j < nCols; j++)
                {
                    for (s = 0.0, k = i; k < nRows; k++)
                        s += U[{k, i}] * U[{k, j}];
                    f = s / h;
                    for (k = i; k < nRows; k++)
                        U[{k, j}] += f * U[{k, i}];
                }
                for (k = i; k < nRows; k++)
                    U[{k, i}] *= scale;
            }
        }
        S[{i, i}] = scale * g;
        g = 0.0;
        s = 0.0;
        scale = 0.0;
        if (i < nRows && i != nCols - 1)
        {
            for (k = l; k < nCols; k++)
                scale += fabs(U[{i, k}]);
            if (scale != 0.0)
            {
                for (k = l; k < nCols; k++)
                {
                    U[{i, k}] /= scale;
                    s += U[{i, k}] * U[{i, k}];
                }
                f = U[{i, l}];
                g = -DissolveMath::sgn(sqrt(s), f);
                h = f * g - s;
                U[{i, l}] = f - g;
                for (k = l; k < nCols; k++)
                    rv1[k] = U[{i, k}] / h;
                if (i != (nRows - 1))
                {
                    for (j = l; j < nRows; j++)
                    {
                        for (s = 0.0, k = l; k < nCols; k++)
                            s += U[{j, k}] * U[{i, k}];
                        for (k = l; k < nCols; k++)
                            U[{j, k}] += s * rv1[k];
                    }
                }
                for (k = l; k < nCols; k++)
                    U[{i, k}] *= scale;
            }
        }
        anorm = std::max(anorm, (fabs(S[{i, i}]) + fabs(rv1[i])));
    }

    // Accumulate right-hand transformation
    for (i = nCols - 1; i >= 0; i--)
    {
        if (i < nCols - 1)
        {
            if (g != 0.0)
            {
                for (j = l; j < nCols; j++)
                    Vt[{j, i}] = (U[{i, j}] / U[{i, l}]) / g;
                for (j = l; j < nCols; j++)
                {
                    for (s = 0.0, k = l; k < nCols; k++)
                        s += U[{i, k}] * Vt[{k, j}];
                    for (k = l; k < nCols; k++)
                        Vt[{k, j}] += s * Vt[{k, i}];
                }
            }

            for (j = l; j < nCols; j++)
            {
                Vt[{i, j}] = 0.0;
                Vt[{j, i}] = 0.0;
            }
        }
        Vt[{i, i}] = 1.0;
        g = rv1[i];
        l = i;
    }

    // Accumulate left-hand transformation
    //   for(i=min(nRows,nCols) - 1;i >= 0;i--) {
    for (i = nCols - 1; i >= 0; i--)
    {
        l = i + 1;
        g = S[{i, i}];
        if (i < (nCols - 1))
            for (j = l; j < nCols; j++)
                U[{i, j}] = 0.0;
        if (g != 0.0)
        {
            g = 1.0 / g;
            if (i != (nCols - 1)) // TGAY
            {
                for (j = l; j < nCols; j++)
                {
                    for (s = 0.0, k = l; k < nRows; k++)
                        s += U[{k, i}] * U[{k, j}];
                    f = (s / U[{i, i}]) * g;
                    for (k = i; k < nRows; k++)
                        U[{k, j}] += f * U[{k, i}];
                }
            }
            for (j = i; j < nRows; j++)
                U[{j, i}] *= g;
        }
        else
        {
            for (j = i; j < nRows; j++)
                U[{j, i}] = 0.0;
        }

        ++U[{i, i}];
    }

    // Diagonalise the bidiagonal form
    for (k = nCols - 1; k >= 0; k--)
    {
        for (its = 0; its < 30; its++)
        {
            flag = 1;
            for (l = k; l >= 0; l--)
            {
                nm = l - 1;
                if ((fabs(rv1[l]) + anorm) == anorm)
                {
                    flag = 0;
                    break;
                }
                if ((fabs(S[{nm, nm}]) + anorm) == anorm)
                    break;
            }
            if (flag)
            {
                c = 0.0;
                s = 1.0;
                for (i = l; i <= k; i++)
                {
                    f = s * rv1[i];

                    if ((fabs(f) + anorm) != anorm)
                    {
                        g = S[{i, i}];
                        h = pythag(f, g);
                        S[{i, i}] = h;
                        h = 1.0 / h;
                        c = g * h;
                        s = -f * h;
                        for (j = 0; j < nRows; j++)
                        {
                            y = U[{j, nm}];
                            z = U[{j, i}];
                            U[{j, nm}] = y * c + z * s;
                            U[{j, i}] = z * c - y * s;
                        }
                    }
                }
            }
            z = S[{k, k}];
            if (l == k)
            {
                if (z < 0.0)
                {
                    S[{k, k}] = -z;
                    for (j = 0; j < nCols; j++)
                        Vt[{j, k}] = -Vt[{j, k}];
                }
                break;
            }
            if (its == 30)
                return Messenger::error("DissolveMath::svd() - No convergence in 31 iterations.\n");

            // Shift from bottom 2x2 minor
            x = S[{l, l}];
            nm = k - 1;
            y = S[{nm, nm}];
            g = rv1[nm];
            h = rv1[k];
            f = ((y - z) * (y + z) + (g - h) * (g + h)) / (2.0 * h * y);
            g = pythag(f, 1.0);
            f = ((x - z) * (x + z) + h * ((y / (f + DissolveMath::sgn(g, f))) - h)) / x;
            c = 1.0;
            s = 1.0;
            for (j = l; j <= nm; j++)
            {
                i = j + 1;
                g = rv1[i];
                y = S[{i, i}];
                h = s * g;
                g = c * g;
                z = pythag(f, h);
                rv1[j] = z;
                c = f / z;
                s = h / z;
                f = x * c + g * s;
                g = g * c - x * s;
                h = y * s;
                y *= c;
                for (jj = 0; jj < nCols; jj++)
                {
                    x = Vt[{jj, j}];
                    z = Vt[{jj, i}];
                    Vt[{jj, j}] = x * c + z * s;
                    Vt[{jj, i}] = z * c - x * s;
                }
                z = pythag(f, h);
                S[{j, j}] = z;
                if (z)
                {
                    z = 1.0 / z;
                    c = f * z;
                    s = h * z;
                }
                f = c * g + s * y;
                x = c * y - s * g;
                for (jj = 0; jj < nRows; jj++)
                {
                    y = U[{jj, j}];
                    z = U[{jj, i}];
                    U[{jj, j}] = y * c + z * s;
                    U[{jj, i}] = z * c - y * s;
                }
            }
            rv1[l] = 0.0;
            rv1[k] = f;
            S[{k, k}] = x;
        }
    }

    // Transpose Vt
    Vt.transpose();

    return true;
}

// Compute in-place pseudoinverse of supplied matrix
bool pseudoinverse(Array2D<double> &A)
{
    // First, compute SVD of the matrix A
    Array2D<double> U, S, Vt;
    if (!decompose(A, U, S, Vt))
        return false;

    // Perform a quick sanity check on the decomposition
    Array2D<double> A2;
    A2 = U * S * Vt;
    for (auto n = 0; n < A.nRows(); ++n)
    {
        // TODO Need better double comparison here
        for (auto m = 0; m < A.nColumns(); ++m)
            if (fabs(A[{n, m}] - A2[{n, m}]) > 1.0e-9)
                return Messenger::error("DissolveMath::pseudoinverse() - SVD does not appear to be valid.\n");
    }
    // Take the diagonal single-value matrix S and form its pseudoinverse.
    // This amounts to taking each non-zero diagonal element and replacing it with its reciprocal
    Array2D<double> Splus = S;
    for (auto n = 0; n < S.nRows(); ++n)
        if (fabs(Splus[{n, n}]) > 1.0e-16)
            Splus[{n, n}] = 1.0 / Splus[{n, n}];

    // Transpose U and Vt to get Ut and V, and multiply
    A = Vt.transposed() * Splus * U.transposed();

    return true;
}

}; // namespace SVD
