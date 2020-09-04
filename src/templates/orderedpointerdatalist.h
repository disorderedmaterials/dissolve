/*
    *** Ordered Pointer With Data List Class
    *** src/templates/orderedpointerdatalist.h
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

/*
 * OrderedPointerDataListItem Class
 */
template <class T, class D> class OrderedPointerDataListItem
{
    /*
     * OrderedPointerDataList maintains a list of OrderedPointerDataListItems, which contain a pointer to an object of the
     * template class type and an associated data item of thie second template class type. The objects in the list are
     * ordered according to their address in memory.
     */

    public:
    OrderedPointerDataListItem<T, D>(T *object = nullptr, D data = D()) : object_(object), data_(data)
    {
        prev = nullptr;
        next = nullptr;
    }
    // List pointers
    OrderedPointerDataListItem<T, D> *prev, *next;

    private:
    // Pointer to object
    T *object_;
    // Associated data
    D data_;

    public:
    // Set pointer to object
    void setObject(T *object) { object_ = object; }
    // Return pointer to object
    T *object() { return object_; }
    // Set data associated to object
    void setData(D data) { data_ = data; }
    // Return data associated to object
    D data() { return data_; }
};

/*
 * OrderedPointerDataList Class
 */
template <class T, class D> class OrderedPointerDataList
{
    /*
     * Ordered, linked list of pointers to objects which exist elsewhere.
     */

    public:
    OrderedPointerDataList<T, D>()
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
    ~OrderedPointerDataList() { clear(); }
    OrderedPointerDataList<T, D>(const OrderedPointerDataList<T, D> &source) { (*this) = source; }

    /*
     * Item List
     */
    private:
    // Object factory
    ObjectFactory<OrderedPointerDataListItem<T, D>> factory_;
    // Pointers to head and tail of list
    OrderedPointerDataListItem<T, D> *listHead_, *listTail_;
    // Number of items in list
    int nItems_;
    // Static array of items
    OrderedPointerDataListItem<T, D> **items_;
    // Static array of objects
    T **objects_;
    // Array sizes (at last new)
    int itemArraySize_, objectArraySize_;
    // Array regeneration flags
    bool regenerateItemArray_, regenerateObjectArray_;

    public:
    // Set chunksize to use in ObjectFactory
    void setChunkSize(int size) { factory_.setChunkSize(size); }

    /*
     * Access / Manipulation
     */
    private:
    // Insert item pointing to specified object, after specified item
    OrderedPointerDataListItem<T, D> *insertAfter(T *object, OrderedPointerDataListItem<T, D> *afterThis)
    {
        // Create new list item (from the ObjectFactory)
        OrderedPointerDataListItem<T, D> *newItem = factory_.produce();
        newItem->setObject(object);

        // Get pointer to next item in list, after the list item 'afterThis'
        // If 'afterThis' is nullptr, then we insert at the start of the list (and make listHead_ point to the new item)
        OrderedPointerDataListItem<T, D> *newNext;
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
    // Insert item pointing to specified object, before specified item
    OrderedPointerDataListItem<T, D> *insertBefore(T *object, OrderedPointerDataListItem<T, D> *beforeThis)
    {
        // Create new list item (from the ObjectFactory)
        OrderedPointerDataListItem<T, D> *newItem = factory_.produce();
        newItem->setObject(object);
        insertBefore(newItem, beforeThis);
        return newItem;
    }
    // Insert specified item, before specified item
    void insertBefore(OrderedPointerDataListItem<T, D> *item, OrderedPointerDataListItem<T, D> *beforeThis)
    {
        // Get pointer to prev item in list, after the list item 'beforeThis'
        // If 'beforeThis' is nullptr, then we insert at the end of the list (and make listTail_ point to the new item)
        OrderedPointerDataListItem<T, D> *newPrev;
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
    // Remove an item from the list
    void remove(OrderedPointerDataListItem<T, D> *xitem)
    {
        if (xitem == nullptr)
        {
            Messenger::error("NULL pointer passed to OrderedPointerDataList<T,D>::remove().\n");
            return;
        }
        // Delete a specific item from the list
        xitem->prev == NULL ? listHead_ = xitem->next : xitem->prev->next = xitem->next;
        xitem->next == NULL ? listTail_ = xitem->prev : xitem->next->prev = xitem->prev;
        factory_.returnObject(xitem);
        --nItems_;
        regenerateItemArray_ = 1;
        regenerateObjectArray_ = 1;
    }
    // Return whether the item is owned by the list
    OrderedPointerDataListItem<T, D> *contains(T *object) const
    {
        // TODO This can probably be made much faster - bisection?
        OrderedPointerDataListItem<T, D> *item = listHead_;
        while (item)
        {
            if (item > object)
                return nullptr;
            if (item == object)
                return item;
            item = item->next;
        }
        return nullptr;
    }
    // Cut item from list
    void cut(OrderedPointerDataListItem<T, D> *item)
    {
        if (item == nullptr)
        {
            Messenger::error("NULL pointer passed to OrderedPointerDataList<T,D>::cut().\n");
            return;
        }

        // Grab list pointers for specified item
        OrderedPointerDataListItem<T, D> *prev, *next;
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
    // Find and return the item with the next highest pointer after the object specified
    OrderedPointerDataListItem<T, D> *nextHighest(T *object)
    {
        // TODO This can probably be made much faster - binary chop?
        OrderedPointerDataListItem<T, D> *item = listHead_;
        while (item)
        {
            if (item > object)
                return item;
            item = item->next;
        }
        return nullptr;
    }

    public:
    // Clear the list
    void clear()
    {
        for (OrderedPointerDataListItem<T, D> *item = listHead_; item != nullptr; item = item->next)
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
    // Returns the number of items referenced in the list
    int nItems() const { return nItems_; }
    // Add a new item reference to the list
    void add(T *object, D data = D())
    {
#ifdef CHECKS
        if (object == nullptr)
        {
            Messenger::error("NULL_POINTER - nullptr passed to OrderedPointerDataList<T,D>::add().\n");
            return;
        }
#endif
        // Add it in the correct place in the list
        OrderedPointerDataListItem<T, D> *nextLargest = nextHighest(object);
        insertBefore(object, nextLargest)->setData(data);
    }
    // Add a new item reference to the list, unless it is already there
    void addExclusive(T *object, D data = D())
    {
#ifdef CHECKS
        if (object == nullptr)
        {
            Messenger::error("NULL_POINTER - nullptr passed to OrderedPointerDataList<T,D>::add().\n");
            return;
        }
#endif
        // Seek the next highest object, checking to see if we find the specified index
        // TODO This can be made much faster - binary chop?
        OrderedPointerDataListItem<T, D> *nextLargest = listHead_;
        while (nextLargest)
        {
            if (nextLargest->object() > object)
                break;
            else if (nextLargest->object() == object)
                return;
            nextLargest = nextLargest->next;
        }

        insertBefore(object, nextLargest)->setData(data);
    }
    // Add a new item reference to the end of the list (with checks)
    void addAtEnd(T *object, D data = D())
    {
#ifdef CHECKS
        if (object == nullptr)
        {
            Messenger::error("NULL_POINTER - nullptr passed to OrderedPointerDataList<T,D>::addAtEnd().\n");
            return;
        }
#endif
        // Add it directly to the end of the list, provided this adheres to the current order
        // Check object index of last item in list
        if (listTail_ == nullptr)
            insertAfter(object, nullptr);
        else if (listTail_->object() < object)
            insertAfter(object, listTail_)->setData(data);
    }
    // Remove item reference from list
    void remove(T *object)
    {
        // Get item for specified objectIndex
        OrderedPointerDataListItem<T, D> *item = contains(object);
        if (item == nullptr)
            return;

        remove(item);
    }
    // Remove item reference from the list (but don't complain if it isn't there)
    bool removeIfPresent(T *object)
    {
        // Get item for specified objectIndex
        OrderedPointerDataListItem<T, D> *item = contains(object);
        if (item == nullptr)
            return false;
        remove(item);
        return true;
    }
    // Move specified item to target list
    void move(T *object, OrderedPointerDataList<T, D> &targetList)
    {
        // Get item for specified objectIndex
        OrderedPointerDataListItem<T, D> *item = contains(object);
        if (item == nullptr)
            return;

        // Add to target list, then delete from this list
        targetList.add(item->object());
        remove(item);
    }
    // Returns the list head
    OrderedPointerDataListItem<T, D> *first() const { return listHead_; }
    // Returns the list tail
    OrderedPointerDataListItem<T, D> *last() const { return listTail_; }
    // Generate (if necessary) and return item array
    OrderedPointerDataListItem<T, D> **items()
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
            items_ = new OrderedPointerDataListItem<T, D> *[itemArraySize_];
        }

        // Fill in pointers
        int count = 0;
        for (OrderedPointerDataListItem<T, D> *item = listHead_; item != nullptr; item = item->next)
            items_[count++] = item;
        regenerateItemArray_ = 0;
        return items_;
    }
    // Generate (if necessary) and return object array
    T **objects()
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
        for (OrderedPointerDataListItem<T, D> *item = listHead_; item != nullptr; item = item->next)
            objects_[count++] = item->object();
        regenerateObjectArray_ = 0;
        return objects_;
    }
    // Invalidate current item and object arrays, forcing them to be recreated
    void invalidateLists()
    {
        regenerateItemArray_ = 1;
        regenerateObjectArray_ = 1;
    }
    // Given a second list, generate new lists containing unique items to each list, and those that appear in both
    void difference(OrderedPointerDataList<T, D> &listB, OrderedPointerDataList<T, D> &uniqueToA,
                    OrderedPointerDataList<T, D> &uniqueToB, OrderedPointerDataList<T, D> &commonItems)
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
     * Operators
     */
    public:
    void operator=(const OrderedPointerDataList<T, D> &other)
    {
        clear();
        for (OrderedPointerDataListItem<T, D> *item = other.listHead_; item != nullptr; item = item->next)
            addAtEnd(item->object());
    }
    OrderedPointerDataListItem<T, D> *operator[](int index)
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
};

/*
 * OrderedPointerDataListIterator
 */
template <class T, class D> class OrderedPointerDataListIterator
{
    public:
    OrderedPointerDataListIterator<T, D>(const OrderedPointerDataList<T, D> &source, bool reverse = false)
        : targetList_(source), reverse_(reverse)
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
    const OrderedPointerDataList<T, D> &targetList_;
    // Current item
    OrderedPointerDataListItem<T, D> *currentItem_;

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
    int currentIndex() { return (currentItem_ ? currentItem_->object()->index() : 0); }
    // Return current item data
    D currentData() { return (currentItem_ ? currentItem_->data() : D()); }
    // Restart iteration
    void restart()
    {
        finished_ = false;
        currentItem_ = nullptr;
    }
};
