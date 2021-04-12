// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "keywords/base.h"
#include "templates/array.h"

// Forward Declarations
/* none */

// Keyword Interface
template <class T> class KeywordData : public KeywordBase
{
    public:
    KeywordData(KeywordBase::KeywordDataType type, T data) : KeywordBase(type), data_(data) {}

    /*
     * Data
     */
    protected:
    // Data (POD or otherwise) that is to be set by reading from an input file
    T data_;

    protected:
    // Determine whether current data is 'empty', and should be considered as 'not set'
    bool isDataEmpty() const override
    {
        // Override this function to handle cases where, for instance, checks for empty lists need to be made.
        return false;
    }

    public:
    // Set data, validating as necessary
    bool setData(T value)
    {
        if (isValid(value))
        {
            // Data is valid, so store it
            data_ = value;

            // Check here if the data is 'empty', in which case it is not strictly 'set'
            set_ = isDataEmpty() ? false : true;

            return true;
        }

        return false;
    }
    // Return data
    T &data() { return data_; }
    const T &data() const { return data_; }

    /*
     * Data Validation
     */
    public:
    // Validate supplied value
    virtual bool isValid(T value) { return true; }
};
