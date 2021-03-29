// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "templates/list.h"
#include "templates/vector3.h"
#include <numeric>
#include <vector>

// Array3D
template <class A> class Array3D
{
    public:
    Array3D(int nX = 0, int nY = 0, int nZ = 0) : nX_(nX), nY_(nY), nZ_(nZ)
    {
        if ((nX > 0) && (nY > 0) && (nZ > 0))
            resize(nX, nY, nZ);
    }
    ~Array3D() { clear(); }
    // Clear array data
    void clear()
    {
        array_.clear();
        sliceOffsets_.clear();
        nX_ = 0;
        nY_ = 0;
        nZ_ = 0;
    }
    Array3D(const Array3D<A> &source) { (*this) = source; }
    void operator=(const A value)
    {
        // Copy source data elements
        std::fill(array_.begin(), array_.end(), value);
    }
    void operator=(const Array3D<A> &source)
    {
        // Clear any existing data and reinitialise the array
        clear();
        initialise(source.nX_, source.nY_, source.nZ_);

        std::copy(source.array_.begin(), source.array_.end(), array_.begin());
    }

    /*
     * Data
     */
    private:
    // Linear array of objects
    std::vector<A> array_;
    // Array dimensions
    int nX_, nY_, nZ_;
    // XY slice offsets
    std::vector<int> sliceOffsets_;

    private:
    // Resize array
    void resize(int nX, int nY, int nZ)
    {
        // Clear old data
        clear();

        // Create new array
        nX_ = nX;
        nY_ = nY;
        nZ_ = nZ;
        array_.resize(nX_ * nY_ * nZ_);

        // Create slice offsets array
        sliceOffsets_.resize(nZ_);
        std::vector<int> indices(nZ_, 0);
        std::iota(indices.begin(), indices.end(), 0);
        std::transform(indices.begin(), indices.end(), sliceOffsets_.begin(),
                       [this](const int index) { return index * nX_ * nY_; });
    }

    public:
    // Initialise array
    void initialise(int nX, int nY, int nZ)
    {
        clear();

        if ((nX > 0) && (nY > 0) && (nZ > 0))
            resize(nX, nY, nZ);
    }
    // Return specified element as modifiable reference
    A &operator[](std::tuple<int, int, int> index)
    {
        auto [x, y, z] = index;
        assert(x >= 0 && x < nX_);
        assert(y >= 0 && y < nY_);
        assert(z >= 0 && z < nZ_);

        return array_[sliceOffsets_[z] + y * nX_ + x];
    }
    // Return specified element as const-reference
    const A &operator[](std::tuple<int, int, int> index) const
    {
        auto [x, y, z] = index;
        assert(x >= 0 && x < nX_);
        assert(y >= 0 && y < nY_);
        assert(z >= 0 && z < nZ_);

        return array_[sliceOffsets_[z] + y * nX_ + x];
    }
    // Return address of specified element
    A *ptr(int x, int y, int z)
    {
        assert(x >= 0 && x < nX_);
        assert(y >= 0 && y < nY_);
        assert(z >= 0 && z < nZ_);

        return &array_[sliceOffsets_[z] + y * nX_ + x];
    }
    // Return array size in x
    int nX() const { return nX_; }
    // Return array size in yu
    int nY() const { return nY_; }
    // Return array size in z
    int nZ() const { return nZ_; }
    // Return linear array size
    int linearArraySize() const { return array_.size(); }
    // Return linear array
    std::vector<A> &linearArray() { return array_; }
    // Return linear array
    const std::vector<A> &linearArray() const { return array_; }
    // Beginning of the underlying array
    typename std::vector<A>::iterator begin() { return array_.begin(); }
    // End of the underlying array
    typename std::vector<A>::iterator end() { return array_.end(); }
    // Beginning of the underlying array
    typename std::vector<A>::const_iterator begin() const { return array_.cbegin(); }
    // End of the underlying array
    typename std::vector<A>::const_iterator end() const { return array_.cend(); }
    bool empty() const { return array_.empty(); }
    // Return linear value
    A &linearValue(int index) { return array_[index]; }
    const A &linearValue(int index) const { return array_[index]; }

    /*
     * Operators
     */
    public:
    // Operator+= (add to all)
    void operator+=(const A value)
    {
        std::transform(array_.begin(), array_.end(), array_.begin(), [value](auto n) { return n + value; });
    }
    // Operator-= (subtract from all)
    void operator-=(const A value)
    {
        std::transform(array_.begin(), array_.end(), array_.begin(), [value](auto n) { return n - value; });
    }
    // Operator*= (multiply all)
    void operator*=(const A value)
    {
        std::transform(array_.begin(), array_.end(), array_.begin(), [value](auto n) { return n * value; });
    }
    // Operator/= (divide all)
    void operator/=(const A value)
    {
        std::transform(array_.begin(), array_.end(), array_.begin(), [value](auto n) { return n / value; });
    }
};

// OffsetArray3D
template <class A> class OffsetArray3D
{
    public:
    OffsetArray3D(int xMin, int xMax, int yMin, int yMax, int zMin, int zMax)
    {
        initialise(xMin, xMax, yMin, yMax, zMin, zMax);
    }
    ~OffsetArray3D() { clear(); }
    // Clear array data
    void clear()
    {
        array_.clear();
        sliceOffsets_.clear();
        xMin_ = 0;
        yMin_ = 0;
        zMin_ = 0;
        xMax_ = 0;
        yMax_ = 0;
        zMax_ = 0;
        nX_ = 0;
        nY_ = 0;
        nZ_ = 0;
    }
    OffsetArray3D(const OffsetArray3D<A> &source)
    {
        array_.clear();
        sliceOffsets_.clear();
        xMin_ = 0;
        yMin_ = 0;
        zMin_ = 0;
        xMax_ = 0;
        yMax_ = 0;
        zMax_ = 0;
        nX_ = 0;
        nY_ = 0;
        nZ_ = 0;
        (*this) = source;
    }
    void operator=(const A value)
    {
        // Copy source data elements
        std::fill(array_.begin(), array_.end(), value);
    }
    void operator=(const Array3D<A> &source)
    {
        // Clear any existing data and reinitialise the array
        clear();
        initialise(source.nX_, source.nY_, source.nZ_);

        std::copy(source.array_.begin(), source.array_.end(), array_.begin());
    }

    /*
     * Data
     */
    private:
    // Linear array of objects
    std::vector<A> array_;
    // Array limits
    int xMin_, xMax_, yMin_, yMax_, zMin_, zMax_;
    // Array dimensions
    int nX_, nY_, nZ_;
    // XY slice offsets
    std::vector<int> sliceOffsets_;

    public:
    // Initialise array
    void initialise(int xMin, int xMax, int yMin, int yMax, int zMin, int zMax)
    {
        clear();

        xMin_ = xMin;
        yMin_ = yMin;
        zMin_ = zMin;
        xMax_ = xMax;
        yMax_ = yMax;
        zMax_ = zMax;
        nX_ = (xMax_ - xMin_) + 1;
        nY_ = (yMax_ - yMin_) + 1;
        nZ_ = (zMax_ - zMin_) + 1;

        if ((nX_ > 0) && (nY_ > 0) && (nZ_ > 0))
        {
            array_.resize(nX_ * nY_ * nZ_);
            sliceOffsets_.resize(nZ_);

            for (auto n = 0; n < nZ_; ++n)
                sliceOffsets_[n] = n * nX_ * nY_;
        }
    }
    // Return specified element as reference
    A &operator[](std::tuple<int, int, int> index)
    {
        auto [x, y, z] = index;
        assert(x >= xMin_ && x <= xMax_);
        assert(y >= yMin_ && y <= yMax_);
        assert(z >= zMin_ && z <= zMax_);

        return array_[sliceOffsets_[z - zMin_] + (y - yMin_) * nX_ + (x - xMin_)];
    }
    // Return specified element as const reference
    const A &operator[](std::tuple<int, int, int> index) const
    {
        auto [x, y, z] = index;
        assert(x >= xMin_ && x <= xMax_);
        assert(y >= yMin_ && y <= yMax_);
        assert(z >= zMin_ && z <= zMax_);

        return array_[sliceOffsets_[z - zMin_] + (y - yMin_) * nX_ + (x - xMin_)];
    }
    // Return address of specified element
    A *pointerAt(int x, int y, int z)
    {
        assert(x >= xMin_ && x <= xMax_);
        assert(y >= yMin_ && y <= yMax_);
        assert(z >= zMin_ && z <= zMax_);

        return &array_[sliceOffsets_[z - zMin_] + (y - yMin_) * nX_ + (x - xMin_)];
    }
    // Return array size in x
    int nX() const { return nX_; }
    // Return array size in yu
    int nY() const { return nY_; }
    // Return array size in z
    int nZ() const { return nZ_; }
    // Return linear array size
    int linearArraySize() const { return array_.size(); }
    // Return linear array
    std::vector<A> linearArray() { return array_; }
    // Beginning iterator
    typename std::vector<A>::iterator begin() { return array_.begin(); }
    // End iterator
    typename std::vector<A>::iterator end() { return array_.end(); }
    // Return linear value
    A &linearValue(int index) { return array_[index]; }
    const A &linearValue(int index) const { return array_[index]; }

    /*
     * Operators
     */
    public:
    // Operator+= (add to all)
    void operator+=(const A value)
    {
        std::transform(array_.begin(), array_.end(), array_.begin(), [value](auto n) { return n + value; });
    }
    // Operator-= (subtract from all)
    void operator-=(const A value)
    {
        std::transform(array_.begin(), array_.end(), array_.begin(), [value](auto n) { return n - value; });
    }
    // Operator*= (multiply all)
    void operator*=(const A value)
    {
        std::transform(array_.begin(), array_.end(), array_.begin(), [value](auto n) { return n * value; });
    }
    // Operator/= (divide all)
    void operator/=(const A value)
    {
        std::transform(array_.begin(), array_.end(), array_.begin(), [value](auto n) { return n / value; });
    }
};
