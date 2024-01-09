// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "templates/vector3.h"
#include <cassert>
#include <optional>
#include <vector>

// Array2D
template <class A> class Array2D
{
    public:
    Array2D(int nrows = 0, int ncolumns = 0, bool half = false) : half_(half)
    {
        if ((nrows > 0) && (ncolumns > 0))
            resize(nrows, ncolumns);
    }
    ~Array2D() { clear(); }
    // Clear array data
    void clear()
    {
        array_.clear();
        rowOffsets_.clear();
        nRows_ = 0;
        nColumns_ = 0;
    }
    Array2D(const Array2D<A> &source) { (*this) = source; }
    void operator=(const A value) { std::fill(array_.begin(), array_.end(), value); }
    void operator=(const Array2D<A> &source)
    {
        // Clear any existing data and reinitialise the array
        clear();
        initialise(source.nRows_, source.nColumns_, source.half_);
        std::copy(source.array_.begin(), source.array_.end(), array_.begin());
    }

    /*
     * Data
     */
    private:
    // Linear array of objects
    std::vector<A> array_;
    // Array dimensions
    int nRows_{0}, nColumns_{0};
    // Half-matrix mode
    bool half_;
    // Row offsets
    std::vector<int> rowOffsets_;

    private:
    // Resize array
    void resize(int nrows, int ncolumns)
    {
        // Clear old data
        clear();

        // If we're only interested in half the matrix then it must be square
        if (half_)
            assert(nrows == ncolumns);

        // Create new array
        nRows_ = nrows;
        nColumns_ = ncolumns;
        rowOffsets_.resize(nRows_);
        if (half_)
        {
            // Half-array, with element (i,j) == (j,i)
            int linearSize = 0;
            for (auto n = nRows_; n > 0; --n)
            {
                rowOffsets_[nRows_ - n] = linearSize;
                linearSize += n;
            }
            array_.resize(linearSize);
        }
        else
        {
            array_.resize(nRows_ * nColumns_);
            for (auto n = 0; n < nRows_; ++n)
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
    void addRow(std::optional<unsigned int> nCols = std::nullopt)
    {
        // Copy current Array
        Array2D<A> oldArray = *this;

        // Check current column count
        if (nColumns_ == 0)
        {
            // Must have been supplied the column size if we currently have no data
            if (!nCols)
            {
                Messenger::error("Array2D<A>::addRow() - Array is currently empty, so column size must be provided.\n");
                return;
            }
        }
        else
            nCols = nColumns_;

        // Reinitialise the present matrix to the new size
        if (half_ && (nRows_ == nCols.value()))
        {
            Messenger::warn("Adding a row to this Array2D<A> will force it to be rectangular, so it will no longer "
                            "be halved.\n");
            initialise(nRows_ + 1, nCols.value(), false);
        }
        else
            initialise(nRows_ + 1, nCols.value(), half_);

        // Copy old data back in
        for (auto n = 0; n < oldArray.nRows_; ++n)
        {
            for (auto m = 0; m < oldArray.nColumns_; ++m)
                (*this)[{n, m}] = oldArray[{n, m}];
        }
    }
    // Set elements in specified row to same value
    void setRow(int row, A value)
    {
        for (auto n = 0; n < nColumns_; ++n)
            (*this)[{row, n}] = value;
    }
    // Set row
    void setRow(int row, std::vector<A> values)
    {
        assert(values.size() == nColumns_);
        for (auto n = 0; n < nColumns_; ++n)
            (*this)[{row, n}] = values[n];
    }
    // Multiply elements in specified row
    void multiplyRow(int row, A multiplier)
    {
        for (auto n = 0; n < nColumns_; ++n)
            (*this)[{row, n}] *= multiplier;
    }
    // Return specified element as reference
    A &operator[](const std::tuple<int, int> index)
    {
        auto [row, column] = index;
        assert(row >= 0 && row < nRows_);
        assert(column >= 0 && column < nColumns_);

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
    const A &operator[](const std::tuple<int, int> index) const
    {
        auto [row, column] = index;
        assert(row >= 0 && row < nRows_);
        assert(column >= 0 && column < nColumns_);

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
        assert(row >= 0 && row < nRows_);
        assert(column >= 0 && column < nColumns_);

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
    typename std::vector<A>::iterator begin() { return array_.begin(); }
    typename std::vector<A>::iterator end() { return array_.end(); }
    typename std::vector<A>::const_iterator begin() const { return array_.begin(); }
    typename std::vector<A>::const_iterator end() const { return array_.end(); }
    // Return linear array size
    int size() const { return array_.size(); }
    // Does the array have contents?
    bool empty() const { return array_.empty(); }
    std::vector<A> &linearArray() { return array_; }
    const std::vector<A> &linearArray() const { return array_; }

    /*
     * Operators
     */
    public:
    // Operator+= (add to all)
    void operator+=(const A value)
    {
        std::transform(array_.begin(), array_.end(), array_.begin(), [&value](auto &n) { return n + value; });
    }
    // Operator-= (subtract from all)
    void operator-=(const A value)
    {
        std::transform(array_.begin(), array_.end(), array_.begin(), [&value](auto &n) { return n - value; });
    }
    // Operator*= (multiply all)
    void operator*=(const A value)
    {
        std::transform(array_.begin(), array_.end(), array_.begin(), [&value](auto &n) { return n * value; });
    }
    // Operator/= (divide all)
    void operator/=(const A value)
    {
        std::transform(array_.begin(), array_.end(), array_.begin(), [&value](auto &n) { return n / value; });
    }
    // Operator+= (matrix addition)
    void operator+=(const Array2D<A> &B)
    {
        assert(nColumns_ == B.nColumns_ && nRows_ == B.nRows_);

        std::transform(array_.begin(), array_.end(), B.array_.begin(), array_.begin(), [](auto &a, auto &b) { return a + b; });
    }
    // Operator-= (matrix subtraction)
    void operator-=(const Array2D<A> &B)
    {
        assert(nColumns_ == B.nColumns_ && nRows_ == B.nRows_);

        std::transform(array_.begin(), array_.end(), B.array_begin(), array_.begin(), [](auto &a, auto &b) { return a - b; });
    }
    Array2D<A> operator+(const Array2D<A> &other) const
    {
        assert(nColumns_ == other.nColumns_ && nRows_ == other.nRows_);
        auto half = this->half_ && other.half_;
        Array2D<A> ret(nRows_, nColumns_, half);
        for (int i = 0; i < this->nRows_; i++)
        {
            int colStart = i ? half : 0;
            for (int j = colStart; j < this->nColumns_; ++j)
                ret[{i, j}] = other[{i, j}] + (*this)[{i, j}];
        };
        return ret;
    }
    // Operator* (matrix multiply)
    Array2D<A> operator*(const Array2D<A> &B) const
    {
        assert(nColumns_ == B.nRows_);

        Array2D<A> C(nRows_, B.nColumns_);
        int colB, i;
        double x;
        for (auto rowA = 0; rowA < nRows_; ++rowA)
        {
            for (colB = 0; colB < B.nColumns_; ++colB)
            {
                // Calculate dot product of rowA (in matrix A (this)) and columnB in matrix B
                // The number of elements equals nColumns in A (== nRows in B)
                x = 0.0;
                for (i = 0; i < nColumns_; ++i)
                    x += (*this)[{rowA, i}] * B[{i, colB}];
                C[{rowA, colB}] = x;
            }
        }

        return C;
    }
    A &operator[](const int index) { return array_[index]; }
    const A &operator[](const int index) const { return array_[index]; }

    /*
     * Functions
     */
    public:
    // Print matrix
    void print(std::string_view title = "Array2D<A>") const
    {
        Messenger::print("'{}' : {} rows x {} columns:\n", title, nRows_, nColumns_);
        for (auto row = 0; row < nRows_; ++row)
        {
            std::string line;
            for (auto column = 0; column < nColumns_; ++column)
                line += fmt::format(" {:e}", (*this)[{row, column}]);
            Messenger::print("R{:2d} :{}\n", row, line);
        }
    }
    // Transpose (in-place) the current array
    void transpose() { *this = transposed(); }
    // Return transpose of the current array
    Array2D<A> transposed() const
    {
        Array2D<A> result(nColumns_, nRows_);
        for (auto r = 0; r < nRows_; ++r)
        {
            for (auto c = 0; c < nColumns_; ++c)
                result[{c, r}] = (*this)[{r, c}];
        }
        return result;
    }
};
