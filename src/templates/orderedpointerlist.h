/*
    *** Ordered Pointer List Class
    *** src/templates/orderedpointerlist.h
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

#include "templates/factory.h"
#include <stdio.h>
#include <stdlib.h>

// OrderedPointerListItem Class
template <class T> class OrderedPointerListItem
{
    /*
     * OrderedPointerList maintains a list of OrderedPointerListItems, which contain a pointer to an object of the template
     * class type. The only requirement of the template class T is that is provides a member function 'int T::index()' which
     * provides the index by which it should be sorted.
     */

    public:
    OrderedPointerListItem<T>(T *object = nullptr);
    // List pointers
    OrderedPointerListItem<T> *prev, *next;

    private:
    // Pointer to object
    T *object_;

    public:
    // Set pointer to object
    void setObject(T *object);
    // Return pointer to object
    T *object();
};

template <class T> OrderedPointerListItem<T>::OrderedPointerListItem(T *object) : object_(object)
{
    prev = nullptr;
    next = nullptr;
}

// Set pointer to object
template <class T> void OrderedPointerListItem<T>::setObject(T *object) { object_ = object; }

// Return reference to object
template <class T> T *OrderedPointerListItem<T>::object() { return object_; }

// OrderedPointerList Class
template <class T> class OrderedPointerList
{
    /*
     * Ordered, linked list of pointers to objects which exist elsewhere.
     */

    public:
    OrderedPointerList<T>();
    ~OrderedPointerList();
    OrderedPointerList<T>(const OrderedPointerList<T> &source);

    /*
     * Item List
     */
    private:
    // Object factory
    ObjectFactory<OrderedPointerListItem<T>> factory_;
    // Pointers to head and tail of list
    OrderedPointerListItem<T> *listHead_, *listTail_;
    // Number of items in list
    int nItems_;
    // Static array of items
    OrderedPointerListItem<T> **items_;
    // Static array of objects
    T **objects_;
    // Array sizes (at last new)
    int itemArraySize_, objectArraySize_;
    // Array regeneration flags
    bool regenerateItemArray_, regenerateObjectArray_;

    /*
     * Access / Manipulation
     */
    private:
    // Insert item pointing to specified object, after specified item
    OrderedPointerListItem<T> *insertAfter(T *object, OrderedPointerListItem<T> *afterThis);
    // Insert item pointing to specified object, before specified item
    OrderedPointerListItem<T> *insertBefore(T *object, OrderedPointerListItem<T> *beforeThis);
    // Insert specified item, before specified item
    void insertBefore(OrderedPointerListItem<T> *item, OrderedPointerListItem<T> *beforeThis);
    // Remove an item from the list
    void remove(OrderedPointerListItem<T> *xitem);
    // Return list item for the specified object, if it exists
    OrderedPointerListItem<T> *objectInList(T *object) const;
    // Cut item from list
    void cut(OrderedPointerListItem<T> *item);
    // Find and return the item with the next highest index to the index specified
    OrderedPointerListItem<T> *nextHighest(T *object);

    public:
    // Clear the list
    void clear();
    // Returns the number of items referenced in the list
    int nItems() const;
    // Add a new item reference to the list
    void add(T *object);
    // Add a new item reference to the list, unless it is already there
    void addExclusive(T *object);
    // Add a new item reference to the end of the list
    void addAtEnd(T *object);
    // Remove item reference from list
    void remove(T *object);
    // Remove item reference from the list (but don't complain if it isn't there)
    bool removeIfPresent(T *object);
    // Move specified item to target list
    void move(T *object, OrderedPointerList<T> &targetList);
    // Returns the list head
    OrderedPointerListItem<T> *first() const;
    // Returns the list tail
    OrderedPointerListItem<T> *last() const;
    // Return whether the specified pointer is present in the list
    bool contains(T *object) const;
    // Generate (if necessary) and return item array
    OrderedPointerListItem<T> **items();
    // Generate (if necessary) and return object array
    T **objects();
    // Invalidate current item and object arrays, forcing them to be recreated
    void invalidateLists();

    /*
     * Operators
     */
    public:
    void operator=(const OrderedPointerList<T> &other);
    OrderedPointerListItem<T> *operator[](int);
};

template <class T> OrderedPointerList<T>::OrderedPointerList()
{
    listHead_ = nullptr;
    listTail_ = nullptr;
    nItems_ = 0;
    regenerateItemArray_ = 1;
    regenerateObjectArray_ = 1;
    itemArraySize_ = 0;
    objectArraySize_ = 0;
    items_ = nullptr;
    objects_ = nullptr;
}

template <class T> OrderedPointerList<T>::~OrderedPointerList() { clear(); }

template <class T> OrderedPointerList<T>::OrderedPointerList(const OrderedPointerList<T> &source) { (*this) = source; }

/*
 * Access / Manipulation
 */

// Insert reference to specifed object after supplied item
template <class T>
OrderedPointerListItem<T> *OrderedPointerList<T>::insertAfter(T *object, OrderedPointerListItem<T> *afterThis)
{
    // Create new list item (from the ObjectFactory)
    OrderedPointerListItem<T> *newItem = factory_.produce();
    newItem->setObject(object);

    // Get pointer to next item in list, after the list item 'afterThis'
    // If 'afterThis' is nullptr, then we insert at the start of the list (and make listHead_ point to the new item)
    OrderedPointerListItem<T> *newNext;
    if (afterThis == nullptr)
    {
        // First item in list will be the newItem, so 'newNext will be the current listHead_
        newNext = listHead_;
        listHead_ = newItem;
    }
    else
    {
        newNext = afterThis->next;
        afterThis->next = newItem;
    }

    // Set pointer to previous item (of our newItem)
    newItem->prev = afterThis;

    // Re-point previous pointer of newNext
    if (newNext != nullptr)
        newNext->prev = newItem;
    else
        listTail_ = newItem;

    // Re-point 'next' pointer of newItem;
    newItem->next = newNext;

    // Increase item count, and flag that the array should be regenerated.
    ++nItems_;
    regenerateItemArray_ = 1;
    regenerateObjectArray_ = 1;

    return newItem;
}

// Insert reference to specifed object after supplied item
template <class T>
OrderedPointerListItem<T> *OrderedPointerList<T>::insertBefore(T *object, OrderedPointerListItem<T> *beforeThis)
{
    // Create new list item (from the ObjectFactory)
    OrderedPointerListItem<T> *newItem = factory_.produce();
    newItem->setObject(object);
    insertBefore(newItem, beforeThis);
    return newItem;
}

// Insert specified item, before specified item
template <class T>
void OrderedPointerList<T>::insertBefore(OrderedPointerListItem<T> *item, OrderedPointerListItem<T> *beforeThis)
{
    // Get pointer to prev item in list, after the list item 'beforeThis'
    // If 'beforeThis' is nullptr, then we insert at the end of the list (and make listTail_ point to the new item)
    OrderedPointerListItem<T> *newPrev;
    if (beforeThis == nullptr)
    {
        // First item in list will be the item, so newPrev will be the current listTail_
        newPrev = listTail_;
        listTail_ = item;
    }
    else
    {
        newPrev = beforeThis->prev;
        beforeThis->prev = item;
    }

    // Set pointer to next item (of our item)
    item->next = beforeThis;

    // Re-point nextious pointer of newPrev
    if (newPrev != nullptr)
        newPrev->next = item;
    else
        listHead_ = item;

    // Re-point 'prev' pointer of item;
    item->prev = newPrev;

    // Increase item count, and flag that the array should be regenerated.
    ++nItems_;
    regenerateItemArray_ = 1;
    regenerateObjectArray_ = 1;
}

// Remove the specified item from the list
template <class T> void OrderedPointerList<T>::remove(OrderedPointerListItem<T> *xitem)
{
    if (xitem == nullptr)
    {
        Messenger::error("NULL pointer passed to OrderedPointerList<T>::remove().\n");
        return;
    }
    // Delete a specific item from the list
    xitem->prev == nullptr ? listHead_ = xitem->next : xitem->prev->next = xitem->next;
    xitem->next == nullptr ? listTail_ = xitem->prev : xitem->next->prev = xitem->prev;
    factory_.returnObject(xitem);
    --nItems_;
    regenerateItemArray_ = 1;
    regenerateObjectArray_ = 1;
}

// Return list item for the specified object, if it exists
template <class T> OrderedPointerListItem<T> *OrderedPointerList<T>::objectInList(T *object) const
{
    // TODO This can probably be made much faster - bisection?
    OrderedPointerListItem<T> *item = listHead_;
    while (item)
    {
        if (item->object() > object)
            return nullptr;
        if (item->object() == object)
            return item;
        item = item->next;
    }
    return nullptr;
}

// Cut - Bridge items over specified item
template <class T> void OrderedPointerList<T>::cut(OrderedPointerListItem<T> *item)
{
    if (item == nullptr)
    {
        Messenger::error("NULL pointer passed to OrderedPointerList<T>::cut().\n");
        return;
    }

    // Grab list pointers for specified item
    OrderedPointerListItem<T> *prev, *next;
    prev = item->prev;
    next = item->next;

    // Adjust previous item
    if (prev == nullptr)
        listHead_ = next;
    else
        prev->next = next;

    // Adjust next item
    if (next == nullptr)
        listTail_ = prev;
    else
        next->prev = prev;

    item->next = nullptr;
    item->prev = nullptr;

    --nItems_;
    regenerateItemArray_ = 1;
    regenerateObjectArray_ = 1;
}

// Find and return the item with the next highest index to the index specified
template <class T> OrderedPointerListItem<T> *OrderedPointerList<T>::nextHighest(T *object)
{
    // TODO This can probably be made much faster - binary chop?
    OrderedPointerListItem<T> *item = listHead_;
    while (item)
    {
        if (item->object() > object)
            return item;
        item = item->next;
    }
    return nullptr;
}

//  Remove all items in the list
template <class T> void OrderedPointerList<T>::clear()
{
    for (OrderedPointerListItem<T> *item = listHead_; item != nullptr; item = item->next)
        factory_.returnObject(item);
    nItems_ = 0;
    listHead_ = nullptr;
    listTail_ = nullptr;

    // Delete static item anb objects array if they exist
    if (items_ != nullptr)
        delete[] items_;
    items_ = nullptr;
    if (objects_ != nullptr)
        delete[] objects_;
    objects_ = nullptr;
    regenerateItemArray_ = 1;
    regenerateObjectArray_ = 1;
}

// Return the number of items in the list
template <class T> int OrderedPointerList<T>::nItems() const { return nItems_; }

// Add item to list
template <class T> void OrderedPointerList<T>::add(T *object)
{
#ifdef CHECKS
    if (object == nullptr)
    {
        Messenger::error("NULL_POINTER - nullptr passed to OrderedPointerList<T>::add().\n");
        return;
    }
#endif
    // Add it in the correct place in the list
    OrderedPointerListItem<T> *nextLargest = nextHighest(object);
    insertBefore(object, nextLargest);
}

// Add a new item reference to the list, unless it is already there
template <class T> void OrderedPointerList<T>::addExclusive(T *object)
{
#ifdef CHECKS
    if (object == nullptr)
    {
        Messenger::error("NULL_POINTER - nullptr passed to OrderedPointerList<T>::add().\n");
        return;
    }
#endif
    // Seek the next highest index, checking to see if we find the specified index
    // TODO This can be made much faster - binary chop?
    OrderedPointerListItem<T> *nextLargest = listHead_;
    while (nextLargest)
    {
        if (nextLargest->object() > object)
            break;
        else if (nextLargest->object() == object)
            return;
        nextLargest = nextLargest->next;
    }

    insertBefore(object, nextLargest);
}

// Add a new item reference to the end of the list
template <class T> void OrderedPointerList<T>::addAtEnd(T *object)
{
#ifdef CHECKS
    if (object == nullptr)
    {
        Messenger::error("NULL_POINTER - nullptr passed to OrderedPointerList<T>::addAtEnd().\n");
        return;
    }
#endif
    // Add it directly to the end of the list, provided this adheres to the current order
    // Check object index of last item in list
    if (listTail_ == nullptr)
        insertAfter(object, nullptr);
    else if (listTail_->object() < object)
        insertAfter(object, listTail_);
}

// Remove item reference from list
template <class T> void OrderedPointerList<T>::remove(T *object) { remove(objectInList(object)); }

// Remove item reference from the list (but don't complain if it isn't there)
template <class T> bool OrderedPointerList<T>::removeIfPresent(T *object)
{
    // Get item for specified object
    OrderedPointerListItem<T> *item = objectInList(object);
    if (item == nullptr)
        return false;
    remove(item);
    return true;
}

// Move specified item to target list
template <class T> void OrderedPointerList<T>::move(T *object, OrderedPointerList<T> &targetList)
{
    // Get item for specified object
    OrderedPointerListItem<T> *item = objectInList(object);

    // Add to target list, then delete from this list
    targetList.add(item->object());
    remove(item);
}

// Return the list head
template <class T> OrderedPointerListItem<T> *OrderedPointerList<T>::first() const { return listHead_; }

// Returns the list tail
template <class T> OrderedPointerListItem<T> *OrderedPointerList<T>::last() const { return listTail_; }

// Return whether the specified pointer is present in the list
template <class T> bool OrderedPointerList<T>::contains(T *object) const
{
    // TODO This can probably be made much faster - bisection?
    OrderedPointerListItem<T> *item = listHead_;
    while (item)
    {
        if (item->object() > object)
            return false;
        if (item->object() == object)
            return true;
        item = item->next;
    }
    return false;
}

// Create (or just return) the item array
template <class T> OrderedPointerListItem<T> **OrderedPointerList<T>::items()
{
    if (regenerateItemArray_ == 0)
        return items_;

    // Recreate item array if it is NULL or too small
    if ((items_ == nullptr) || (nItems_ > itemArraySize_))
    {
        // Delete old list if necessary
        if (items_ != nullptr)
            delete[] items_;

        // Create new list
        itemArraySize_ = nItems_;
        items_ = new OrderedPointerListItem<T> *[itemArraySize_];
    }

    // Fill in pointers
    int count = 0;
    for (OrderedPointerListItem<T> *item = listHead_; item != nullptr; item = item->next)
        items_[count++] = item;
    regenerateItemArray_ = 0;
    return items_;
}

// Create (or just return) the object array
template <class T> T **OrderedPointerList<T>::objects()
{
    if (regenerateObjectArray_ == 0)
        return objects_;

    // Recreate object array if it is NULL or too small
    if ((objects_ == nullptr) || (nItems_ > objectArraySize_))
    {
        // Delete old list if necessary
        if (objects_ != nullptr)
            delete[] objects_;

        // Create new list
        objectArraySize_ = nItems_;
        objects_ = new T *[objectArraySize_];
    }

    // Fill in pointers
    int count = 0;
    for (OrderedPointerListItem<T> *item = listHead_; item != nullptr; item = item->next)
        objects_[count++] = item->object();
    regenerateObjectArray_ = 0;
    return objects_;
}

// Invalidate current item and object arrays, forcing them to be recreated
template <class T> void OrderedPointerList<T>::invalidateLists()
{
    regenerateItemArray_ = 1;
    regenerateObjectArray_ = 1;
}

/*
 * Operators
 */

template <class T> void OrderedPointerList<T>::operator=(const OrderedPointerList<T> &other)
{
    clear();
    for (OrderedPointerListItem<T> *item = other.listHead_; item != nullptr; item = item->next)
        addAtEnd(item->object());
}

template <class T> OrderedPointerListItem<T> *OrderedPointerList<T>::operator[](int index)
{
#ifdef CHECKS
    if ((index < 0) || (index >= nItems_))
    {
        Messenger::error("LIST_OPERATOR[] - Array index ({}) out of bounds ({} items in List) >>>>\n", index, nItems_);
        return nullptr;
    }
#endif
    return items()[index];
}

/*
 * OrderedPointerListIterator
 */

// Ordered Pointer List Iterator
template <class T> class OrderedPointerListIterator
{
    public:
    OrderedPointerListIterator<T>(const OrderedPointerList<T> &source, bool reverse = false)
        : reverse_(reverse), targetList_(source)
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
    const OrderedPointerList<T> &targetList_;
    // Current item
    OrderedPointerListItem<T> *currentItem_;

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
            currentItem_ = reverse_ ? currentItem_->prev : currentItem_->next;

        // Check for end of list
        if (currentItem_ == nullptr)
            finished_ = true;

        return (currentItem_ ? currentItem_->object() : nullptr);
    }
    // Return current item index
    int currentIndex()
    {
        if (finished_)
            return 0;

        return (currentItem_ ? currentItem_->object()->index() : 0);
    }
    // Restart iteration
    void restart()
    {
        finished_ = false;
        currentItem_ = nullptr;
    }
};
