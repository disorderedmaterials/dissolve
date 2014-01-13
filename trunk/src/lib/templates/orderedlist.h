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
 * \details OrderedList maintains a list of OrderedListItems, which contain a reference to an object of the template class type.
 */
template <class T> class OrderedListItem
{
	public:
	// Constructor
	OrderedListItem<T>(T& object);
	// List pointers
	OrderedListItem<T>* prev, *next;

	private:
	// Reference to object
	T& object_;

	public:
	// Return reference to object
	T& object();
	// Return object index
	int objectIndex();
};

/*!
 * \brief Constructor
 * \details Constructor for OrderedListItem
 */
template <class T> OrderedListItem<T>::OrderedListItem(T& object) : object_(object)
{
	prev = NULL;
	next = NULL;
}

/*!
 * \brief Return reference to object
 */
template <class T> T& OrderedListItem<T>::object()
{
	return object_;
}

/*!
 * \brief Return object index
 */
template <class T> int OrderedListItem<T>::objectIndex()
{
	return object_.index();
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
	OrderedListItem<T> *listHead_, *listTail_;
	// Number of items in list
	int nItems_;
	// Static array of items
	OrderedListItem<T> *items_;
	// Array regeneration flag
	bool regenerate_;


	/*!
	 * \name Access / Manipulation
	 */
	private:
	// Insert reference to specified object after specified item
	OrderedListItem<T>* insertAfter(T& object, OrderedListItem<T>* afterThis);
	// Insert reference to specified object after specified item
	OrderedListItem<T>* insertBefore(T& object, OrderedListItem<T>* beforeThis);
	// Add the item into this list
	void own(T *item);
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
	void add(T& object);
	// Move specified item to target list
	void move(int objectIndex, OrderedList<T>& targetList);
	// Returns the list head
	OrderedListItem<T>* first() const;
	// Generate (if necessary) and return item array
	OrderedListItem<T> *array();


	/*!
	 * \name Operators
	*/
	public:
	// Element access operator
	OrderedListItem<T> *operator[](int);
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
	regenerate_ = 1;
	items_ = NULL;
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
template <class T> OrderedListItem<T>* OrderedList<T>::insertAfter(T& object, OrderedListItem<T>* afterThis)
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
	regenerate_ = 1;
	
	return newItem;
}

/*!
 * \brief Insert reference to specifed object after supplied item
 */
template <class T> OrderedListItem<T>* OrderedList<T>::insertBefore(T& object, OrderedListItem< T >* beforeThis)
{
	// Create new list item
	OrderedListItem<T>* newItem = new OrderedListItem<T>(object);
	
	// Get pointer to prev item in list, after the list item 'afterThis'
	// If 'afterThis' is NULL, then we insert at the end of the list (and make listTail_ point to the new item)
	OrderedListItem<T>* newPrev;
	if (beforeThis == NULL)
	{
		// First item in list will be the newItem, so 'newPrev will be the current listTail_
		newPrev = listTail_;
		listTail_ = newItem;
	}
	else
	{
		newPrev = beforeThis->prev;
		beforeThis->prev = newItem;
	}

	// Set pointer to next item (of our newItem)
	newItem->next = beforeThis;

	// Re-point nextious pointer of newPrev
	if (newPrev != NULL) newPrev->next = newItem;
	else listHead_ = newItem;

	// Re-point 'prev' pointer of newItem;
	newItem->prev = newPrev;

	// Increase item count, and flag that the array should be regenerated.
	++nItems_;
	regenerate_ = 1;
	
	return newItem;
}

/*!
 * \brief Own an existing item
 */
template <class T> void OrderedList<T>::own(T* olditem)
{
	if (olditem == NULL)
	{
		printf("Internal Error: NULL pointer passed to OrderedList<T>::own().\n");
		return;
	}
	// In the interests of 'pointer etiquette', refuse to own the item if its pointers are not NULL
	if ((olditem->next != NULL) || (olditem->prev != NULL))
	{
		printf("list::own <<<< List refused to own an item that still had ties >>>>\n");
		return;
	}
	listHead_ == NULL ? listHead_ = olditem : listTail_->next = olditem;
	olditem->prev = listTail_;
	olditem->next = NULL;
	listTail_ = olditem;
	nItems_ ++;
	regenerate_ = 1;
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
	regenerate_ = 1;
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
		if (item->index() == objectIndex) return item;
		if (item->index() > objectIndex) return NULL;
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
	T *prev, *next;
	prev = item->prev;
	next = item->next;
	if (prev == NULL) listHead_ = next;
	else prev->next = next;
	if (next == NULL) listTail_ = prev;
	else next->prev = prev;
	item->next = NULL;
	item->prev = NULL;
	regenerate_ = 1;
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
template <class T> OrderedListItem<T>* OrderedList<T>::array()
{
	if (regenerate_ == 0) return items_;
	
	// Delete old item list if it is too small (and if there is one)
	if (items_ != NULL) delete[] items_;
	
	// Create new list
	items_ = new OrderedListItem<T>*[nItems_];
	
	// Fill in pointers
	int count = 0;
	for (OrderedListItem<T>* i = listHead_; i != NULL; i = i->next) items_[count++] = i;
	regenerate_ = 0;
	return items_;
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
	regenerate_ = 1;
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
template <class T> void OrderedList<T>::add(T& object)
{
	// Add it in the correct place in the list
	OrderedListItem<T>* nextLargest = nextHighestIndex(object.index());
	insertBefore(object, nextLargest);
}

// Move specified item to target list
template <class T> void OrderedList<T>::move(int objectIndex, OrderedList<T>& targetList)
{
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
	return array()[index];
}

#endif
