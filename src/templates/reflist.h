/*
	*** Linked List Reference Class
	*** src/templates/RefList.h
	Copyright T. Youngs 2012-2016

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

#ifndef DUQ_REFLIST_H
#define DUQ_REFLIST_H

#include "templates/list.h"
#include <stddef.h>
#include <stdio.h>

/*
 * \brief Linked List Reference Item Class
 * \details Linked list reference item, allowing storage of pointers to user-defined classes without disrupting their internal ListItem pointers. Subclassing ListItem is not necessary in order to place objects in a RefList.
*/
template <class T, class D> class RefListItem
{
	public:
	// Constructor
	RefListItem<T,D>();

	// List pointers
	RefListItem<T,D>* prev, *next;
	// Pointer to item
	T* item;
	// Additional temporary info stored in structure
	D data;
};

/*
 * \brief Constructor
 * \details Constructor for RefListItem.
 */
template <class T, class D> RefListItem<T,D>::RefListItem()
{
	item = NULL;
	next = NULL;
	prev = NULL;
}

/*
 * \brief Linked List Reference Class
 * \details Linked list of references for/to user-defined classes. Subclassing ListItem or RefListItem is not necessary in order to place objects in a RefList.
*/
template <class T, class D> class RefList
{
	public:
	// Constructor
	RefList<T,D>();
	// List Constructor
	RefList<T,D>(const List<T>& source, D startData);
	// Destructor
	~RefList();
	// Copy Constructor
	RefList<T,D>(const RefList<T,D>& source);

	/*
	// Reference List of Items
	*/
	private:
	// Head and tail of reference items
	RefListItem<T,D>* listHead_, *listTail_;
	// Number of items in list
	int nItems_;
	// Static array of items
	RefListItem<T,D>* *items_;
	// Array regeneration flag
	bool regenerate_;

	public:
	// Returns the head of the atom list
	RefListItem<T,D>* first() const;
	// Returns the last item in the list
	RefListItem<T,D>* last() const;
	// Returns the number of atoms in the list
	int nItems() const;
	// Add reference to the list
	RefListItem<T,D>* add(T* item);
	// Add reference to the list with extra data
	RefListItem<T,D>* add(T* item, D extradata);
	// Add reference to the beginning of the list
	RefListItem<T,D>* addStart(T* item);
	// Add reference after the specified item
	RefListItem<T,D>* addAfter(RefListItem<T,D>* target, T* item);
	// Add reference before the specified item
	RefListItem<T,D>* addBefore(RefListItem<T,D>* target, T* item);
	// Add reference to the beginning of the list with extra data
	RefListItem<T,D>* addStart(T* item, D extradata);
	// Add reference to list, unless already there
	RefListItem<T,D>* addUnique(T* item);
	// Add reference to list, unless already there
	RefListItem<T,D>* addUnique(T* , D extradata);
	// Cut item from list (orphan it)
	void cut(RefListItem<T,D>* item);
	// Add an orphaned item into this list
	void own(RefListItem<T,D>* item);
	// Delete the reference from the list
	void remove(RefListItem<T,D>* item);
	// Delete the reference containing specified item from the list
	void remove(T* item);
	// Remove the first item in the list
	void removeFirst();
	// Remove the last item in the list
	void removeLast();
	// Operator =
	void operator=(RefList<T,D> &source);
	// Element access operator
	RefListItem<T,D>* operator[](int);
	// Item access operator
	RefListItem<T,D>* item(int n) const;
	// Search references for item
	RefListItem<T,D>* contains(T* item) const;
	// Search references for data
	RefListItem<T,D>* containsData(D data) const;
	// Clear the list of all references
	void clear();
	// Prune items with specified data
	void prune(D data);
	// Move head of list to tail of list
	void moveHeadToTail();
	// Create from List
	void createFromList(T* listhead, D startData);
	// Create from RefList
	void createFromRefList(RefListItem<T,D>* listhead);
	// Fills the supplied array with 'n' pointer values to the reference items
	void fillArray(int n, T* data);
	// Swap the two items specified
	void swap(T* item1, T* item2);
	// Return array of items
	RefListItem<T,D>* *array();
};

/*
 * \brief Constructor
 * \details Constructor for RefList.
 */
template <class T, class D> RefList<T,D>::RefList()
{
	listHead_ = NULL;
	listTail_ = NULL;
	items_ = NULL;
	regenerate_ = 1;
	nItems_ = 0;
}

/*
 * \brief List Constructor
 */
template <class T, class D> RefList<T,D>::RefList(const List<T>& source, D startData)
{
	listHead_ = NULL;
	listTail_ = NULL;
	items_ = NULL;
	regenerate_ = 1;
	nItems_ = 0;
	createFromList(source.first(), startData);
}

/*
 * \brief Destructor
 * \details Destructor for RefList.
 */
template <class T, class D> RefList<T,D>::~RefList()
{
	clear();
}

/*
 * \brief Copy Constructur
 * \details Copy Constructor for RefList.
 */
template <class T, class D> RefList<T,D>::RefList(const RefList<T,D> &source)
{
	listHead_ = NULL;
	listTail_ = NULL;
	items_ = NULL;
	regenerate_ = 1;
	nItems_ = 0;
	for (RefListItem<T,D>* ri = source.first(); ri != NULL; ri = ri->next) add(ri->item, ri->data);
}

/*
 * \brief Assignment operator =
 */
template <class T, class D> void RefList<T,D>::operator=(RefList<T,D> &source)
{
	// Clear any current data...
	clear();
	for (RefListItem<T,D>* ri = source.first(); ri != NULL; ri = ri->next) add(ri->item, ri->data);
}

/*
 * \brief Returns the head of the atom list
 */
template <class T, class D> RefListItem<T,D>* RefList<T,D>::first() const
{
	return listHead_;
}

/*
 * \brief Returns the last item in the list
 */
template <class T, class D> RefListItem<T,D>* RefList<T,D>::last() const
{
	return listTail_;
}

/*
 * \brief Returns the number of atoms in the list
 */
template <class T, class D> int RefList<T,D>::nItems() const
{
	return nItems_;
}

/*
 * \brief Add item to list
 */
template <class T, class D> RefListItem<T,D>* RefList<T,D>::add(T* item)
{
	RefListItem<T,D>* newitem = new RefListItem<T,D>;
	// Add the pointer to the list
	listHead_ == NULL ? listHead_ = newitem : listTail_->next = newitem;
	newitem->prev = listTail_;
	listTail_ = newitem;
	newitem->item = item;
	nItems_ ++;
	regenerate_ = 1;
	return newitem;
}

/*
 * \brief Add item to list with extra data
 */
template <class T, class D> RefListItem<T,D>* RefList<T,D>::add(T* item, D extradata)
{
	RefListItem<T,D>* newitem = new RefListItem<T,D>;
	// Add the pointer to the list
	listHead_ == NULL ? listHead_ = newitem : listTail_->next = newitem;
	newitem->prev = listTail_;
	listTail_ = newitem;
	newitem->item = item;
	newitem->data = extradata;
	nItems_ ++;
	regenerate_ = 1;
	return newitem;
}

/*
 * \brief Add item to start of list
 */
template <class T, class D> RefListItem<T,D>* RefList<T,D>::addStart(T* item)
{
	RefListItem<T,D>* newitem = new RefListItem<T,D>;
	// Add the pointer to the beginning of the list
	newitem->next = listHead_;
	if (listHead_ != NULL) listHead_->prev = newitem;
	listHead_ = newitem;
	if (listTail_ == NULL) listTail_ = newitem;
	newitem->item = item;
	nItems_ ++;
	regenerate_ = 1;
	return newitem;
}

/*
 * \brief Add reference after the specified item
 */
template <class T, class D> RefListItem<T,D>* RefList<T,D>::addAfter(RefListItem<T,D>* target, T* item)
{
	if (target == NULL) return add(item);
	else
	{
		RefListItem<T,D>* newitem = new RefListItem<T,D>;
		newitem->prev = target;
		newitem->next = target->next;
		if (target->next != NULL) target->next->prev = newitem;
		target->next = newitem;
		if (target == listTail_) listTail_ = newitem;
		newitem->item = item;
		nItems_ ++;
		regenerate_ = 1;
		return newitem;
	}
}

/*
 * \brief Add reference before the specified item
 */
template <class T, class D> RefListItem<T,D>* RefList<T,D>::addBefore(RefListItem<T,D>* target, T* item)
{
	if (target == NULL) return add(item);
	else
	{
		RefListItem<T,D>* newitem = new RefListItem<T,D>;
		newitem->next = target;
		newitem->prev = target->prev;
		if (target->prev != NULL) target->prev->next = newitem;
		target->prev = newitem;
		if (target == listHead_) listHead_ = newitem;
		newitem->item = item;
		nItems_ ++;
		regenerate_ = 1;
		return newitem;
	}
}

/*
 * \brief Add item to start of list with extra data
 */
template <class T, class D> RefListItem<T,D>* RefList<T,D>::addStart(T* item, D extradata)
{
	RefListItem<T,D>* newitem = new RefListItem<T,D>;
	// Add the pointer to the beginning of the list
	newitem->next = listHead_;
	listHead_ == NULL ? listHead_ = newitem : listHead_->prev = newitem;
	listHead_ = newitem;
	newitem->item = item;
	newitem->data = extradata;
	nItems_ ++;
	regenerate_ = 1;
	return newitem;
}

/*
 * \brief Add unique item to list
 */
template <class T, class D> RefListItem<T,D>* RefList<T,D>::addUnique(T* item)
{
	RefListItem<T,D>* srch = contains(item);
	if (srch == NULL) return add(item);
	else return srch;
}

/*
 * \brief Add unique item to list
 */
template <class T, class D> RefListItem<T,D>* RefList<T,D>::addUnique(T* item, D extradata)
{
	RefListItem<T,D>* srch = contains(item);
	if (srch == NULL) return add(item, extradata);
	else return srch;
}

/*
 * \brief Cut item from list
 */
template <class T, class D> void RefList<T,D>::cut(RefListItem<T,D>* item)
{
	if (item == NULL)
	{
		printf("Internal Error: NULL pointer passed to RefList<T>::cut().\n");
		return;
	}
	RefListItem<T,D>* prev, *next;
	prev = item->prev;
	next = item->next;
	if (prev == NULL) listHead_ = next;
	else prev->next = next;
	if (next == NULL) listTail_ = prev;
	else next->prev = prev;
	item->next = NULL;
	item->prev = NULL;
	--nItems_;
	regenerate_ = 1;
}

/*
 * \brief Own an existing item
 */
template <class T, class D> void RefList<T,D>::own(RefListItem<T,D>* olditem)
{
	// In the interests of 'pointer cleanliness, refuse to own the item if its pointers are not NULL
	if ((olditem->next != NULL) || (olditem->prev != NULL))
	{
		printf("RefList::own <<<< RefList refused to own an item that still had ties >>>>\n");
		return;
	}
	listHead_ == NULL ? listHead_ = olditem : listTail_->next = olditem;
	olditem->prev = listTail_;
	olditem->next = NULL;
	listTail_ = olditem;
	nItems_ ++;
	regenerate_ = 1;
}

/*
 * \brief Remove RefListItem from list
 */
template <class T, class D> void RefList<T,D>::remove(RefListItem<T,D>* xitem)
{
	if (xitem == NULL)
	{
		printf("Internal Error: NULL pointer passed to RefList<T,D>::remove().\n");
		return;
	}
	// Delete a specific RefListItem from the list
	xitem->prev == NULL ? listHead_ = xitem->next : xitem->prev->next = xitem->next;
	xitem->next == NULL ? listTail_ = xitem->prev : xitem->next->prev = xitem->prev;
	delete xitem;
	nItems_ --;
	regenerate_ = 1;
}

/*
 * \brief Remove first item from list
 */
template <class T, class D> void RefList<T,D>::removeFirst()
{
	if (listHead_ == NULL)
	{
		printf("Internal Error: No item to delete in  RefList<T,D>::removeFirst().\n");
		return;
	}
	remove(listHead_);
	regenerate_ = 1;
}

/*
 * \brief Remove last item from list
 */
template <class T, class D> void RefList<T,D>::removeLast()
{
	if (listTail_ == NULL)
	{
		printf("Internal Error: No item to delete in  RefList<T,D>::removeFirst().\n");
		return;
	}
	remove(listTail_);
	regenerate_ = 1;
}

/*
 * \brief Remove item from list
 */
template <class T, class D> void RefList<T,D>::remove(T* xitem)
{
	// Delete a specific item from the list
	RefListItem<T,D>* r = contains(xitem);
	if (r != NULL) remove(r);
}

/*
 * \brief Element access operator
 */
template <class T, class D> RefListItem<T,D>* RefList<T,D>::operator[](int index)
{
#ifdef CHECKS
	if ((index < 0) || (index >= nItems_))
	{
		printf("REFLIST_OPERATOR[] - Array index (%i) out of bounds (%i items in RefList) >>>>\n", index, nItems_);
		return NULL;
	}
#endif
	// Use array() function to return item
	return array()[index];
}

/*
 * \brief Item access operator
 */
template <class T, class D> RefListItem<T,D>* RefList<T,D>::item(int n) const
{
#ifdef CHECKS
	if ((n < 0) || (n >= nItems_))
	{
		printf("REFLIST_OPERATOR[] - Array index (%i) out of bounds (%i items in RefList) >>>>\n", n, nItems_);
		return NULL;
	}
#endif
	int count = -1;
	for (RefListItem<T,D>* r = listHead_; r != NULL; r = r->next) if (++count == n) return r;
	return NULL;
}

/*
 * \brief Search for item
 */
template <class T, class D> RefListItem<T,D>* RefList<T,D>::contains(T* xitem) const
{
	// Search references for specified item
	RefListItem<T,D>* r;
	for (r = listHead_; r != NULL; r = r->next) if (r->item == xitem) break;
	return r;
}

/*
 * \brief Search for data
 */
template <class T, class D> RefListItem<T,D>* RefList<T,D>::containsData(D data) const
{
	// Search references for specified data
	RefListItem<T,D>* r;
	for (r = listHead_; r != NULL; r = r->next) if (r->data == data) break;
	return r;
}

/*
 * \brief Clear atoms from list
 */
template <class T, class D> void RefList<T,D>::clear()
{
	// Clear the list 
	RefListItem<T,D>* xitem = listHead_;
	while (xitem != NULL)
	{
		remove(xitem);
		xitem = listHead_;
	}
	listHead_ = NULL;
	listTail_ = NULL;
	nItems_ = 0;
	// Delete static items array if its there
	if (items_ != NULL) delete[] items_;
	items_ = NULL;
	regenerate_ = 1;
}

/*
 * \brief Prune items from list
 */
template <class T, class D> void RefList<T,D>::prune(D data)
{
	RefListItem<T,D>* xitem = listHead_, *next;
	while (xitem != NULL)
	{
		next = xitem->next;
		if (xitem->data == data) remove(xitem);
		xitem = next;
	}
}

/*
 * \brief Move head to tail
 */
template <class T, class D> void RefList<T,D>::moveHeadToTail()
{
	// Add a new item to the list (a copy of the current head)
	add(listHead_->item,listHead_->data);
	// Delete head item
	remove(listHead_);
}

/*
 * \brief Create from List
 */
template <class T, class D> void RefList<T,D>::createFromList(T* xitem, D startData)
{
	clear();
	while (xitem != NULL)
	{
		add(xitem, startData);
		xitem = xitem->next;
	}
}

/*
 * \brief Create from RefList
 */
template <class T, class D> void RefList<T,D>::createFromRefList(RefListItem<T,D>* xitem)
{
	clear();
	while (xitem != NULL)
	{
		add(xitem->item, xitem->data);
		xitem = xitem->next;
	}
}

/*
 * \brief Fill array
 */
template <class T, class D> void RefList<T,D>::fillArray(int n, T* data)
{
	int count = 0;
	RefListItem<T,D>* ri = listHead_;
	while (ri != NULL)
	{
		data[count] = ri->item;
		count ++;
		if (count == n) break;
		ri = ri->next;
		if (ri == NULL) printf("Internal Error: Not enough items in list (requested %i, had %i) in RefList::fillArray()\n",n,nItems_);
	}
	regenerate_ = 1;
}

/*
 * \brief Swap the two items specified
 */
template <class T, class D> void RefList<T,D>::swap(T* item1, T* item2)
{
	if ((item1 == NULL) || (item2 == NULL))
	{
		printf("Internal Error: NULL pointer(s) passed to RefList<T,D>::swap().\n", item1, item2);
		return;
	}
	T* prev1 = item1->prev, *next1 = item1->next;
	item1->prev = item2->prev;
	item1->next = item2->next;
	item2->prev = prev1;
	item2->next = next1;
	regenerate_ = 1;
}

/*
 * \brief Create (or just return) the item array
 */
template <class T, class D> RefListItem<T,D>* *RefList<T,D>::array()
{
	if (regenerate_ == 0) return items_;
	// Delete old atom list (if there is one)
	if (items_ != NULL) delete[] items_;
	// Create new list
	items_ = new RefListItem<T,D>*[nItems_];
	// Fill in pointers
	int count = 0;
	for (RefListItem<T,D>* ri = listHead_; ri != NULL; ri = ri->next) items_[count++] = ri;
	regenerate_ = 0;
	return items_;
}

#endif
