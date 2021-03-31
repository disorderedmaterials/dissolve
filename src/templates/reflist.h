// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/messenger.h"
#include <cassert>
#include <iterator>
#include <stddef.h>

// Forward Declarations
template <class T> class RefList;

/*
 * Item
 */

template <class T> class RefListItem : public std::iterator<std::forward_iterator_tag, T *>
{
    public:
    RefListItem<T>()
    {
        item_ = nullptr;
        next_ = nullptr;
        prev_ = nullptr;
    }

    /*
     * Data
     */
    private:
    // Pointer to item
    T *item_;

    public:
    // Return item
    T *item() { return item_; }
    RefListItem<T> operator++()
    {
        if (next_ == nullptr)
        {
            prev_ = nullptr;
            item_ = nullptr;
            next_ = nullptr;
        }
        else
        {
            prev_ = next_->prev_;
            item_ = next_->item_;
            next_ = next_->next_;
        }
        return *this;
    }
    bool operator==(RefListItem<T> that) { return next_ == that.next_ && prev_ == that.prev_ && item_ == that.item_; }
    bool operator!=(RefListItem<T> that) { return next_ != that.next_ || prev_ != that.prev_ || item_ != that.item_; }
    T *&operator*() { return item_; }

    /*
     * List Pointers
     */
    private:
    // List pointers
    RefListItem<T> *prev_, *next_;

    public:
    // Return item after this one
    RefListItem<T> *next() const { return next_; }
    // Declare the list and iterator as friends
    friend class RefList<T>;
};

/*
 * Reference List
 */

template <class T> class RefList
{
    public:
    RefList<T>()
    {
        listHead_ = nullptr;
        listTail_ = nullptr;
        items_ = nullptr;
        regenerate_ = true;
        nItems_ = 0;
    }
    RefList<T>(T *singleItem)
    {
        listHead_ = nullptr;
        listTail_ = nullptr;
        items_ = nullptr;
        regenerate_ = true;
        nItems_ = 0;

        append(singleItem);
    }

    ~RefList() { clear(); }
    RefList<T>(const RefList<T> &source)
    {
        listHead_ = nullptr;
        listTail_ = nullptr;
        items_ = nullptr;
        regenerate_ = true;
        nItems_ = 0;
        for (RefListItem<T> *ri = source.first(); ri != nullptr; ri = ri->next_)
            append(ri->item_);
    }
    void operator=(const RefList<T> &source)
    {
        // Clear any current data...
        clear();
        for (RefListItem<T> *ri = source.first(); ri != nullptr; ri = ri->next_)
            append(ri->item_);
    }
    void operator+=(const RefList<T> &source)
    {
        // Add unique items in the source list to our own
        for (RefListItem<T> *ri = source.first(); ri != nullptr; ri = ri->next_)
            addUnique(ri->item_);
    }
    RefListItem<T> *operator[](int index)
    {
        assert(index >= 0 && index < nItems_);

        return array()[index];
    }
    RefListItem<T> begin() const
    {
        if (listHead_ == nullptr)
            return end();

        return *listHead_;
    }
    const RefListItem<T> end() const
    {
        RefListItem<T> temp;
        return temp;
    }

    /*
     * Items
     */
    private:
    // Head and tail of reference items
    RefListItem<T> *listHead_, *listTail_;
    // Number of items in list
    int nItems_;
    // Static array of items
    RefListItem<T> **items_;
    // Array regeneration flag
    bool regenerate_;

    public:
    // Clear the list of all references
    void clear()
    {
        // Clear the list
        RefListItem<T> *xitem = listHead_;
        while (xitem != nullptr)
        {
            remove(xitem);
            xitem = listHead_;
        }
        listHead_ = nullptr;
        listTail_ = nullptr;
        nItems_ = 0;

        // Delete static items array if its there
        if (items_ != nullptr)
            delete[] items_;
        items_ = nullptr;
        regenerate_ = true;
    }
    // Returns the head of the item list
    RefListItem<T> *first() const { return listHead_; }
    // Returns the last item in the list
    RefListItem<T> *last() const { return listTail_; }
    // Returns the T referenced by the head of the item list
    T *firstItem() const
    {
        if (listHead_)
            return listHead_->item_;
        else
            return nullptr;
    }
    // Returns the T referenced by the tail of the item list
    T *lastItem() const
    {
        if (listTail_)
            return listTail_->item_;
        else
            return nullptr;
    }
    // Returns the number of atoms in the list
    int nItems() const { return nItems_; }
    // Append reference to the list
    RefListItem<T> *append(T *item)
    {
        RefListItem<T> *newitem = new RefListItem<T>;
        // Add the pointer to the list
        listHead_ == nullptr ? listHead_ = newitem : listTail_->next_ = newitem;
        newitem->prev_ = listTail_;
        listTail_ = newitem;
        newitem->item_ = item;
        ++nItems_;
        regenerate_ = true;
        return newitem;
    }
    // Prepend reference to the beginning of the list
    RefListItem<T> *prepend(T *item)
    {
        RefListItem<T> *newitem = new RefListItem<T>;
        // Add the pointer to the beginning of the list
        newitem->next_ = listHead_;
        listHead_ == nullptr ? listHead_ = newitem : listHead_->prev_ = newitem;
        listHead_ = newitem;
        newitem->item_ = item;
        ++nItems_;
        regenerate_ = true;
        return newitem;
    }
    // Add reference after the specified item
    RefListItem<T> *addAfter(RefListItem<T> *target, T *item)
    {
        if (target == nullptr)
            return add(item);

        RefListItem<T> *newitem = new RefListItem<T>;
        newitem->prev_ = target;
        newitem->next_ = target->next_;
        if (target->next_ != nullptr)
            target->next->prev_ = newitem;
        target->next_ = newitem;
        if (target == listTail_)
            listTail_ = newitem;
        newitem->item = item;
        ++nItems_;
        regenerate_ = true;

        return newitem;
    }
    // Add reference after the specified item (overload)
    RefListItem<T> *addAfter(T *targetItem, T *item)
    {
        // Find the specified item
        RefListItem<T> *target = contains(targetItem);
        if (target)
            return addAfter(target, item);

        return add(item);
    }
    // Add reference before the specified item
    RefListItem<T> *addBefore(RefListItem<T> *target, T *item)
    {
        if (target == nullptr)
            return add(item);

        RefListItem<T> *newitem = new RefListItem<T>;
        newitem->next_ = target;
        newitem->prev_ = target->prev_;
        if (target->prev_ != nullptr)
            target->prev->next_ = newitem;
        target->prev_ = newitem;
        if (target == listHead_)
            listHead_ = newitem;
        newitem->item = item;
        ++nItems_;
        regenerate_ = true;

        return newitem;
    }
    // Add reference before the specified item (overload)
    RefListItem<T> *addBefore(T *targetItem, T *item)
    {
        // Find the specified item
        RefListItem<T> *target = contains(targetItem);
        if (target)
            return addBefore(target, item);

        return addStart(item);
    }
    // Add reference to list, unless already there
    RefListItem<T> *addUnique(T *item)
    {
        RefListItem<T> *srch = contains(item);
        if (srch == nullptr)
            return append(item);
        else
            return srch;
    }
    // Cut item from list (orphan it)
    void cut(RefListItem<T> *item)
    {
        if (item == nullptr)
            return;

        RefListItem<T> *prev, *next;
        prev = item->prev_;
        next = item->next_;
        if (prev == nullptr)
            listHead_ = next;
        else
            prev->next_ = next;
        if (next == nullptr)
            listTail_ = prev;
        else
            next->prev_ = prev;
        item->next_ = nullptr;
        item->prev_ = nullptr;
        --nItems_;
        regenerate_ = true;
    }
    // Add an orphaned item into this list
    void own(RefListItem<T> *item)
    {
        // In the interests of 'pointer cleanliness, refuse to own the item if its pointers are not nullptr
        if ((item->next_ != nullptr) || (item->prev_ != nullptr))
        {
            fmt::print("RefList::own() <<<< Refused to own an item that still had links to other items >>>>\n");
            return;
        }
        listHead_ == nullptr ? listHead_ = item : listTail_->next_ = item;
        item->prev_ = listTail_;
        item->next_ = nullptr;
        listTail_ = item;
        ++nItems_;
        regenerate_ = true;
    }
    // Delete the item from the list
    void remove(RefListItem<T> *item)
    {
        if (item == nullptr)
            return;

        // Delete a specific RefDataItem from the list
        item->prev_ == nullptr ? listHead_ = item->next_ : item->prev_->next_ = item->next_;
        item->next_ == nullptr ? listTail_ = item->prev_ : item->next_->prev_ = item->prev_;
        delete item;
        nItems_--;
        regenerate_ = true;
    }
    // Delete the reference containing specified item from the list
    void remove(T *item)
    {
        // Delete a specific item from the list
        RefListItem<T> *r = contains(item);
        if (r != nullptr)
            remove(r);
    }
    // Remove the first item in the list
    void removeFirst()
    {
        if (listHead_ == nullptr)
            return;

        remove(listHead_);
        regenerate_ = true;
    }
    // Remove the last item in the list
    void removeLast()
    {
        if (listTail_ == nullptr)
            return;

        remove(listTail_);
        regenerate_ = true;
    }
    // Swap the two items specified
    void swap(T *item1, T *item2)
    {
        if ((item1 == nullptr) || (item2 == nullptr))
            return;

        T *prev1 = item1->prev, *next1 = item1->next_;
        item1->prev_ = item2->prev_;
        item1->next_ = item2->next_;
        item2->prev_ = prev1;
        item2->next_ = next1;
        regenerate_ = true;
    }
    // Fills the supplied array with 'n' pointer values to the reference items
    void fillArray(int n, T *destArray)
    {
        int count = 0;
        RefListItem<T> *ri = listHead_;
        while (ri != nullptr)
        {
            destArray[count] = ri->item;
            count++;
            if (count == n)
                break;
            ri = ri->next_;
            if (ri == nullptr)
                return;
        }
        regenerate_ = true;
    }
    // Return nth item in list
    T *item(int n)
    {
        if ((n < 0) || (n >= nItems_))
            throw std::runtime_error(fmt::format("Array index ({}) out of bounds ({} items in RefList).", n, nItems_));

        // Use array() function to return item
        return array()[n]->item();
    }
    // Return array of items
    RefListItem<T> **array()
    {
        if (regenerate_ == 0)
            return items_;

        // Delete old atom list (if there is one)
        if (items_ != nullptr)
            delete[] items_;

        // Create new list
        items_ = new RefListItem<T> *[nItems_];

        // Fill in pointers
        int count = 0;
        for (RefListItem<T> *ri = listHead_; ri != nullptr; ri = ri->next_)
            items_[count++] = ri;

        regenerate_ = false;

        return items_;
    }

    /*
     * Search
     */
    public:
    // Search references for item
    RefListItem<T> *contains(const T *item) const
    {
        // Search references for specified item
        for (RefListItem<T> *r = listHead_; r != nullptr; r = r->next_)
            if (r->item_ == item)
                return r;

        return nullptr;
    }
};
