// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

// Dynamic Array Object Class
template <class T> class DynamicArrayObject
{
    protected:
    // Object's index within the parent DynamicArray
    int arrayIndex_;

    public:
    DynamicArrayObject<T>() { arrayIndex_ = -1; }
    
    // Sets the index of the object within the parent DynamicArray
    void setArrayIndex(int index) { arrayIndex_ = index; }
    // Gets the index of the object within the parent DynamicArray
    int arrayIndex() const { return arrayIndex_; }

    // Clear object, ready for re-use
    virtual void clear() = 0;
};
