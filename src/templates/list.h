/*
    *** Linked List Class
    *** src/templates/list.h
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

#include "base/messenger.h"
#include <stdio.h>
#include <stdlib.h>

// Forward Declarations
/* none */

/*
 * List Class
 * Linked list for user-defined classes. Any class which is required to be contained in a List must subclass ListItem.
 */
template <class T> class List
{
    public:
    List<T>()
    {
        listHead_ = nullptr;
        listTail_ = nullptr;
        nItems_ = 0;
        regenerate_ = true;
        disownOnDestruction_ = false;
        items_ = nullptr;
    }
    ~List()
    {
        if (disownOnDestruction_)
        {
            while (listHead_)
                takeFirst();
        }
        else
            clear();
    }
    List<T>(const List<T> &source)
    {
        listHead_ = nullptr;
        listTail_ = nullptr;
        nItems_ = 0;
        regenerate_ = true;
        disownOnDestruction_ = false;
        items_ = nullptr;
        (*this) = source;
    }
    List<T> &operator=(const List<T> &source)
    {
        // Clear any current data in the list...
        clear();
        T *newitem, *olditem;
        for (olditem = source.first(); olditem != nullptr; olditem = olditem->next_)
        {
            // To ensure that we don't mess around with the pointers of the old list, copy the object and then own
            // it
            newitem = new T(*olditem);
            newitem->prev_ = nullptr;
            newitem->next_ = nullptr;
            own(newitem);
        }

        disownOnDestruction_ = source.disownOnDestruction_;

        return *this;
    }
    T *operator[](int index)
    {
        if ((index < 0) || (index >= nItems_))
        {
            Messenger::error("LIST_OPERATOR[] - Array index ({}) out of bounds ({} items in List).\n", index, nItems_);
            return nullptr;
        }
        return array()[index];
    }

    /*
     * Basic Data
     */
    protected:
    // Pointers to head and tail of list
    T *listHead_, *listTail_;
    // Number of items in list
    int nItems_;
    // Static array of items
    T **items_;
    // Array regeneration flag
    bool regenerate_;
    // Whether to only disown items on destruction, rather than delete them
    bool disownOnDestruction_;

    public:
    // Clear the list
    void clear()
    {
        while (listTail_)
            remove(listTail_);

        // Delete static items array and reset all quantities
        if (items_)
            delete[] items_;
        items_ = nullptr;
        listHead_ = nullptr;
        listTail_ = nullptr;
        nItems_ = 0;
        regenerate_ = true;
    }
    // Create empty list of specified size
    void createEmpty(int size)
    {
        clear();
        for (int n = 0; n < size; ++n)
            add();
        regenerate_ = true;
    }
    // Returns the number of items in the list
    int nItems() const { return nItems_; }
    // Returns the list head
    T *first() const { return listHead_; }
    // Returns the second item in the list
    T *second() const { return (listHead_ == nullptr ? nullptr : listHead_->next_); }
    // Returns the list tail
    T *last() const { return listTail_; }
    // Set whether to only disown items on destruction, rather than delete them
    void setDisownOnDestruction(bool dod) { disownOnDestruction_ = dod; }

    /*
     * Item Addition
     */
    public:
    // Append an item to the list
    template <typename... Args> T *add(Args... args)
    {
        T *newItem = new T(args...);
        // Add the pointer to the list
        listHead_ == nullptr ? listHead_ = newItem : listTail_->next_ = newItem;
        newItem->prev_ = listTail_;
        listTail_ = newItem;
        ++nItems_;
        regenerate_ = true;
        return newItem;
    }
    // Prepend an item to the list
    template <typename... Args> T *prepend(Args... args)
    {
        T *newItem = new T(args...);

        // Add the pointer to the start of the list
        newItem->next_ = listHead_;
        listHead_ == nullptr ? listTail_ = newItem : listHead_->prev_ = newItem;
        listHead_ = newItem;
        ++nItems_;
        regenerate_ = true;
        return newItem;
    }
    // Add an item into the list at the position specified
    T *addAt(int position)
    {
        // If position is -1, or the list is empty, just add at end of the list
        if ((position == -1) || (nItems_ == 0))
            return add();

        // If position is zero, add at start of list
        if (position == 0)
            return insertBefore(listHead_);

        // Get item at (position-1), and add a new item after it
        T *prevItem = array()[position - 1];
        return insertAfter(prevItem);
    }
    // Insert an item into the list (after supplied item)
    template <typename... Args> T *insertAfter(T *item, Args... args)
    {
        if (item == nullptr)
            return prepend();

        T *newItem = new T(args...);
        // Get pointer to next item after the supplied item (our newnext)
        T *newNext = item->next_;
        // Re-point newprev and the new item
        item->next_ = newItem;
        newItem->prev_ = item;
        // Re-point newnext and the new item
        if (newNext != nullptr)
            newNext->prev_ = newItem;
        else
            listTail_ = newItem;
        newItem->next_ = newNext;
        ++nItems_;
        regenerate_ = true;
        return newItem;
    }
    // Insert an item into the list (before supplied item)
    template <typename... Args> T *insertBefore(T *item, Args... args)
    {
        if (item == nullptr)
        {
            Messenger::error("No item supplied to List<T>::insertBefore().\n");
            return nullptr;
        }
        T *newItem = new T(args...);
        // Get pointer to next item after the supplied item (our newprev)
        T *newPrev = item->prev_;
        // Re-point newnext and the new item
        item->prev_ = newItem;
        newItem->next_ = item;
        // Re-point newprev and the new item
        if (newPrev != nullptr)
            newPrev->next_ = newItem;
        else
            listHead_ = newItem;
        newItem->prev_ = newPrev;
        ++nItems_;
        regenerate_ = true;
        return newItem;
    }
    // Add an existing item into this list
    void own(T *oldItem)
    {
        if (oldItem == nullptr)
        {
            Messenger::error("Internal Error: NULL pointer passed to List<T>::own().\n");
            return;
        }
        // In the interests of 'pointer etiquette', refuse to own the item if its pointers are not NULL
        if ((oldItem->next_ != nullptr) || (oldItem->prev_ != nullptr))
        {
            Messenger::error("List::own() <<<< Refused to own an item that still had links to other items >>>>\n");
            return;
        }
        listHead_ == nullptr ? listHead_ = oldItem : listTail_->next_ = oldItem;
        oldItem->prev_ = listTail_;
        oldItem->next_ = nullptr;
        listTail_ = oldItem;
        ++nItems_;
        regenerate_ = true;
    }
    // Own an item into the list (before supplied item)
    void ownBefore(T *oldItem, T *beforeItem)
    {
        if (beforeItem == nullptr)
        {
            Messenger::error("No item supplied to List<T>::ownBefore().\n");
            return;
        }
        if (oldItem == nullptr)
        {
            Messenger::error("Internal Error: NULL pointer passed to List<T>::own().\n");
            return;
        }
        // In the interests of 'pointer etiquette', refuse to own the item if its pointers are not NULL
        if ((oldItem->next_ != nullptr) || (oldItem->prev_ != nullptr))
        {
            Messenger::error("List::own() <<<< Refused to own an item that still had links to other items >>>>\n");
            return;
        }
        // Get pointer to next item after the supplied item (our newprev)
        T *newPrev = beforeItem->prev_;
        // Re-point newnext and the new item
        beforeItem->prev_ = oldItem;
        oldItem->next_ = beforeItem;
        // Re-point newprev and the new item
        if (newPrev != nullptr)
            newPrev->next_ = oldItem;
        else
            listHead_ = oldItem;
        oldItem->prev_ = newPrev;
        ++nItems_;
        regenerate_ = true;
    }

    /*
     * Item Removal
     */
    public:
    // Disown the item, but do not delete it
    void disown(T *item)
    {
        if (item == nullptr)
        {
            Messenger::error("Internal Error: NULL pointer passed to List<T>::disown().\n");
            return;
        }
        item->prev_ == nullptr ? listHead_ = item->next_ : item->prev_->next_ = item->next_;
        item->next_ == nullptr ? listTail_ = item->prev_ : item->next_->prev_ = item->prev_;
        item->next_ = nullptr;
        item->prev_ = nullptr;

        --nItems_;
        regenerate_ = true;
    }
    // Take the first item out of the list
    T *takeFirst()
    {
        if (listHead_ == nullptr)
            return nullptr;

        T *item = listHead_;
        disown(item);

        return item;
    }
    // Take the last item out of the list
    T *takeLast()
    {
        if (listTail_ == nullptr)
            return nullptr;

        T *item = listTail_;
        disown(item);

        return item;
    }
    // Remove an item from the list
    void remove(T *item)
    {
        if (item == nullptr)
        {
            Messenger::error("Internal Error: NULL pointer passed to List<T>::remove().\n");
            return;
        }
        // Delete a specific item from the list
        item->prev_ == nullptr ? listHead_ = item->next_ : item->prev_->next_ = item->next_;
        item->next_ == nullptr ? listTail_ = item->prev_ : item->next_->prev_ = item->prev_;
        delete item;
        --nItems_;
        regenerate_ = true;
    }
    // Remove the item at the specified position
    void remove(int pos)
    {
        if ((pos < 0) || (pos >= nItems_))
        {
            Messenger::error("Internal Error: Invalid position {} passed to List<T>::remove(int pos) (nItems_ = {})\n", pos,
                             nItems_);
            return;
        }
        remove(array()[pos]);
    }
    // Remove first item from the list
    void removeFirst()
    {
        if (listHead_ == nullptr)
        {
            Messenger::error("Internal Error: No first item to delete in list.\n");
            return;
        }

        // Delete a first item from the list
        T *xitem = listHead_;
        xitem->next_ == nullptr ? listTail_ = xitem->prev_ : xitem->next_->prev_ = xitem->prev_;
        listHead_ = xitem->next_;
        delete xitem;
        --nItems_;
        regenerate_ = true;
    }
    // Remove last item from the list
    void removeLast()
    {
        if (listTail_ == nullptr)
        {
            Messenger::error("Internal Error: No last item to delete in list.\n");
            return;
        }
        // Delete the last item from the list
        T *xitem = listTail_;
        xitem->prev_ == nullptr ? listHead_ = xitem->next_ : xitem->prev_->next_ = xitem->next_;
        listTail_ = xitem->prev_;
        delete xitem;
        --nItems_;
        regenerate_ = true;
    }
    // Remove an item from the list, and return the next in the list
    T *removeAndGetNext(T *item)
    {
        if (item == nullptr)
        {
            Messenger::error("Internal Error: NULL pointer passed to List<T>::removeAndGetNext().\n");
            return nullptr;
        }
        // Delete a specific item from the list, and return the next in the list
        T *result = item->next_;
        item->prev_ == nullptr ? listHead_ = item->next_ : item->prev_->next_ = item->next_;
        item->next_ == nullptr ? listTail_ = item->prev_ : item->next_->prev_ = item->prev_;
        delete item;
        --nItems_;
        regenerate_ = true;
        return result;
    }
    // Cut item from list, bridging over specified item
    void cut(T *item)
    {
        if (item == nullptr)
        {
            Messenger::error("Internal Error: NULL pointer passed to List<T>::cut().\n");
            return;
        }
        T *prev, *next;
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
        regenerate_ = true;
    }

    /*
     * Array / Indexing
     */
    public:
    // Find list index of supplied item
    int indexOf(const T *item) const
    {
        int result = 0;
        for (T *i = listHead_; i != nullptr; i = i->next_)
        {
            if (item == i)
                return result;
            ++result;
        }
        Messenger::error("Internal Error: List::indexOf() could not find supplied item.\n");
        return -1;
    }
    // Return nth item in List
    T *item(int n) const
    {
        if ((n < 0) || (n >= nItems_))
        {
            Messenger::error("Internal Error: List array index {} is out of bounds in List<T>::item().\n", n);
            return nullptr;
        }
        int count = -1;
        for (T *item = listHead_; item != nullptr; item = item->next_)
            if (++count == n)
                return item;
        return nullptr;
    }
    // Fills the supplied array with pointer values to the list items
    void fillArray(int nItems, T **itemArray)
    {
        int count = 0;
        T *i = listHead_;
        while (i != nullptr)
        {
            itemArray[count] = i->item;
            ++count;
            if (count == nItems)
                break;
            i = i->next_;
            if (i == nullptr)
                Messenger::error("List::fillArray <<<< Not enough items in list - requested {}, had {} >>>>\n", nItems,
                                 nItems_);
        }
    }
    // Generate (if necessary) and return item array
    T **array()
    {
        if (!regenerate_)
            return items_;

        // Delete old item array (if there is one)
        if (items_ != nullptr)
            delete[] items_;
        items_ = nullptr;

        if (nItems_ == 0)
            return items_;

        // Create new list
        items_ = new T *[nItems_];

        // Fill in pointers
        int count = 0;
        for (T *i = listHead_; i != nullptr; i = i->next_)
            items_[count++] = i;
        regenerate_ = false;
        return items_;
    }

    /*
     * Search
     */
    public:
    // Return whether the item is owned by the list
    bool contains(const T *searchItem) const
    {
        T *item;
        for (item = listHead_; item != nullptr; item = item->next_)
            if (searchItem == item)
                break;
        return (item != nullptr);
    }

    /*
     * Item Moves
     */
    private:
    // Swap two items in list (by pointer)
    void swap(T *item1, T *item2)
    {
        if ((item1 == nullptr) || (item2 == nullptr))
            return;

        // If the items are adjacent, swap the pointers 'outside' the pair and swap the next/prev between them
        T *n1, *n2, *p1, *p2;
        if ((item1->next_ == item2) || (item2->next_ == item1))
        {
            // Order the pointers so that item1->next_ == item2
            if (item2->next_ == item1)
            {
                n1 = item2;
                item2 = item1;
                item1 = n1;
            }
            p1 = item1->prev_;
            n2 = item2->next_;
            item2->prev_ = p1;
            item2->next_ = item1;
            if (p1 != nullptr)
                p1->next_ = item2;
            else
                listHead_ = item2;
            item1->prev_ = item2;
            item1->next_ = n2;
            if (n2 != nullptr)
                n2->prev_ = item1;
            else
                listTail_ = item1;
        }
        else
        {
            // Store the list pointers of the two items
            n1 = item1->next_;
            p1 = item1->prev_;
            n2 = item2->next_;
            p2 = item2->prev_;

            // Set new values of swapped items
            item1->next_ = n2;
            item1->prev_ = p2;
            item2->next_ = n1;
            item2->prev_ = p1;

            // Set new values of items around swapped items
            if (item1->next_ != nullptr)
                item1->next_->prev_ = item1;
            else
                listTail_ = item1;
            if (item1->prev_ != nullptr)
                item1->prev_->next_ = item1;
            else
                listHead_ = item1;
            if (item2->next_ != nullptr)
                item2->next_->prev_ = item2;
            else
                listTail_ = item2;
            if (item2->prev_ != nullptr)
                item2->prev_->next_ = item2;
            else
                listHead_ = item2;
        }
        regenerate_ = true;
    }

    public:
    // Shift item up (towards head)
    void shiftUp(T *item)
    {
        if (item == nullptr)
        {
            Messenger::error("Internal Error: NULL pointer passed to List<T>::shiftUp().\n");
            return;
        }
        // If the item is already at the head of the list, return NULL.
        if (listHead_ == item)
            return;
        swap(item->prev_, item);
        regenerate_ = true;
    }
    // Shift item down (towards tail)
    void shiftDown(T *item)
    {
        if (item == nullptr)
        {
            Messenger::error("Internal Error: NULL pointer passed to List<T>::shiftDown().\n");
            return;
        }

        // If the item is already at the tail of the list, return.
        if (listTail_ == item)
            return;
        swap(item->next_, item);
        regenerate_ = true;
    }
    // Move item at position 'old' by 'delta' positions (+/-)
    void move(int target, int delta)
    {
        // Check positions
        if ((target < 0) || (target >= nItems_))
        {
            Messenger::error("Internal Error: Old position ({}) is out of range (0 - {}) in List<T>::move\n", target,
                             nItems_ - 1);
            return;
        }
        int newpos = target + delta;
        if ((newpos < 0) || (newpos >= nItems_))
        {
            Messenger::error("Internal Error: New position ({}) is out of range (0 - {}) in List<T>::move\n", newpos,
                             nItems_ - 1);
            return;
        }
        // Get pointer to item that we're moving and shift it
        T *olditem = array()[target];
        for (int n = 0; n < abs(delta); n++)
            (delta < 0 ? shiftUp(olditem) : shiftDown(olditem));
        // Flag for regeneration
        regenerate_ = true;
    }
    // Move item to end of list
    void moveToEnd(T *item)
    {
        if (item == nullptr)
        {
            Messenger::error("Internal Error: NULL pointer passed to List<T>::moveToEnd().\n");
            return;
        }
        // If the item is already at the tail, exit
        if (listTail_ == item)
            return;
        cut(item);
        item->prev_ = listTail_;
        item->next_ = nullptr;
        if (listTail_ != nullptr)
            listTail_->next_ = item;
        listTail_ = item;
        regenerate_ = true;
    }
    // Move item to start of list
    void moveToStart(T *item)
    {
        if (item == nullptr)
        {
            Messenger::error("Internal Error: NULL pointer passed to List<T>::moveToStart().\n");
            return;
        }
        // If the item is already at the head, exit
        if (listHead_ == item)
            return;
        cut(item);
        item->prev_ = nullptr;
        item->next_ = listHead_;
        if (listHead_ != nullptr)
            listHead_->prev_ = item;
        listHead_ = item;
        regenerate_ = true;
    }
    // Move item so it is after specified item
    void moveAfter(T *item, T *reference)
    {
        // If 'reference' is nullptr, then move to the start of the list
        if (item == nullptr)
        {
            Messenger::error("Internal Error: NULL pointer passed to List<T>::moveAfter().\n");
            return;
        }

        // Cut item out of list...
        T *prev, *next;
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

        // ...and then re-insert it
        // Get pointer to next item after newprev (our newnext)
        T *newnext = (reference == nullptr ? listHead_ : reference->next_);
        // Re-point reference and the new item
        if (reference != nullptr)
            reference->next_ = item;
        else
            listHead_ = item;
        item->prev_ = reference;
        // Re-point newnext and the new item
        if (newnext != nullptr)
            newnext->prev_ = item;
        else
            listTail_ = item;
        item->next_ = newnext;
        regenerate_ = true;
    }
    // Move item so it is before specified item
    void moveBefore(T *item, T *reference)
    {
        // Check for valid item
        if (item == nullptr)
        {
            Messenger::error("Internal Error: NULL pointer passed to List<T>::moveAfter().\n");
            return;
        }

        // Cut item out of list
        T *prev, *next;
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

        // ...and then re-insert it
        // Get pointer to item before the reference (our newPrev)
        T *newPrev = (reference == nullptr ? listTail_ : reference->prev_);

        // Re-point reference and the new item
        if (reference != nullptr)
            reference->prev_ = item;
        else
            listTail_ = item;
        item->next_ = reference;

        // Re-point newPrev and the new item
        if (newPrev != nullptr)
            newPrev->next_ = item;
        else
            listHead_ = item;
        item->prev_ = newPrev;

        regenerate_ = true;
    }
    // Swap two items in list
    void swapByIndex(int id1, int id2)
    {
        // Check positions
        if ((id1 < 0) || (id1 >= nItems_))
        {
            Messenger::error("Internal Error: First index ({}) is out of range (0 - {}) in List<T>::swapByIndex\n", id1,
                             nItems_ - 1);
            return;
        }
        if ((id2 < 0) || (id2 >= nItems_))
        {
            Messenger::error("Internal Error: Second index ({}) is out of range (0 - {}) in List<T>::swapByIndex\n", id2,
                             nItems_ - 1);
            return;
        }

        // Get pointers to item that we're swapping, and swap them
        swap(array()[id1], array()[id2]);

        // Flag for regeneration
        regenerate_ = true;
    }
};

/*
 * List with Parent references for items
 */
template <class T, class P> class ParentList : public List<T>
{
    public:
    // Override the add() member function
    T *add(P &parent)
    {
        T *newitem = new T(parent);
        List<T>::own(newitem);
        return newitem;
    }
    // Override the assignment operator
    void operator=(const ParentList<T, P> &source)
    {
        // Clear any current data in the list...
        List<T>::clear();
        T *newitem, *olditem;
        for (olditem = source.first(); olditem != nullptr; olditem = olditem->next_)
        {
            // To ensure that we don't mess around with the pointers of the old list, copy the object and then own
            // it
            newitem = new T(olditem->parent());
            *newitem = *olditem;
            newitem->prev_ = nullptr;
            newitem->next_ = nullptr;
            List<T>::own(newitem);
        }
        // Don't deep-copy the static list, just flag that it must be regenerated if required.
        List<T>::regenerate_ = 1;
    }
};

/*
 * List Iterator
 */
template <class T> class ListIterator
{
    public:
    ListIterator<T>(const List<T> &source, bool reverse = false) : reverse_(reverse), targetList_(source)
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
    const List<T> &targetList_;
    // Current item
    T *currentItem_;

    public:
    // Iterate
    T *iterate()
    {
        if (finished_)
            return nullptr;

        // Go to initial / next item
        if (currentItem_ == nullptr)
            currentItem_ = reverse_ ? targetList_.last() : targetList_.first();
        else
            currentItem_ = reverse_ ? currentItem_->prev_ : currentItem_->next_;

        // Check for end of list
        if (currentItem_ == nullptr)
            finished_ = true;

        return currentItem_;
    }
    // Restart iteration
    void restart()
    {
        finished_ = false;
        currentItem_ = nullptr;
    }
    // Return whether we are on the first item in the list
    bool isFirst() const { return (currentItem_ == targetList_.first()); }
    // Return whether we are on the last item in the list
    bool isLast() const { return (currentItem_ == targetList_.last()); }
    // Peek the next item (if any)
    T *peek()
    {
        if (reverse_)
        {
            return (currentItem_ ? currentItem_->prev_ : nullptr);
        }
        else
            return (currentItem_ ? currentItem_->next_ : nullptr);
    }
    // Peek the previous item (if any)
    T *peekPrevious()
    {
        if (reverse_)
        {
            return (currentItem_ ? currentItem_->next_ : nullptr);
        }
        else
            return (currentItem_ ? currentItem_->prev_ : nullptr);
    }
};
