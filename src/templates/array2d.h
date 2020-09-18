/*
    *** Two-Dimensional Array
    *** src/templates/array2d.h
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

#pragma once

#include "base/messenger.h"
#include "templates/list.h"
#include "templates/vector3.h"

// Array2D
template <class A> class Array2D
{
    public:
    Array2D(int nrows = 0, int ncolumns = 0, bool half = false)
    {
        array_ = nullptr;
        linearSize_ = 0;
        rowOffsets_ = nullptr;
        nRows_ = 0;
        nColumns_ = 0;
        half_ = half;
        if ((nrows > 0) && (ncolumns > 0))
            resize(nrows, ncolumns);
    }
    ~Array2D() { clear(); }
    // Clear array data
    void clear()
    {
        if (array_ != nullptr)
            delete[] array_;
        if (rowOffsets_ != nullptr)
            delete[] rowOffsets_;
        rowOffsets_ = nullptr;
        array_ = nullptr;
        linearSize_ = 0;
        nRows_ = 0;
        nColumns_ = 0;
    }
    Array2D(const Array2D<A> &source)
    {
        array_ = nullptr;
        linearSize_ = 0;
        rowOffsets_ = nullptr;
        nRows_ = 0;
        nColumns_ = 0;
        half_ = false;
        (*this) = source;
    }
    void operator=(const A value)
    {
        // Copy source data elements
        for (int row = 0; row < nRows_; ++row)
        {
            if (half_)
                for (int column = row; column < nColumns_; ++column)
                    array_[rowOffsets_[row] + column - row] = value;
            else
                for (int column = 0; column < nColumns_; ++column)
                    array_[rowOffsets_[row] + column] = value;
        }
    }
    void operator=(const Array2D<A> &source)
    {
        // Clear any existing data and reinitialise the array
        clear();
        initialise(source.nRows_, source.nColumns_, source.half_);

        // Copy source data elements
        for (int row = 0; row < nRows_; ++row)
        {
            if (half_)
                for (int column = row; column < nColumns_; ++column)
                    array_[rowOffsets_[row] + column - row] = source.array_[rowOffsets_[row] + column - row];
            else
                for (int column = 0; column < nColumns_; ++column)
                    array_[rowOffsets_[row] + column] = source.array_[rowOffsets_[row] + column];
        }
    }

    /*
     * Data
     */
    private:
    // Linear array of objects
    A *array_;
    // Size of linear array
    int linearSize_;
    // Array dimensions
    int nRows_, nColumns_;
    // Half-matrix mode
    bool half_;
    // Row offsets
    int *rowOffsets_;

    private:
    // Resize array
    void resize(int nrows, int ncolumns)
    {
        // Clear old data
        clear();

        // If we're only interested in half the matrix then it must be square
        if (half_ && (nrows != ncolumns))
        {
            Messenger::print("BAD_USAGE - Requested half-matrix mode on a non-square matrix in Array2D::resize().\n");
        }

        // Create new array
        nRows_ = nrows;
        nColumns_ = ncolumns;
        rowOffsets_ = new int[nRows_];
        if (half_)
        {
            // Half-array, with element (i,j) == (j,i)
            int n;
            linearSize_ = 0;
            for (n = nRows_; n > 0; --n)
            {
                rowOffsets_[nRows_ - n] = linearSize_;
                linearSize_ += n;
            }
            array_ = new A[linearSize_];
        }
        else
        {
            linearSize_ = nRows_ * nColumns_;
            array_ = new A[linearSize_];
            for (int n = 0; n < nRows_; ++n)
                rowOffsets_[n] = n * nColumns_;
        }
    }

    public:
    // Initialise array
    void initialise(int nrows, int ncolumns, bool half = false)
    {
        clear();

        half_ = half;
        if ((nrows > 0) && (ncolumns > 0))
            resize(nrows, ncolumns);
    }
    // Add empty row to array
    void addRow(int nCols = -1)
    {
        // Copy current Array
        Array2D<A> oldArray = *this;

        // Check current column count
        if (nColumns_ == 0)
        {
            // Must have been supplied the column size if we currently have no data
            if (nCols == -1)
            {
                Messenger::error("Array2D<A>::addRow() - Array is currently empty, so column size must be provided.\n");
                return;
            }
        }
        else
            nCols = nColumns_;

        // Reinitialise the present matrix to the new size
        if (half_ && (nRows_ == nCols))
        {
            Messenger::warn("Adding a row to this Array2D<A> will force it to be rectangular, so it will no longer "
                            "be halved.\n");
            initialise(nRows_ + 1, nCols, false);
        }
        else
            initialise(nRows_ + 1, nCols, half_);

        // Copy old data back in
        for (int n = 0; n < oldArray.nRows_; ++n)
        {
            for (int m = 0; m < oldArray.nColumns_; ++m)
                at(n, m) = oldArray.at(n, m);
        }
    }
    // Set row
    void setRow(int row, A value)
    {
        for (int n = 0; n < nColumns_; ++n)
            at(row, n) = value;
    }
    // Return specified element as reference
    A &at(int row, int column)
    {
#ifdef CHECKS
        static A dummy;
        if ((row < 0) || (row >= nRows_))
        {
            Messenger::print("OUT_OF_RANGE - Row number ({}) is out of range in Array2D::at() (nRows = {}).\n", row, nRows_);
            return dummy;
        }
        if ((column < 0) || (column >= nColumns_))
        {
            Messenger::print("OUT_OF_RANGE - Column number ({}) is out of range in Array2D::at() (nColumns = {}).\n", column,
                             nColumns_);
            return dummy;
        }
#endif
        if (half_)
        {
            if (row > column)
                return array_[rowOffsets_[column] + row - column];
            else
                return array_[rowOffsets_[row] + column - row];
        }
        else
            return array_[rowOffsets_[row] + column];
    }
    // Return specified element as const reference
    A &constAt(int row, int column) const
    {
#ifdef CHECKS
        static A dummy;
        if ((row < 0) || (row >= nRows_))
        {
            Messenger::print("OUT_OF_RANGE - Row number ({}) is out of range in Array2D::constAt() (nRows = {}).\n", row,
                             nRows_);
            return dummy;
        }
        if ((column < 0) || (column >= nColumns_))
        {
            Messenger::print("OUT_OF_RANGE - Column number ({}) is out of range in Array2D::constAt() (nColumns = {}).\n",
                             column, nColumns_);
            return dummy;
        }
#endif
        if (half_)
        {
            if (row > column)
                return array_[rowOffsets_[column] + row - column];
            else
                return array_[rowOffsets_[row] + column - row];
        }
        else
            return array_[rowOffsets_[row] + column];
    }
    // Return address of specified element
    A *pointerAt(int row, int column)
    {
#ifdef CHECKS
        static A dummy;
        if ((row < 0) || (row >= nRows_))
        {
            Messenger::print("OUT_OF_RANGE - Row number ({}) is out of range in Array2D::pointerAt() (nRows = {}).\n", row,
                             nRows_);
            return &dummy;
        }
        if ((column < 0) || (column >= nColumns_))
        {
            Messenger::print("OUT_OF_RANGE - Column number ({}) is out of range in Array2D::pointerAt() (nColumns = {}).\n",
                             column, nColumns_);
            return &dummy;
        }
#endif
        if (half_)
        {
            if (row > column)
                return &array_[rowOffsets_[column] + row - column];
            else
                return &array_[rowOffsets_[row] + column - row];
        }
        else
            return &array_[rowOffsets_[row] + column];
    }
    // Return whether the array is halved
    bool halved() const { return half_; }
    // Return number of rows
    int nRows() const { return nRows_; }
    // Return number of columns
    int nColumns() const { return nColumns_; }
    // Return linear array size
    int linearArraySize() const { return linearSize_; }
    //
    // Return linear array
    A *linearArray() { return array_; }

    // Return linear array (const)
    A *constLinearArray() const { return array_; }
    // Return linear value
    A &linearValue(int index)
    {
#ifdef CHECKS
        static A dummy;
        if ((index < 0) || (index >= linearSize_))
        {
            Messenger::print("OUT_OF_RANGE - Index ({}) is out of range in Array2D::linearValue() (linearSize = {}).\n", index,
                             linearSize_);
            return dummy;
        }
#endif
        return array_[index];
    }
    // Return linear value (const)
    A &constLinearValue(int index) const
    {
#ifdef CHECKS
        static A dummy;
        if ((index < 0) || (index >= linearSize_))
        {
            Messenger::print("OUT_OF_RANGE - Index ({}) is out of range in Array2D::constLinearValue() (linearSize = {}).\n",
                             index, linearSize_);
            return dummy;
        }
#endif
        return array_[index];
    }

    /*
     * Operators
     */
    public:
    // Operator+= (add to all)
    void operator+=(const A value)
    {
        for (int n = 0; n < linearSize_; ++n)
            array_[n] += value;
    }
    // Operator-= (subtract from all)
    void operator-=(const A value)
    {
        for (int n = 0; n < linearSize_; ++n)
            array_[n] -= value;
    }
    // Operator*= (multiply all)
    void operator*=(const A value)
    {
        for (int n = 0; n < linearSize_; ++n)
            array_[n] *= value;
    }
    // Operator/= (divide all)
    void operator/=(const A value)
    {
        for (int n = 0; n < linearSize_; ++n)
            array_[n] /= value;
    }
    // Operator+= (matrix addition)
    void operator+=(const Array2D<A> &B) const
    {
        // Check array sizes are compatible
        if (nColumns_ != B.nRows_)
        {
            Messenger::error("Can't add matrices together, as they have incompatible sizes ({}x{} and {}x{}, RxC)\n", nRows_,
                             nColumns_, B.nRows_, B.nColumns_);
            return;
        }
        for (int n = 0; n < linearSize_; ++n)
            array_[n] += B.constLinearValue(n);
    }
    // Operator-= (matrix subtraction)
    void operator-=(const Array2D<A> &B) const
    {
        // Check array sizes are compatible
        if (nColumns_ != B.nRows_)
        {
            Messenger::error("Can't subtract matrices, as they have incompatible sizes ({}x{} and {}x{}, RxC)\n", nRows_,
                             nColumns_, B.nRows_, B.nColumns_);
            return;
        }
        for (int n = 0; n < linearSize_; ++n)
            array_[n] -= B.constLinearValue(n);
    }
    // Operator* (matrix multiply)
    Array2D<A> operator*(const Array2D<A> &B) const
    {
        // Check array sizes are compatible
        if (nColumns_ != B.nRows_)
        {
            Messenger::error("Can't multiply matrices together, as they have incompatible sizes ({}x{} and {}x{}, RxC)\n",
                             nRows_, nColumns_, B.nRows_, B.nColumns_);
            return Array2D<A>();
        }

        Array2D<A> C(nRows_, B.nColumns_);
        int colB, i;
        double x;
        for (int rowA = 0; rowA < nRows_; ++rowA)
        {
            for (colB = 0; colB < B.nColumns_; ++colB)
            {
                // Calculate dot product of rowA (in matrix A (this)) and columnB in matrix B
                // The number of elements equals nColumns in A (== nRows in B)
                x = 0.0;
                for (i = 0; i < nColumns_; ++i)
                    x += constAt(rowA, i) * B.constAt(i, colB);
                C.at(rowA, colB) = x;
            }
        }

        return C;
    }

    /*
     * Functions
     */
    public:
    // Print matrix
    void print(std::string_view title = "Array2D<A>") const
    {
        Messenger::print("'{}' : {} rows x {} columns:\n", title, nRows_, nColumns_);
        std::string line;
        for (int row = 0; row < nRows_; ++row)
        {
            line += fmt::format("R{:2i} :", row);
            for (int column = 0; column < nColumns_; ++column)
                line += fmt::format(" {:e}", constAt(row, column));
            Messenger::print("{}\n", line);
        }
    }
    // Transpose (in-place) the current array
    void transpose() { *this = transposed(); }
    // Return transpose of the current array
    Array2D<A> transposed() const
    {
        Array2D<A> result(nColumns_, nRows_);
        for (int r = 0; r < nRows_; ++r)
        {
            for (int c = 0; c < nColumns_; ++c)
                result.at(c, r) = constAt(r, c);
        }
        return result;
    }
};
