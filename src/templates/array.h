// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include "templates/listitem.h"
#include "templates/vector3.h"
#include <cassert>
#include <new>

// Array
template <class A> class Array : public ListItem<Array<A>>
{
    public:
    Array(int initialSize = 0) : ListItem<Array<A>>()
    {
        chunkSize_ = 128;
        array_ = nullptr;
        size_ = 0;
        nItems_ = 0;
        if (initialSize > 0)
            initialise(initialSize);
    }
    Array(const Array<A> &source, int firstIndex, int lastIndex) : ListItem<Array<A>>()
    {
        array_ = nullptr;
        size_ = 0;
        nItems_ = 0;
        copy(source, firstIndex, lastIndex);
    }
    ~Array()
    {
        if (array_ != nullptr)
            delete[] array_;
    }
    Array(const Array<A> &source)
    {
        chunkSize_ = source.chunkSize_;
        array_ = nullptr;
        size_ = 0;
        nItems_ = 0;
        initialise(source.size_);
        nItems_ = source.nItems_;
        for (auto n = 0; n < nItems_; ++n)
            array_[n] = source.array_[n];
    }
    void operator=(const Array<A> &source)
    {
        chunkSize_ = source.chunkSize_;
        clear();
        resize(source.size_);
        nItems_ = source.nItems_;
        for (auto n = 0; n < nItems_; ++n)
            array_[n] = source.array_[n];
    }
    operator A *() { return array_; }

    /*
     * Array Data
     */
    private:
    // Current size of Array
    int size_;
    // Array data
    A *array_;
    // Number of data actually in Array
    int nItems_;
    // Chunk (increment) size for Array
    int chunkSize_;

    private:
    // Resize array
    void resize(int newSize)
    {
        // Array large enough already?
        if ((newSize - size_) <= 0)
            return;

        // Copy old data to temporary array
        A *oldItems = nullptr;
        if (nItems_ > 0)
        {
            oldItems = new A[nItems_];
            for (auto n = 0; n < nItems_; ++n)
                oldItems[n] = array_[n];
        }

        // Delete old, and create new array
        if (array_ != nullptr)
            delete[] array_;
        size_ = newSize;
        try
        {
            array_ = new A[size_];
        }
        catch (std::bad_alloc &alloc)
        {
            Messenger::error("Array<T>() - Failed to allocate sufficient memory for array_. Exception was : {}\n",
                             alloc.what());
            return;
        }

        // Copy old data into new array
        for (auto n = 0; n < nItems_; ++n)
            array_[n] = oldItems[n];
        delete[] oldItems;
    }

    public:
    // Return number of items in array
    int nItems() const { return nItems_; }
    // Return current maximum size of array
    int size() const { return size_; }
    // Return data array
    A *array() { return array_; }
    // Return const data array
    const A *constArray() const { return array_; }
    // Create array of specified size
    void initialise(int size)
    {
        // First, resize array...
        resize(size);

        // ...then set number of items to specified size...
        nItems_ = size;

        // ...and finally set all elements to default value
        for (auto n = 0; n < nItems_; ++n)
            array_[n] = A();
    }
    // Create empty array of specified size
    void createEmpty(int size)
    {
        clear();

        resize(size);

        nItems_ = 0;
    }
    // Set chunksize to use when incrementally resizing this array
    void setChunkSize(int chunkSize) { chunkSize_ = chunkSize; }
    // Copy data from source array
    void copy(const Array<A> &source, int firstIndex, int lastIndex)
    {
        clear();

        int nItemsToCopy = (lastIndex - firstIndex) + 1;
        if (nItemsToCopy > 0)
        {
            resize(nItemsToCopy);
            nItems_ = nItemsToCopy;
            for (auto n = 0; n < nItems_; ++n)
                array_[n] = source.array_[n + firstIndex];
        }
    }

    /*
     * Add / Remove
     */
    public:
    // Add new element to array
    void add(A data)
    {
        // Is current array large enough?
        if (nItems_ == size_)
            resize(size_ + chunkSize_);

        // Store new value
        array_[nItems_++] = data;
    }
    // Insert new element in array
    void insert(A data, const int position)
    {
        assert(position >= 0 && position < nItems_);

        // Is current array large enough?
        if (nItems_ == size_)
            resize(size_ + chunkSize_);

        // Working from the top of the array, shift all items after or at 'position' up one place
        for (auto n = nItems_; n > position; --n)
            array_[n] = array_[n - 1];

        array_[position] = data;
        ++nItems_;
    }
    // Forget data (set nItems to zero) leaving array intact
    void forgetData() { nItems_ = 0; }
    // Clear array
    void clear()
    {
        nItems_ = 0;
        if (array_ != nullptr)
            delete[] array_;
        array_ = nullptr;
        size_ = 0;
    }
    // Drop the first item from the array
    void removeFirst()
    {
        if (nItems_ == 0)
        {
            Messenger::warn("Tried to drop the first item of an empty array...\n");
            return;
        }

        for (auto n = 0; n < nItems_ - 1; ++n)
            array_[n] = array_[n + 1];

        --nItems_;
    }
    // Drop the last item from the array
    void removeLast()
    {
        if (nItems_ == 0)
        {
            Messenger::warn("Tried to drop the last item of an empty array...\n");
            return;
        }
        --nItems_;
    }
    // Remove the specified index
    void remove(const int position)
    {
        assert(position >= 0 && position < nItems_);

        for (auto n = position; n < nItems_ - 1; ++n)
            array_[n] = array_[n + 1];

        --nItems_;
    }

    /*
     * Set / Get
     */
    public:
    // Return reference to nth item in array
    A &operator[](int n)
    {
        assert(n >= 0 && n < nItems_);

        return array_[n];
    }
    // Return single value
    A &at(int n)
    {
        assert(n >= 0 && n < nItems_);

        return array_[n];
    }
    // Return nth item as const reference
    const A &at(int n) const
    {
        assert(n >= 0 && n < nItems_);

        return array_[n];
    }
    // Return first value in array
    A firstValue() const
    {
        assert(nItems_ > 0);

        return array_[0];
    }
    // Return last value in array
    A lastValue() const
    {
        assert(nItems_ > 0);

        return array_[nItems_ - 1];
    }
    // Return first item in array
    A &first()
    {
        assert(nItems_ > 0);

        return array_[0];
    }
    // Return last item in array
    A &last()
    {
        assert(nItems_ > 0);

        return array_[nItems_ - 1];
    }

    /*
     * Move
     */
    public:
    // Shift item up in the array (towards higher indices)
    void shiftUp(int position)
    {
        assert(position >= 0 && position < nItems_);

        // If this item is already last in the list, return now
        if (position == (nItems_ - 1))
            return;

        A temporary(array_[position + 1]);
        array_[position + 1] = array_[position];
        array_[position] = temporary;
    }
    // Shift item down in the array (towards lower indices)
    void shiftDown(int position)
    {
        assert(position >= 0 && position < nItems_);

        // If this item is already first in the list, return now
        if (position == 0)
            return;

        A temporary(array_[position - 1]);
        array_[position - 1] = array_[position];
        array_[position] = temporary;
    }

    /*
     * Operators
     */
    public:
    // Operator= (set all)
    void operator=(const A value)
    {
        for (auto n = 0; n < nItems_; ++n)
            array_[n] = value;
    }
    // Operator+= (add to all)
    void operator+=(const A value)
    {
        for (auto n = 0; n < nItems_; ++n)
            array_[n] += value;
    }
    void operator+=(const Array<A> array)
    {
        for (auto n = 0; n < nItems_; ++n)
            array_[n] += array.at(n);
    }
    // Operator-= (subtract from all)
    void operator-=(const A value)
    {
        for (auto n = 0; n < nItems_; ++n)
            array_[n] -= value;
    }
    // Operator*= (multiply all)
    void operator*=(const A value)
    {
        for (auto n = 0; n < nItems_; ++n)
            array_[n] *= value;
    }
    // Operator/= (divide all)
    void operator/=(const A value)
    {
        for (auto n = 0; n < nItems_; ++n)
            array_[n] /= value;
    }
    // Operator- (subtraction)
    Array<A> operator-(const A value)
    {
        Array<A> result = *this;
        result -= value;
        return result;
    }
    Array<A> operator-(const Array<A> array)
    {
        Array<A> result(nItems_);
        for (auto n = 0; n < nItems_; ++n)
            result[n] = array_[n] - array.at(n);
        return result;
    }
    // Operator+ (addition)
    Array<A> operator+(const A value)
    {
        Array<A> result = *this;
        result += value;
        return result;
    }
    Array<A> operator+(const Array<A> array)
    {
        Array<A> result(nItems_);
        for (auto n = 0; n < nItems_; ++n)
            result[n] = array_[n] + array.at(n);
        return result;
    }
    // Operator* (multiply all and return new)
    Array<A> operator*(const A value)
    {
        Array<A> result = *this;
        result *= value;
        return result;
    }
    // Operator/ (divide all and return new)
    Array<A> operator/(const A value)
    {
        Array<A> result = *this;
        result /= value;
        return result;
    }

    /*
     * Functions
     */
    public:
    // Return sum of elements in array
    A sum()
    {
        A result = 0.0;
        for (auto n = 0; n < nItems_; ++n)
            result += array_[n];
        return result;
    }
    // Return maximal absolute value in array
    A maxAbs() const
    {
        A result = 0.0;
        A temp;
        for (auto n = 0; n < nItems_; ++n)
        {
            temp = array_[n];
            if (temp < 0)
                temp = -temp;
            if (temp > result)
                result = temp;
        }
        return result;
    };
};
