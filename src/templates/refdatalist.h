// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <cassert>
#include <fmt/core.h>
#include <stddef.h>
#include <stdio.h>

// Forward Declarations
template <class T, class D> class RefDataList;
template <class T, class D> class RefDataListIterator;

/*
 * Item
 */

// Reference Item With Data
template <class T, class D> class RefDataItem
{
    public:
    RefDataItem<T, D>()
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
    // Associated data
    D data_;

    public:
    // Return item
    T *item() { return item_; }
    // Return associated data
    D &data() { return data_; }

    /*
     * List Pointers
     */
    private:
    // List pointers
    RefDataItem<T, D> *prev_, *next_;

    public:
    // Return item after this one
    RefDataItem<T, D> *next() const { return next_; }
    // Declare the list and iterator as friends
    friend class RefDataList<T, D>;
    friend class RefDataListIterator<T, D>;
};

/*
 * Reference List with Data
 */

template <class T, class D> class RefDataList
{
    public:
    RefDataList<T, D>()
    {
        listHead_ = nullptr;
        listTail_ = nullptr;
        items_ = nullptr;
        regenerate_ = true;
        nItems_ = 0;
    }

    ~RefDataList() { clear(); }
    RefDataList<T, D>(const RefDataList<T, D> &source)
    {
        listHead_ = nullptr;
        listTail_ = nullptr;
        items_ = nullptr;
        regenerate_ = true;
        nItems_ = 0;
        for (RefDataItem<T, D> *ri = source.first(); ri != nullptr; ri = ri->next_)
            append(ri->item_, ri->data_);
    }
    void operator=(const RefDataList<T, D> &source)
    {
        // Clear any current data...
        clear();
        for (RefDataItem<T, D> *ri = source.first(); ri != nullptr; ri = ri->next_)
            append(ri->item_, ri->data_);
    }
    RefDataItem<T, D> *operator[](int index)
    {
        assert(index >= 0 && index < nItems_);

        return array()[index];
    }

    /*
     * Items
     */
    private:
    // Head and tail of reference items
    RefDataItem<T, D> *listHead_, *listTail_;
    // Number of items in list
    int nItems_;
    // Static array of items
    RefDataItem<T, D> **items_;
    // Array regeneration flag
    bool regenerate_;

    public:
    // Clear the list of all references
    void clear()
    {
        // Clear the list
        RefDataItem<T, D> *xitem = listHead_;
        while (xitem != nullptr)
        {
            remove(xitem);
            xitem = listHead_;
        }
        listHead_ = nullptr;
        listTail_ = nullptr;
        nItems_ = 0;

        // Delete static items array if it's there
        if (items_ != nullptr)
            delete[] items_;
        items_ = nullptr;
        regenerate_ = true;
    }
    // Returns the head of the item list
    RefDataItem<T, D> *first() const { return listHead_; }
    // Returns the last item in the list
    RefDataItem<T, D> *last() const { return listTail_; }
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
    RefDataItem<T, D> *append(T *item)
    {
        RefDataItem<T, D> *newitem = new RefDataItem<T, D>;
        // Add the pointer to the list
        listHead_ == nullptr ? listHead_ = newitem : listTail_->next_ = newitem;
        newitem->prev_ = listTail_;
        listTail_ = newitem;
        newitem->item_ = item;
        ++nItems_;
        regenerate_ = true;
        return newitem;
    }
    // Append reference to the list with data
    RefDataItem<T, D> *append(T *item, D data)
    {
        RefDataItem<T, D> *newitem = new RefDataItem<T, D>;
        // Add the pointer to the list
        listHead_ == nullptr ? listHead_ = newitem : listTail_->next_ = newitem;
        newitem->prev_ = listTail_;
        listTail_ = newitem;
        newitem->item_ = item;
        newitem->data_ = data;
        ++nItems_;
        regenerate_ = true;
        return newitem;
    }
    // Prepend reference to the beginning of the list
    RefDataItem<T, D> *prepend(T *item, D data)
    {
        RefDataItem<T, D> *newitem = new RefDataItem<T, D>;
        // Add the pointer to the beginning of the list
        newitem->next_ = listHead_;
        listHead_ == nullptr ? listHead_ = newitem : listHead_->prev_ = newitem;
        listHead_ = newitem;
        newitem->item_ = item;
        newitem->data_ = data;
        ++nItems_;
        regenerate_ = true;
        return newitem;
    }
    // Add reference after the specified item
    RefDataItem<T, D> *addAfter(RefDataItem<T, D> *target, T *item)
    {
        if (target == nullptr)
            return add(item);

        RefDataItem<T, D> *newitem = new RefDataItem<T, D>;
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
    RefDataItem<T, D> *addAfter(T *targetItem, T *item)
    {
        // Find the specified item
        RefDataItem<T, D> *target = contains(targetItem);
        if (target)
            return addAfter(target, item);

        return add(item);
    }
    // Add reference before the specified item
    RefDataItem<T, D> *addBefore(RefDataItem<T, D> *target, T *item)
    {
        if (target == nullptr)
            return add(item);

        RefDataItem<T, D> *newitem = new RefDataItem<T, D>;
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
    RefDataItem<T, D> *addBefore(T *targetItem, T *item)
    {
        // Find the specified item
        RefDataItem<T, D> *target = contains(targetItem);
        if (target)
            return addBefore(target, item);

        return addStart(item);
    }
    // Add reference to list, unless already there, in which case we just set the data
    RefDataItem<T, D> *addUnique(T *item, D data)
    {
        RefDataItem<T, D> *srch = contains(item);
        if (srch == nullptr)
            return append(item, data);

        srch->data_ = data;
        return srch;
    }
    // Cut item from list (orphan it)
    void cut(RefDataItem<T, D> *item)
    {
        if (item == nullptr)
        {
            fmt::print("NULL pointer passed to RefDataList<T>::cut().\n");
            return;
        }
        RefDataItem<T, D> *prev, *next;
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
    void own(RefDataItem<T, D> *item)
    {
        // In the interests of 'pointer cleanliness, refuse to own the item if its pointers are not NULL
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
    void remove(RefDataItem<T, D> *item)
    {
        if (item == nullptr)
        {
            fmt::print("NULL pointer passed to RefDataList<T,D>::remove().\n");
            return;
        }
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
        RefDataItem<T, D> *r = contains(item);
        if (r != nullptr)
            remove(r);
    }
    // Remove all items that match specified data value
    void removeIfData(D value)
    {
        RefDataItem<T, D> *ri = listHead_, *next;
        while (ri)
        {
            // Store next pointer, in case we delete the current item
            next = ri->next_;
            if (ri->data == value)
                remove(ri);
            ri = next;
        }
    }
    // Remove the first item in the list
    void removeFirst()
    {
        if (listHead_ == nullptr)
        {
            fmt::print("No item to delete in  RefDataList<T,D>::removeFirst().\n");
            return;
        }
        remove(listHead_);
        regenerate_ = true;
    }
    // Remove the last item in the list
    void removeLast()
    {
        if (listTail_ == nullptr)
        {
            fmt::print("No item to delete in  RefDataList<T,D>::removeFirst().\n");
            return;
        }
        remove(listTail_);
        regenerate_ = true;
    }
    // Prune items with specified data
    void prune(const D data)
    {
        RefDataItem<T, D> *xitem = listHead_, *next;
        while (xitem != nullptr)
        {
            next = xitem->next_;
            if (xitem->data == data)
                remove(xitem);
            xitem = next;
        }
    }
    // Move head of list to tail of list
    void moveHeadToTail()
    {
        // Add a new item to the list (a copy of the current head)
        add(listHead_->item, listHead_->data);
        // Delete head item
        remove(listHead_);
    }
    // Swap the two items specified
    void swap(T *item1, T *item2)
    {
        if ((item1 == nullptr) || (item2 == nullptr))
        {
            fmt::print("NULL pointer(s) passed to RefDataList<T,D>::swap().\n", item1, item2);
            return;
        }
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
        RefDataItem<T, D> *ri = listHead_;
        while (ri != nullptr)
        {
            destArray[count] = ri->item;
            count++;
            if (count == n)
                break;
            ri = ri->next_;
            if (ri == nullptr)
                fmt::print("Not enough items in list (requested {}, had {}) in "
                           "RefDataList::fillArray()\n",
                           n, nItems_);
        }
        regenerate_ = true;
    }
    // Return nth item in list
    T *item(int n)
    {
        assert(n >= 0 && n < nItems_);

        // Use array() function to return item
        return array()[n]->item();
    }
    // Return (first) item with specified data
    T *itemWithData(D data)
    {
        // Search references for specified data
        for (RefDataItem<T, D> *r = listHead_; r != nullptr; r = r->next_)
            if (r->data_ == data)
                return r->item_;

        return nullptr;
    }
    // Return data for specified item
    D &dataForItem(T *item) const
    {
        // Search references for specified item
        for (RefDataItem<T, D> *r = listHead_; r != nullptr; r = r->next_)
            if (r->item_ == item)
                return r->data_;

        static D dummy;
        return dummy;
    }
    // Return array of items
    RefDataItem<T, D> **array()
    {
        if (regenerate_ == 0)
            return items_;

        // Delete old atom list (if there is one)
        if (items_ != nullptr)
            delete[] items_;

        // Create new list
        items_ = new RefDataItem<T, D> *[nItems_];

        // Fill in pointers
        int count = 0;
        for (RefDataItem<T, D> *ri = listHead_; ri != nullptr; ri = ri->next_)
            items_[count++] = ri;

        regenerate_ = false;

        return items_;
    }

    /*
     * Search
     */
    public:
    // Search references for item
    RefDataItem<T, D> *contains(const T *item) const
    {
        // Search references for specified item
        for (RefDataItem<T, D> *r = listHead_; r != nullptr; r = r->next_)
            if (r->item_ == item)
                return r;

        return nullptr;
    }
    // Search references for item and data
    RefDataItem<T, D> *contains(const T *item, D data) const
    {
        // Search references for specified item
        for (RefDataItem<T, D> *r = listHead_; r != nullptr; r = r->next_)
            if ((r->item_ == item) && (r->data_ == data))
                return r;

        return nullptr;
    }
    // Search references for data
    RefDataItem<T, D> *containsData(D data) const
    {
        // Search references for specified data
        for (RefDataItem<T, D> *r = listHead_; r != nullptr; r = r->next_)
            if (r->data_ == data)
                return r;

        return nullptr;
    }
};

/*
 * List Iterator
 */

// Reference List Data Iterator
template <class T, class D> class RefDataListIterator
{
    public:
    RefDataListIterator<T, D>(const RefDataList<T, D> &source, bool reverse = false) : reverse_(reverse), targetRefList_(source)
    {
        finished_ = false;
        currentItem_ = nullptr;
    }

    private:
    // Whether the iterator has reached the end of the list
    bool finished_;
    // Whether the iterator operates in reverse (iterating tail to head)
    bool reverse_;
    // Target list
    const RefDataList<T, D> &targetRefList_;
    // Current item
    RefDataItem<T, D> *currentItem_;

    public:
    // Iterate
    T *iterate()
    {
        if (finished_)
            return nullptr;

        // Go to initial / next item
        if (currentItem_ == nullptr)
            currentItem_ = reverse_ ? targetRefList_.last() : targetRefList_.first();
        else
            currentItem_ = reverse_ ? currentItem_->prev_ : currentItem_->next_;

        // Check for end of list
        if (currentItem_ == nullptr)
            finished_ = true;

        return (currentItem_ ? currentItem_->item() : nullptr);
    }
    // Peek the next item (if any)
    T *peek()
    {
        if (reverse_)
        {
            return (currentItem_ ? (currentItem_->prev_ ? currentItem_->prev_->item_ : nullptr) : nullptr);
        }
        else
            return (currentItem_ ? (currentItem_->next_ ? currentItem_->next_->item_ : nullptr) : nullptr);
    }
    // Peek the previous item (if any)
    T *peekPrevious()
    {
        if (reverse_)
        {
            return (currentItem_ ? (currentItem_->next_ ? currentItem_->next_->item_ : nullptr) : nullptr);
        }
        else
            return (currentItem_ ? (currentItem_->prev_ ? currentItem_->prev_->item_ : nullptr) : nullptr);
    }
    // Peek the next data (if any)
    D &peekData()
    {
        static D dummy;
        if (reverse_)
        {
            return (currentItem_ ? (currentItem_->prev_ ? currentItem_->prev_->data_ : dummy) : dummy);
        }
        else
            return (currentItem_ ? (currentItem_->next_ ? currentItem_->next_->data_ : dummy) : dummy);
    }
    // Peek the previous data (if any)
    D &peekPreviousData()
    {
        static D dummy;
        if (reverse_)
        {
            return (currentItem_ ? (currentItem_->next_ ? currentItem_->next_->data_ : dummy) : dummy);
        }
        else
            return (currentItem_ ? (currentItem_->prev_ ? currentItem_->prev_->data_ : dummy) : dummy);
    }
    // Return current reference item
    RefDataItem<T, D> *currentItem() { return currentItem_; }
    // Set current data
    void setCurrentData(D data)
    {
        if (currentItem_)
            currentItem_->data_ = data;
        else
            fmt::print("No current item, so can't set data in RefDataListIterator.\n");
    }
    // Return reference to current data
    D &currentData()
    {
        static D dummy;

        if (finished_)
            return dummy;

        return (currentItem_ ? currentItem_->data() : dummy);
    }
    // Restart iteration
    void restart()
    {
        finished_ = false;
        currentItem_ = nullptr;
    }
    // Return whether we are on the first item in the list
    bool isFirst() const { return (currentItem_ == targetRefList_.first()); }
    // Return whether we are on the last item in the list
    bool isLast() const { return (currentItem_ == targetRefList_.last()); }
};
