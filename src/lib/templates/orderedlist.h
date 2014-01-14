/*
	*** Ordered Reference List Class
	*** src/lib/templates/orderedlist.h
	Copyright T. Youngs 2012-2014

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_ORDEREDLIST_H
#define DUQ_ORDEREDLIST_H

#include <stdlib.h>
#include <stdio.h>

/*!
 * \brief OrderedListItem Class
 * \details OrderedList maintains a list of OrderedListItems, which contain a pointer to an object of the template class type.
 */
template <class T> class OrderedListItem
{
	public:
	// Constructor
	OrderedListItem<T>(T* object);
	// List pointers
	OrderedListItem<T>* prev, *next;

	private:
	// Reference to object
	T* object_;

	public:
	// Return reference to object
	T* object();
	// Return object index
	int objectIndex();
};

/*!
 * \brief Constructor
 * \details Constructor for OrderedListItem
 */
template <class T> OrderedListItem<T>::OrderedListItem(T* object) : object_(object)
{
	prev = NULL;
	next = NULL;
}

/*!
 * \brief Return reference to object
 */
template <class T> T* OrderedListItem<T>::object()
{
	return object_;
}

/*!
 * \brief Return object index
 */
template <class T> int OrderedListItem<T>::objectIndex()
{
#ifdef CHECKS
	if (object_ == NULL)
	{
		printf("NULL_POINTER - NULL object_ pointer encountered in OrderedListItem::objectIndex().\n");
		return -1;
	}
#endif
	return object_->index();
}

/*!
 * \brief OrderedList Class
 * \details Ordered, linked list.
*/
template <class T> class OrderedList
{
	public:
	// Constructor
	OrderedList<T>();
	// Destructor
	~OrderedList();


	/*!
	 * \name Item List
	 */
	///@{
	private:
	// Pointers to head and tail of list
	OrderedListItem<T>* listHead_, *listTail_;
	// Number of items in list
	int nItems_;
	// Static array of items
	OrderedListItem<T>** items_;
	// Static array of objects
	T** objects_;
	// Array sizes (at last new)
	int itemArraySize_, objectArraySize_;
	// Array regeneration flags
	bool regenerateItemArray_, regenerateObjectArray_;


	/*!
	 * \name Access / Manipulation
	 */
	private:
	// Insert item pointing to specified object, after specified item
	OrderedListItem<T>* insertAfter(T* object, OrderedListItem<T>* afterThis);
	// Insert item pointing to specified object, before specified item
	OrderedListItem<T>* insertBefore(T* object, OrderedListItem<T>* beforeThis);
	// Insert specified item, before specified item
	void insertBefore(OrderedListItem<T>* item, OrderedListItem<T>* beforeThis);
	// Remove an item from the list
	void remove(T *item);
	// Return whether the item is owned by the list
	OrderedListItem<T>* contains(int objectIndex) const;
	// Cut item from list
	void cut(OrderedListItem<T>* item);
	// Find and return the item with the next highest index to the index specified
	OrderedListItem<T>* nextHighestIndex(int objectIndex);

	public:
	// Clear the list
	void clear();
	// Returns the number of items referenced in the list
	int nItems() const;
	// Add a new item reference to the list
	void add(T* object);
	// Move specified item to target list
	void move(int objectIndex, OrderedList<T>& targetList);
	// Returns the list head
	OrderedListItem<T>* first() const;
	// Generate (if necessary) and return item array
	OrderedListItem<T>** items();
	// Generate (if necessary) and return object array
	T** objects();


	/*!
	 * \name Operators
	*/
	public:
	// Element access operator
	OrderedListItem<T>* operator[](int);
};

/*!
 * \brief Constructor
 * \details Constructor for List. 
 */
template <class T> OrderedList<T>::OrderedList()
{
	listHead_ = NULL;
	listTail_ = NULL;
	nItems_ = 0;
	regenerateItemArray_ = 1;
	regenerateObjectArray_ = 1;
	itemArraySize_ = 0;
	objectArraySize_ = 0;
	items_ = NULL;
	objects_ = NULL;
}

/*!
 * \brief Destructor
 * \details Destructor for List. 
 */
template <class T> OrderedList<T>::~OrderedList()
{
	clear();
}

/*
 * Access / Manipulation
 */

/*!
 * \brief Insert reference to specifed object after supplied item
 */
template <class T> OrderedListItem<T>* OrderedList<T>::insertAfter(T* object, OrderedListItem<T>* afterThis)
{
	// Create new list item
	OrderedListItem<T>* newItem = new OrderedListItem<T>(object);
	
	// Get pointer to next item in list, after the list item 'afterThis'
	// If 'afterThis' is NULL, then we insert at the start of the list (and make listHead_ point to the new item)
	OrderedListItem<T>* newNext;
	if (afterThis == NULL)
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
	if (newNext != NULL) newNext->prev = newItem;
	else listTail_ = newItem;

	// Re-point 'next' pointer of newItem;
	newItem->next = newNext;

	// Increase item count, and flag that the array should be regenerated.
	++nItems_;
	regenerateItemArray_ = 1;
	regenerateObjectArray_ = 1;
	
	return newItem;
}

/*!
 * \brief Insert reference to specifed object after supplied item
 */
template <class T> OrderedListItem<T>* OrderedList<T>::insertBefore(T* object, OrderedListItem<T>* beforeThis)
{
	// Create new list item
	OrderedListItem<T>* newItem = new OrderedListItem<T>(object);
	insertBefore(newItem, beforeThis);
	return newItem;
}

/*!
 * \brief Insert specified item, before specified item
 */
template <class T> void OrderedList<T>::insertBefore(OrderedListItem<T>* item, OrderedListItem<T>* beforeThis)
{
	// Get pointer to prev item in list, after the list item 'beforeThis'
	// If 'beforeThis' is NULL, then we insert at the end of the list (and make listTail_ point to the new item)
	OrderedListItem<T>* newPrev;
	if (beforeThis == NULL)
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
	if (newPrev != NULL) newPrev->next = item;
	else listHead_ = item;

	// Re-point 'prev' pointer of item;
	item->prev = newPrev;

	// Increase item count, and flag that the array should be regenerated.
	++nItems_;
	regenerateItemArray_ = 1;
	regenerateObjectArray_ = 1;
}

/*!
 * \brief Remove the specified item from the list
 */
template <class T> void OrderedList<T>::remove(T *xitem)
{
	if (xitem == NULL)
	{
		printf("Internal Error: NULL pointer passed to OrderedList<T>::remove().\n");
		return;
	}
	// Delete a specific item from the list
	xitem->prev == NULL ? listHead_ = xitem->next : xitem->prev->next = xitem->next;
	xitem->next == NULL ? listTail_ = xitem->prev : xitem->next->prev = xitem->prev;
	delete xitem;
	--nItems_;
	regenerateItemArray_ = 1;
	regenerateObjectArray_ = 1;
}

/*!
 * \brief Return whether item index is in the list
 */
template <class T> OrderedListItem<T>* OrderedList<T>::contains(int objectIndex) const
{
	// TODO This can probably be made much faster - bisection?
	OrderedListItem<T> *item = listHead_;
	while (item)
	{
		if (item->objectIndex() > objectIndex) return NULL;
		if (item->objectIndex() == objectIndex) return item;
		item = item->next;
	}
	return NULL;
}

/*!
 * \brief Cut - Bridge items over specified item
 */
template <class T> void OrderedList<T>::cut(OrderedListItem<T>* item)
{
	if (item == NULL)
	{
		printf("Internal Error: NULL pointer passed to OrderedList<T>::cut().\n");
		return;
	}

	// Grab list pointers for specified item
	OrderedListItem<T>* prev, *next;
	prev = item->prev;
	next = item->next;

	// Adjust previous item
	if (prev == NULL) listHead_ = next;
	else prev->next = next;

	// Adjust next item
	if (next == NULL) listTail_ = prev;
	else next->prev = prev;

	item->next = NULL;
	item->prev = NULL;

	--nItems_;
	regenerateItemArray_ = 1;
	regenerateObjectArray_ = 1;
}

/*!
 * \brief Find and return the item with the next highest index to the index specified
 */
template <class T> OrderedListItem<T>* OrderedList<T>::nextHighestIndex(int objectIndex)
{
	// TODO This can probably be made much faster - bisection?
	OrderedListItem<T> *item = listHead_;
	while (item)
	{
		if (item->objectIndex() > objectIndex) return item;
		item = item->next;
	}
	return NULL;
}

/*!
 * \brief Create (or just return) the item array
 */
template <class T> OrderedListItem<T>** OrderedList<T>::items()
{
	if (regenerateItemArray_ == 0) return items_;
	
	// Recreate item array if it is NULL or too small
	if ((items_ == NULL) || (nItems_ > itemArraySize_))
	{
		// Delete old list if necessary
		if (items_ != NULL) delete[] items_;
		
		// Create new list
		itemArraySize_ = nItems_;
		items_ = new OrderedListItem<T>*[itemArraySize_];
	}

	// Fill in pointers
	int count = 0;
	for (OrderedListItem<T>* item = listHead_; item != NULL; item = item->next) items_[count++] = item;
	regenerateItemArray_ = 0;
	return items_;
}

/*!
 * \brief Create (or just return) the object array
 */
template <class T> T** OrderedList<T>::objects()
{
	if (regenerateObjectArray_ == 0) return objects_;
	
	// Recreate object array if it is NULL or too small
	if ((objects_ == NULL) || (nItems_ > objectArraySize_))
	{
		// Delete old list if necessary
		if (objects_ != NULL) delete[] objects_;
		
		// Create new list
		objectArraySize_ = nItems_;
		objects_ = new T*[objectArraySize_];
	}

	// Fill in pointers
	int count = 0;
	for (OrderedListItem<T>* item = listHead_; item != NULL; item = item->next) objects_[count++] = item->object();
	regenerateObjectArray_ = 0;
	return objects_;
}


/*!
 * \brief Remove all items in the list
 */
template <class T> void OrderedList<T>::clear()
{
	OrderedListItem<T>* nextItem;
	for (OrderedListItem<T>* item = listHead_; item != NULL; item = item->next)
	{
		nextItem = item->next;
		delete item;
	}

	// Delete static items array if its there
	if (items_ != NULL) delete[] items_;
	items_ = NULL;
	regenerateItemArray_ = 1;
	regenerateObjectArray_ = 1;
}

/*!
 * \brief Return the number of items in the list
 */
template <class T> int OrderedList<T>::nItems() const
{
	return nItems_;
}

/*!
 * \brief Add item to list
 */
template <class T> void OrderedList<T>::add(T* object)
{
#ifdef CHECKS
	if (object == NULL)
	{
		printf("NULL_POINTER - NULL object passed to OrderedList<T>::add().\n");
		return;
	}
#endif
	// Add it in the correct place in the list
	OrderedListItem<T>* nextLargest = nextHighestIndex(object->index());
	insertBefore(object, nextLargest);
}

// Move specified item to target list
template <class T> void OrderedList<T>::move(int objectIndex, OrderedList<T>& targetList)
{
	// Get item for specified objectIndex
	OrderedListItem<T>* item = contains(objectIndex);
#ifdef CHECKS
	if (item == NULL)
	{
		printf("Internal Error: Specified objectIndex (%i) does not exist in this OrderedList.\n", objectIndex);
		return;
	}
#endif
	// Remove the item from this list
	cut(item);

	// Find next highest index item in targetList
	OrderedListItem<T>* nextHighest = targetList.nextHighestIndex(objectIndex);
	targetList.insertBefore(item, nextHighest);
}

/*!
 * \brief Return the list head
 */
template <class T> OrderedListItem<T>* OrderedList<T>::first() const
{
	return listHead_;
}

/*
// Operators
*/

/*!
 * \brief Element access operator
 */
template <class T> OrderedListItem<T> *OrderedList<T>::operator[](int index)
{
#ifdef CHECKS
	if ((index < 0) || (index >= nItems_))
	{
		printf("LIST_OPERATOR[] - Array index (%i) out of bounds (%i items in List) >>>>\n", index, nItems_);
		return NULL;
	}
#endif
	return items()[index];
}

#endif
