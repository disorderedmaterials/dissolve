/*
    *** Ordered List Class
    *** src/templates/orderedlist.h
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

// OrderedListItem Class
template <class T> class OrderedListItem
{
    public:
    OrderedListItem<T>();
    // List pointers
    OrderedListItem<T> *prev, *next;

    private:
    // Instance of the object
    T object_;

    public:
    // Return reference to object
    T &object();
    // Return object index
    int objectIndex() const;
};

template <class T> OrderedListItem<T>::OrderedListItem()
{
    prev = nullptr;
    next = nullptr;
}

// Return reference to object
template <class T> T &OrderedListItem<T>::object() { return object_; }

// Return object index
template <class T> int OrderedListItem<T>::objectIndex() const { return object_.index(); }

// OrderedList Class
template <class T> class OrderedList
{
    /*
     * Ordered, linked list of OrderedListItems, which contain an instance of the template class T.
     * The class T must provide an 'int T::index() const' function, as well as operator=.
     */
    public:
    OrderedList<T>();
    ~OrderedList();
    OrderedList<T>(const OrderedList<T> &source);

    /*
     * Item List
     */
    private:
    // Object factory
    ObjectFactory<OrderedListItem<T>> factory_;
    // Pointers to head and tail of list
    OrderedListItem<T> *listHead_, *listTail_;
    // Number of items in list
    int nItems_;
    // Static array of items
    OrderedListItem<T> **items_;
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
    OrderedListItem<T> *insertAfter(T &object, OrderedListItem<T> *afterThis);
    // Insert item pointing to specified object, before specified item
    OrderedListItem<T> *insertBefore(T &object, OrderedListItem<T> *beforeThis);
    // Insert specified item, before specified item
    void insertBefore(OrderedListItem<T> *item, OrderedListItem<T> *beforeThis);
    // Remove an item from the list
    void remove(OrderedListItem<T> *xitem);
    // Return whether the item is owned by the list
    OrderedListItem<T> *contains(int objectIndex) const;
    // Cut item from list
    void cut(OrderedListItem<T> *item);
    // Find and return the item with the next highest index to the index specified
    OrderedListItem<T> *nextHighestIndex(int objectIndex);

    public:
    // Clear the list
    void clear();
    // Returns the number of items referenced in the list
    int nItems() const;
    // Add a new item reference to the list
    T *add(T object);
    // Add a new item reference to the end of the list
    void addAtEnd(T object);
    // Remove item reference from list
    void remove(int objectIndex);
    // Remove item reference from the list (but don't complain if it isn't there)
    bool removeIfPresent(int objectIndex);
    // Move specified item to target list
    void move(int objectIndex, OrderedList<T> &targetList);
    // Return object with index specified (if it exists)
    T *objectWithIndex(int objectIndex);
    // Returns the list head
    OrderedListItem<T> *first() const;
    // Generate (if necessary) and return item array
    OrderedListItem<T> **items();
    // Generate (if necessary) and return object array
    T **objects();
    // Invalidate current item and object arrays, forcing them to be recreated
    void invalidateLists();
    // Given a second list, generate new lists containing unique items to each list, and those that appear in both
    void difference(OrderedList<T> &listB, OrderedList<T> &uniqueToA, OrderedList<T> &uniqueToB, OrderedList<T> &commonItems);

    /*
     * Operators
     */
    public:
    void operator=(const OrderedList<T> &other);
    OrderedListItem<T> *operator[](int);
};

template <class T> OrderedList<T>::OrderedList()
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

template <class T> OrderedList<T>::~OrderedList() { clear(); }

template <class T> OrderedList<T>::OrderedList(const OrderedList<T> &source) { (*this) = source; }

/*
 * Access / Manipulation
 */

// Insert reference to specifed object after supplied item
template <class T> OrderedListItem<T> *OrderedList<T>::insertAfter(T &object, OrderedListItem<T> *afterThis)
{
    // Create new list item (from the ObjectFactory)
    OrderedListItem<T> *newItem = factory_.produce();
    newItem->setObject(object);

    // Get pointer to next item in list, after the list item 'afterThis'
    // If 'afterThis' is nullptr, then we insert at the start of the list (and make listHead_ point to the new item)
    OrderedListItem<T> *newNext;
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
template <class T> OrderedListItem<T> *OrderedList<T>::insertBefore(T &object, OrderedListItem<T> *beforeThis)
{
    // Create new list item (from the ObjectFactory)
    OrderedListItem<T> *newItem = factory_.produce();
    newItem->object() = object;
    insertBefore(newItem, beforeThis);
    return newItem;
}

// Insert specified item, before specified item
template <class T> void OrderedList<T>::insertBefore(OrderedListItem<T> *item, OrderedListItem<T> *beforeThis)
{
    // Get pointer to prev item in list, after the list item 'beforeThis'
    // If 'beforeThis' is nullptr, then we insert at the end of the list (and make listTail_ point to the new item)
    OrderedListItem<T> *newPrev;
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
template <class T> void OrderedList<T>::remove(OrderedListItem<T> *xitem)
{
    if (xitem == nullptr)
    {
        Messenger::error("NULL pointer passed to OrderedList<T>::remove().\n");
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

// Return whether item index is in the list
template <class T> OrderedListItem<T> *OrderedList<T>::contains(int objectIndex) const
{
    // TODO This can probably be made much faster - bisection?
    OrderedListItem<T> *item = listHead_;
    while (item)
    {
        if (item->objectIndex() > objectIndex)
            return nullptr;
        if (item->objectIndex() == objectIndex)
            return item;
        item = item->next;
    }
    return nullptr;
}

// Cut - Bridge items over specified item
template <class T> void OrderedList<T>::cut(OrderedListItem<T> *item)
{
    if (item == nullptr)
    {
        Messenger::error("NULL pointer passed to OrderedList<T>::cut().\n");
        return;
    }

    // Grab list pointers for specified item
    OrderedListItem<T> *prev, *next;
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
template <class T> OrderedListItem<T> *OrderedList<T>::nextHighestIndex(int objectIndex)
{
    // TODO This can probably be made much faster - bisection?
    OrderedListItem<T> *item = listHead_;
    while (item)
    {
        if (item->objectIndex() > objectIndex)
            return item;
        item = item->next;
    }
    return nullptr;
}

// Remove all items in the list
template <class T> void OrderedList<T>::clear()
{
    for (OrderedListItem<T> *item = listHead_; item != nullptr; item = item->next())
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
template <class T> int OrderedList<T>::nItems() const { return nItems_; }

// Add item to list
template <class T> T *OrderedList<T>::add(T object)
{
    // Add it in the correct place in the list
    OrderedListItem<T> *nextLargest = nextHighestIndex(object.index());
    OrderedListItem<T> *newObject = insertBefore(object, nextLargest);
    return &newObject->object();
}

// Add a new item reference to the end of the list
template <class T> void OrderedList<T>::addAtEnd(T object)
{
    // Add it directly to the end of the list, provided this adheres to the current order
    // Check object index of last item in list
    if (listTail_ == nullptr)
        insertAfter(object, nullptr);
    else if (listTail_->objectIndex() < object->index())
        insertAfter(object, listTail_);
    else
        Messenger::error("BAD_USAGE - Attempted to add object with index {} to end of OrderedList, but last item in list has "
                         "index {}\n",
                         object->index(), listTail_->objectIndex());
}

// Remove item reference from list
template <class T> void OrderedList<T>::remove(int objectIndex)
{
    // Get item for specified objectIndex
    OrderedListItem<T> *item = contains(objectIndex);
#ifdef CHECKS
    if (item == nullptr)
    {
        Messenger::error("Specified objectIndex ({}) does not exist in this OrderedList.\n", objectIndex);
        return;
    }
#endif
    remove(item);
}

// Remove item reference from the list (but don't complain if it isn't there)
template <class T> bool OrderedList<T>::removeIfPresent(int objectIndex)
{
    // Get item for specified objectIndex
    OrderedListItem<T> *item = contains(objectIndex);
    if (item == nullptr)
        return false;
    remove(item);
    return true;
}

// Move specified item to target list
template <class T> void OrderedList<T>::move(int objectIndex, OrderedList<T> &targetList)
{
    // Get item for specified objectIndex
    OrderedListItem<T> *item = contains(objectIndex);
#ifdef CHECKS
    if (item == nullptr)
    {
        Messenger::error("Specified objectIndex ({}) does not exist in this OrderedList.\n", objectIndex);
        return;
    }
#endif
    // Add to target list, then delete from this list
    targetList.add(item->object());
    remove(item);
}

// Return object with index specified (if it exists)
template <class T> T *OrderedList<T>::objectWithIndex(int objectIndex)
{
    OrderedListItem<T> *item = contains(objectIndex);
    return (item == nullptr ? nullptr : &item->object());
}

// Return the list head
template <class T> OrderedListItem<T> *OrderedList<T>::first() const { return listHead_; }

// Create (or just return) the item array
template <class T> OrderedListItem<T> **OrderedList<T>::items()
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
        items_ = new OrderedListItem<T> *[itemArraySize_];
    }

    // Fill in pointers
    int count = 0;
    for (OrderedListItem<T> *item = listHead_; item != nullptr; item = item->next())
        items_[count++] = item;
    regenerateItemArray_ = 0;
    return items_;
}

// Create (or just return) the object array
template <class T> T **OrderedList<T>::objects()
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
    for (OrderedListItem<T> *item = listHead_; item != nullptr; item = item->next())
        objects_[count++] = &item->object();
    regenerateObjectArray_ = 0;
    return objects_;
}

// Invalidate current item and object arrays, forcing them to be recreated
template <class T> void OrderedList<T>::invalidateLists()
{
    regenerateItemArray_ = 1;
    regenerateObjectArray_ = 1;
}

// Given a second list, generate new lists containing unique items to each list, and those that appear in both
template <class T>
void OrderedList<T>::difference(OrderedList<T> &listB, OrderedList<T> &uniqueToA, OrderedList<T> &uniqueToB,
                                OrderedList<T> &commonItems)
{
    // Clear supplied results lists
    uniqueToA.clear();
    uniqueToB.clear();
    commonItems.clear();

    // Check here for either list being empty
    if (nItems_ == 0)
    {
        // Add all items in listB to uniqueToB, and we're done
        uniqueToB = listB;
        return;
    }
    if (listB.nItems_ == 0)
    {
        // Add all items in this_ list to uniqueToA, and we're done
        uniqueToA = (*this);
        return;
    }

    // Traverse the lists simultaneously, comparing indices at each turn
    int indexA = 0, indexB = 0;
    T **itemsA = objects(), **itemsB = listB.objects();
    int objectIndexA = itemsA[indexA]->objectIndex(), objectIndexB = itemsB[indexB]->objectIndex();
    while ((indexA < nItems_) && (indexB < listB.nItems_))
    {
        // If objectAndexA is less than objectIndexB, then the item in this_ list at indexA is unique
        if (objectIndexA < objectIndexB)
        {
            uniqueToA.addAtEnd(itemsA[indexA]);
            ++indexA;
            objectIndexA = itemsA[indexA]->objectIndex();
            continue;
        }

        // If indexB is less than indexA, then the item in listB at indexB is unique
        if (objectIndexB < objectIndexA)
        {
            uniqueToB.addAtEnd(itemsB[indexB]);
            ++indexB;
            objectIndexB = itemsB[indexB]->objectIndex();
            continue;
        }

        // The indices are the same, so this is common element to both lists
        commonItems.addAtEnd(itemsA[indexA]);
        ++indexA;
        objectIndexA = itemsA[indexA]->objectIndex();
        ++indexB;
        objectIndexB = itemsB[indexB]->objectIndex();
    }

    // If we have not yet gone through all the items in either list, add them to the relevant unique results list
    for (int n = indexA; n < nItems_; ++n)
        uniqueToA.addAtEnd(itemsA[n]);
    for (int n = indexB; n < listB.nItems_; ++n)
        uniqueToB.addAtEnd(itemsB[n]);
}

/*
// Operators
*/

template <class T> void OrderedList<T>::operator=(const OrderedList<T> &other)
{
    clear();
    for (OrderedListItem<T> *item = other.listHead_; item != nullptr; item = item->next())
        addAtEnd(item->object());
}

template <class T> OrderedListItem<T> *OrderedList<T>::operator[](int index)
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
