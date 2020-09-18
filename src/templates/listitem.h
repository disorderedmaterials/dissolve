/*
    *** Linked List Item Class
    *** src/templates/listitem.h
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
