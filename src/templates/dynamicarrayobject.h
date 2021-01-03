// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

// Forward Declarations
template <class T> class DynamicArray;

// Dynamic Array Object Class
template <class T> class DynamicArrayObjectBase
{
    public:
    DynamicArrayObjectBase<T>() { arrayIndex_ = -1; }

    protected:
    // Index of object within parent DynamicArray
    int arrayIndex_;

    public:
    // Return index of object within parent DynamicArray
    int arrayIndex() const { return arrayIndex_; }
};

// Dynamic Array Object Class
template <class T> class DynamicArrayObject : public DynamicArrayObjectBase<T>
{
    public:
    // Clear object, ready for re-use
    virtual void clear() = 0;
    // Return index of object within parent DynamicArray
    void setArrayIndex(int index) { DynamicArrayObjectBase<T>::arrayIndex_ = index; }
};
