// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

// Forward Declarations
template <class T> class List;
template <class T> class ListIterator;

// ListItem Class
template <class T> class ListItem
{
    public:
    ListItem<T>()
    {
        prev_ = nullptr;
        next_ = nullptr;
    }

    private:
    // Pointer to previous item in list
    T *prev_;
    // Pointer to next item in list
    T *next_;

    public:
    // Return previous item in list
    T *prev() const { return prev_; }
    // Return next item in list
    T *next() const { return next_; }
    // Declare the list and iterator as friends
    friend class List<T>;
    friend class ListIterator<T>;
};
