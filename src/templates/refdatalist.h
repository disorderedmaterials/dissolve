/*
	*** Reference Data List
	*** src/templates/refdatalist.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_REFDATALIST_H
#define DISSOLVE_REFDATALIST_H

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
	// Constructor
	RefDataItem<T,D>()
	{
		item_ = NULL;
		next_ = NULL;
		prev_ = NULL;
	}


	/*
	 * Data
	 */
	private:
	// Pointer to item
	T* item_;
	// Associated data
	D data_;

	public:
	// Return item
	T* item()
	{
		return item_;
	}
	// Set associated data
	void setData(D data)
	{
		data_ = data;
	}
	// Return associated data
	const D& data() const
	{
		return data_;
	}


	/*
	 * List Pointers
	 */
	private:
	// List pointers
	RefDataItem<T,D>* prev_, *next_;

	public:
	// Return item after this one
	RefDataItem<T,D>* next() const
	{
		return next_;
	}
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
	// Constructor
	RefDataList<T,D>()
	{
		listHead_ = NULL;
		listTail_ = NULL;
		items_ = NULL;
		regenerate_ = true;
		nItems_ = 0;
	}

	// Destructor
	~RefDataList()
	{
		clear();
	}
	// Copy Constructor
	RefDataList<T,D>(const RefDataList<T,D>& source)
	{
		listHead_ = NULL;
		listTail_ = NULL;
		items_ = NULL;
		regenerate_ = true;
		nItems_ = 0;
		for (RefDataItem<T,D>* ri = source.first(); ri != NULL; ri = ri->next_) append(ri->item_, ri->data_);
	}
	// Operator =
	void operator=(const RefDataList<T,D>& source)
	{
		// Clear any current data...
		clear();
		for (RefDataItem<T,D>* ri = source.first(); ri != NULL; ri = ri->next_) append(ri->item_, ri->data_);
	}
	// Element access operator
	RefDataItem<T,D>* operator[](int index)
	{
#ifdef CHECKS
		if ((index < 0) || (index >= nItems_))
		{
			printf("Array index (%i) out of bounds (%i items in RefDataList)\n", index, nItems_);
			return NULL;
		}
#endif
		// Use array() function to return item
		return array()[index];
	}


	/*
	 * Items
	 */
	private:
	// Head and tail of reference items
	RefDataItem<T,D>* listHead_, *listTail_;
	// Number of items in list
	int nItems_;
	// Static array of items
	RefDataItem<T,D>** items_;
	// Array regeneration flag
	bool regenerate_;

	public:
	// Clear the list of all references
	void clear()
	{
		// Clear the list 
		RefDataItem<T,D>* xitem = listHead_;
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
		regenerate_ = true;
	}
	// Returns the head of the item list
	RefDataItem<T,D>* first() const
	{
		return listHead_;
	}
	// Returns the last item in the list
	RefDataItem<T,D>* last() const
	{
		return listTail_;
	}
	// Returns the T referenced by the head of the item list
	T* firstItem() const
	{
		if (listHead_) return listHead_->item_;
		else return NULL;
	}
	// Returns the T referenced by the tail of the item list
	T* lastItem() const
	{
		if (listTail_) return listTail_->item_;
		else return NULL;
	}
	// Returns the number of atoms in the list
	int nItems() const
	{
		return nItems_;
	}
	// Append reference to the list
	RefDataItem<T,D>* append(T* item, D data)
	{
		RefDataItem<T,D>* newitem = new RefDataItem<T,D>;
		// Add the pointer to the list
		listHead_ == NULL ? listHead_ = newitem : listTail_->next_ = newitem;
		newitem->prev_ = listTail_;
		listTail_ = newitem;
		newitem->item_ = item;
		newitem->data_ = data;
		++nItems_;
		regenerate_ = true;
		return newitem;
	}
	// Prepend reference to the beginning of the list
	RefDataItem<T,D>* prepend(T* item, D data)
	{
		RefDataItem<T,D>* newitem = new RefDataItem<T,D>;
		// Add the pointer to the beginning of the list
		newitem->next_ = listHead_;
		listHead_ == NULL ? listHead_ = newitem : listHead_->prev_ = newitem;
		listHead_ = newitem;
		newitem->item_ = item;
		newitem->data_ = data;
		++nItems_;
		regenerate_ = true;
		return newitem;
	}
	// Add reference after the specified item
	RefDataItem<T,D>* addAfter(RefDataItem<T,D>* target, T* item)
	{
		if (target == NULL) return add(item);

		RefDataItem<T,D>* newitem = new RefDataItem<T,D>;
		newitem->prev_ = target;
		newitem->next_ = target->next_;
		if (target->next_ != NULL) target->next->prev_ = newitem;
		target->next_ = newitem;
		if (target == listTail_) listTail_ = newitem;
		newitem->item = item;
		++nItems_;
		regenerate_ = true;

		return newitem;
	}
	// Add reference after the specified item (overload)
	RefDataItem<T,D>* addAfter(T* targetItem, T* item)
	{
		// Find the specified item
		RefDataItem<T,D>* target = contains(targetItem);
		if (target) return addAfter(target, item);

		printf("Couldn't find specified item %p in RefDataList, so adding to end.\n", item);
		return add(item);
	}
	// Add reference before the specified item
	RefDataItem<T,D>* addBefore(RefDataItem<T,D>* target, T* item)
	{
		if (target == NULL) return add(item);

		RefDataItem<T,D>* newitem = new RefDataItem<T,D>;
		newitem->next_ = target;
		newitem->prev_ = target->prev_;
		if (target->prev_ != NULL) target->prev->next_ = newitem;
		target->prev_ = newitem;
		if (target == listHead_) listHead_ = newitem;
		newitem->item = item;
		++nItems_;
		regenerate_ = true;

		return newitem;
	}
	// Add reference before the specified item (overload)
	RefDataItem<T,D>* addBefore(T* targetItem, T* item)
	{
		// Find the specified item
		RefDataItem<T,D>* target = contains(targetItem);
		if (target) return addBefore(target, item);

		printf("Couldn't find specified item %p in RefDataList, so adding to start.\n", item);
		return addStart(item);
	}
	// Add reference to list, unless already there
	RefDataItem<T,D>* addUnique(T* item, D data)
	{
		RefDataItem<T,D>* srch = contains(item);
		if (srch == NULL) return append(item, data);
		else return srch;
	}
	// Cut item from list (orphan it)
	void cut(RefDataItem<T,D>* item)
	{
		if (item == NULL)
		{
			printf("Internal Error: NULL pointer passed to RefDataList<T>::cut().\n");
			return;
		}
		RefDataItem<T,D>* prev, *next;
		prev = item->prev_;
		next = item->next_;
		if (prev == NULL) listHead_ = next;
		else prev->next_ = next;
		if (next == NULL) listTail_ = prev;
		else next->prev_ = prev;
		item->next_ = NULL;
		item->prev_ = NULL;
		--nItems_;
		regenerate_ = true;
	}
	// Add an orphaned item into this list
	void own(RefDataItem<T,D>* item)
	{
		// In the interests of 'pointer cleanliness, refuse to own the item if its pointers are not NULL
		if ((item->next_ != NULL) || (item->prev_ != NULL))
		{
			printf("RefList::own() <<<< Refused to own an item that still had links to other items >>>>\n");
			return;
		}
		listHead_ == NULL ? listHead_ = item : listTail_->next_ = item;
		item->prev_ = listTail_;
		item->next_ = NULL;
		listTail_ = item;
		++nItems_;
		regenerate_ = true;
	}
	// Delete the item from the list
	void remove(RefDataItem<T,D>* item)
	{
		if (item == NULL)
		{
			printf("Internal Error: NULL pointer passed to RefDataList<T,D>::remove().\n");
			return;
		}
		// Delete a specific RefDataItem from the list
		item->prev_ == NULL ? listHead_ = item->next_ : item->prev_->next_ = item->next_;
		item->next_ == NULL ? listTail_ = item->prev_ : item->next_->prev_ = item->prev_;
		delete item;
		nItems_ --;
		regenerate_ = true;
	}
	// Delete the reference containing specified item from the list
	void remove(T* item)
	{
		// Delete a specific item from the list
		RefDataItem<T,D>* r = contains(item);
		if (r != NULL) remove(r);
	}
        // Remove all items that match specified data value
        void removeIfData(D value)
	{
		RefDataItem<T,D>* ri = listHead_, *next;
		while (ri)
		{
			// Store next pointer, in case we delete the current item
			next = ri->next_;
			if (ri->data == value) remove(ri);
			ri = next;
		}
	}
	// Remove the first item in the list
	void removeFirst()
	{
		if (listHead_ == NULL)
		{
			printf("Internal Error: No item to delete in  RefDataList<T,D>::removeFirst().\n");
			return;
		}
		remove(listHead_);
		regenerate_ = true;
	}
	// Remove the last item in the list
	void removeLast()
	{
		if (listTail_ == NULL)
		{
			printf("Internal Error: No item to delete in  RefDataList<T,D>::removeFirst().\n");
			return;
		}
		remove(listTail_);
		regenerate_ = true;
	}
	// Prune items with specified data
	void prune(D data)
	{
		RefDataItem<T,D>* xitem = listHead_, *next;
		while (xitem != NULL)
		{
			next = xitem->next_;
			if (xitem->data == data) remove(xitem);
			xitem = next;
		}
	}
	// Move head of list to tail of list
	void moveHeadToTail()
	{
		// Add a new item to the list (a copy of the current head)
		add(listHead_->item,listHead_->data);
		// Delete head item
		remove(listHead_);
	}
	// Swap the two items specified
	void swap(T* item1, T* item2)
	{
		if ((item1 == NULL) || (item2 == NULL))
		{
			printf("Internal Error: NULL pointer(s) passed to RefDataList<T,D>::swap().\n", item1, item2);
			return;
		}
		T* prev1 = item1->prev, *next1 = item1->next_;
		item1->prev_ = item2->prev_;
		item1->next_ = item2->next_;
		item2->prev_ = prev1;
		item2->next_ = next1;
		regenerate_ = true;
	}
	// Fills the supplied array with 'n' pointer values to the reference items
	void fillArray(int n, T* destArray)
	{
		int count = 0;
		RefDataItem<T,D>* ri = listHead_;
		while (ri != NULL)
		{
			destArray[count] = ri->item;
			count ++;
			if (count == n) break;
			ri = ri->next_;
			if (ri == NULL) printf("Internal Error: Not enough items in list (requested %i, had %i) in RefDataList::fillArray()\n", n, nItems_);
		}
		regenerate_ = true;
	}
	// Return nth item in list
	T* item(int n)
	{
	#ifdef CHECKS
		if ((n < 0) || (n >= nItems_))
		{
			printf("Array index (%i) out of bounds (%i items in RefDataList)\n", n, nItems_);
			return NULL;
		}
	#endif
		// Use array() function to return item
		return array()[n]->item;
	}
	// Return (first) item with specified data
	T* itemWithData(D data)
	{
		// Search references for specified data
		for (RefDataItem<T,D>* r = listHead_; r != NULL; r = r->next_) if (r->data_ == data) return r->item_;

		return NULL;
	}
	// Return array of items
	RefDataItem<T,D>** array()
	{
		if (regenerate_ == 0) return items_;

		// Delete old atom list (if there is one)
		if (items_ != NULL) delete[] items_;

		// Create new list
		items_ = new RefDataItem<T,D>*[nItems_];

		// Fill in pointers
		int count = 0;
		for (RefDataItem<T,D>* ri = listHead_; ri != NULL; ri = ri->next_) items_[count++] = ri;

		regenerate_ = false;

		return items_;
	}


	/*
	 * Search
	 */
	public:
	// Search references for item
	RefDataItem<T,D>* contains(T* item) const
	{
		// Search references for specified item
		for (RefDataItem<T,D>* r = listHead_; r != NULL; r = r->next_) if (r->item_ == item) return r;

		return NULL;
	}
	// Search references for item and data
	RefDataItem<T,D>* contains(T* item, D data) const
	{
		// Search references for specified item
		for (RefDataItem<T,D>* r = listHead_; r != NULL; r = r->next_) if ((r->item_ == item) && (r->data_ == data)) return r;

		return NULL;
	}
	// Search references for data
	RefDataItem<T,D>* containsData(D data) const
	{
		// Search references for specified data
		for (RefDataItem<T,D>* r = listHead_; r != NULL; r = r->next_) if (r->data_ == data) return r;

		return NULL;
	}
};

/*
 * List Iterator
 */

// Reference List Data Iterator
template <class T, class D> class RefDataListIterator
{
	public:
	// Constructor
	RefDataListIterator<T,D>(const RefDataList<T,D>& source, bool reverse = false) : reverse_(reverse), targetRefList_(source)
	{
		finished_ = false;
		currentItem_ = NULL;
	}

	private:
	// Whether the iterator has reached the end of the list
	bool finished_;
	// Whether the iterator operates in reverse (iterating tail to head)
	bool reverse_;
	// Target list
	const RefDataList<T,D>& targetRefList_;
	// Current item
	RefDataItem<T,D>* currentItem_;

	public:
	// Iterate
	T* iterate()
	{
		if (finished_) return NULL;

		// Go to initial / next item
		if (currentItem_ == NULL) currentItem_ = reverse_ ? targetRefList_.last() : targetRefList_.first();
		else currentItem_ = reverse_ ? currentItem_->prev_ : currentItem_->next_;

		// Check for end of list
		if (currentItem_ == NULL) finished_ = true;

		return (currentItem_ ? currentItem_->item() : NULL);
	}
	// Peek the next item (if any)
	T* peek()
	{
		if (reverse_)
		{
			return (currentItem_ ? (currentItem_->prev_ ? currentItem_->prev->item : NULL) : NULL);
		}
		else return (currentItem_ ? (currentItem_->next_ ? currentItem_->next->item : NULL) : NULL);
	}
	// Peek the previous item (if any)
	T* peekPrevious()
	{
		if (reverse_)
		{
			return (currentItem_ ? (currentItem_->next_ ? currentItem_->next->item : NULL) : NULL);
		}
		else return (currentItem_ ? (currentItem_->prev_ ? currentItem_->prev->item : NULL) : NULL);
	}
	// Peek the next data (if any)
	D& peekData()
	{
		static D dummy;
		if (reverse_)
		{
			return (currentItem_ ? (currentItem_->prev_ ? currentItem_->prev->data : dummy) : dummy);
		}
		else return (currentItem_ ? (currentItem_->next_ ? currentItem_->next->data : dummy) : dummy);
	}
	// Peek the previous data (if any)
	D& peekPreviousData()
	{
		static D dummy;
		if (reverse_)
		{
			return (currentItem_ ? (currentItem_->next_ ? currentItem_->next->data : dummy) : dummy);
		}
		else return (currentItem_ ? (currentItem_->prev_ ? currentItem_->prev->data : dummy) : dummy);
	}
	// Return current reference item
	RefDataItem<T,D>* currentItem()
	{
		return currentItem_;
	}
	// Set current data
	void setCurrentData(D data)
	{
		if (currentItem_) currentItem_->setData(data);
		else printf("No current item, so can't set data in RefDataListIterator.\n");
	}
	// Return reference to current data
	const D& currentData() const
	{
		static D dummy;

		if (finished_) return dummy;

		return (currentItem_ ? currentItem_->data() : dummy);
	}
	// Restart iteration
	void restart()
	{
		finished_ = false;
		currentItem_ = NULL;
	}
	// Return whether we are on the first item in the list
	bool isFirst() const
	{
		return (currentItem_ == targetRefList_.first());
	}
	// Return whether we are on the last item in the list
	bool isLast() const
	{
		return (currentItem_ == targetRefList_.last());
	}
};

#endif
