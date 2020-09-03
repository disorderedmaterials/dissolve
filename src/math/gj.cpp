/*
    *** Gauss-Jordan Inversion
    *** src/math/gj.cpp
    Copyright T. Youngs 2012-2020

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

#include "math/gj.h"
#include "templates/array2d.h"

// Perform Gauss-Jordan inversion of the supplied Array2D<double>
bool GaussJordan::invert(Array2D<double> &A)
{
    // Matrix must be square, and not stored as a half-matrix
    if (A.nRows() != A.nColumns())
    {
        Messenger::error("Can't invert this Array2D since it is not square (it is {}x{}).\n", A.nRows(), A.nColumns());
        return false;
    }
    if (A.halved())
    {
        Messenger::error("Can't invert this matrix since it is stored as a half-matrix.\n");
        return false;
    }

    const auto rank = A.nRows();
    double *array = A.linearArray();

    int pivotrows[rank], pivotcols[rank], pivotrow = 0, pivotcol = 0;
    bool pivoted[rank];
    int row, col, n, m;
    double large, element;
    for (n = 0; n < rank; ++n)
    {
        pivotrows[n] = 0;
        pivotcols[n] = 0;
        pivoted[n] = false;
    }
    // Loop over columns to be reduced
    for (n = 0; n < rank; ++n)
    {
        // Locate suitable pivot element - find largest value in the matrix A
        large = 0.0;
        for (row = 0; row < rank; ++row)
        {
            // Only search this row if it has not previously contained a pivot element
            if (pivoted[row])
                continue;
            for (col = 0; col < rank; ++col)
            {
                // Similarly, only look at the column element if the column hasn't been pivoted yet.
                if (pivoted[col])
                    continue;
                // Check the size of the element...
                element = fabs(array[row * rank + col]);
                if (element > large)
                {
                    large = element;
                    pivotrow = row;
                    pivotcol = col;
                }
            }
        }

        // Mark the pivot row/column as changed
        pivoted[pivotcol] = true;
        pivotrows[n] = pivotrow;
        pivotcols[n] = pivotcol;

        // Exchange rows to put pivot element on the diagonal
        if (pivotrow != pivotcol)
        {
            for (m = 0; m < rank; ++m)
            {
                element = array[pivotrow * rank + m];
                array[pivotrow * rank + m] = array[pivotcol * rank + m];
                array[pivotcol * rank + m] = element;
            }
        }

        // Now ready to divide through row elements.
        element = 1.0 / array[pivotcol * rank + pivotcol];
        array[pivotcol * rank + pivotcol] = 1.0;
        for (m = 0; m < rank; ++m)
            array[pivotcol * rank + m] *= element;

        // Divide through other rows by the relevant multiple of the pivot row
        for (row = 0; row < rank; ++row)
        {
            if (row == pivotcol)
                continue;
            element = array[row * rank + pivotcol];
            array[row * rank + pivotcol] = 0.0;
            for (m = 0; m < rank; ++m)
                array[row * rank + m] = array[row * rank + m] - array[pivotcol * rank + m] * element;
        }
    }

    // Rearrange columns to undo row exchanges performed earlier
    for (n = rank - 1; n >= 0; --n)
    {
        if (pivotrows[n] != pivotcols[n])
            for (m = 0; m < rank; ++m)
            {
                element = array[m * rank + pivotrows[n]];
                array[m * rank + pivotrows[n]] = array[m * rank + pivotcols[n]];
                array[m * rank + pivotcols[n]] = element;
            }
    }

    return true;
}
