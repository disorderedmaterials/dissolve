/*
	*** Reference List
	*** src/templates/reflist.h
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

#ifndef DISSOLVE_REFLIST_H
#define DISSOLVE_REFLIST_H

#include <stddef.h>
#include <stdio.h>
#include <iterator>

// Forward Declarations
template <class T> class RefList;

/*
 * Item
 */

template <class T> class RefListItem : public std::iterator<std::forward_iterator_tag, T*>
{
	public:
	// Constructor
	RefListItem<T>()
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

	public:
	// Return item
	T* item()
	{
		return item_;
	}
	RefListItem<T> operator++()
	{
		if(next_ == nullptr)
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
	bool operator==(RefListItem<T> that)
	{
		return next_ == that.next_ && prev_ == that.prev_ && item_ == that.item_;
	}
	bool operator!=(RefListItem<T> that)
	{
		return next_ != that.next_ || prev_ != that.prev_ || item_ != that.item_;
	}
	T*& operator*()
	{
		return item_;
	}


	/*
	 * List Pointers
	 */
	private:
	// List pointers
	RefListItem<T>* prev_, *next_;

	public:
	// Return item after this one
	RefListItem<T>* next() const
	{
		return next_;
	}
	// Declare the list and iterator as friends
	friend class RefList<T>;
};

/*
 * Reference List
 */

template <class T> class RefList
{
	public:
	// Constructors
	RefList<T>()
	{
		listHead_ = NULL;
		listTail_ = NULL;
		items_ = NULL;
		regenerate_ = true;
		nItems_ = 0;
	}
	RefList<T>(T* singleItem)
	{
		listHead_ = NULL;
		listTail_ = NULL;
		items_ = NULL;
		regenerate_ = true;
		nItems_ = 0;

		append(singleItem);
	}

	// Destructor
	~RefList()
	{
		clear();
	}
	// Copy Constructor
	RefList<T>(const RefList<T>& source)
	{
		listHead_ = NULL;
		listTail_ = NULL;
		items_ = NULL;
		regenerate_ = true;
		nItems_ = 0;
		for (RefListItem<T>* ri = source.first(); ri != NULL; ri = ri->next_) append(ri->item_);
	}
	// Operator =
	void operator=(const RefList<T>& source)
	{
		// Clear any current data...
		clear();
		for (RefListItem<T>* ri = source.first(); ri != NULL; ri = ri->next_) append(ri->item_);
	}
	// Operator +=
	void operator+=(const RefList<T>& source)
	{
		// Add unique items in the source list to our own
		for (RefListItem<T>* ri = source.first(); ri != NULL; ri = ri->next_) addUnique(ri->item_);
	}
	// Element access operator
	RefListItem<T>* operator[](int index)
	{
#ifdef CHECKS
		if ((index < 0) || (index >= nItems_))
		{
			printf("Array index (%i) out of bounds (%i items in RefList)\n", index, nItems_);
			return NULL;
		}
#endif
		// Use array() function to return item
		return array()[index];
	}
	RefListItem<T> begin() const
	{
		if (listHead_ == nullptr) {
			return end();
		}
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
	RefListItem<T>* listHead_, *listTail_;
	// Number of items in list
	int nItems_;
	// Static array of items
	RefListItem<T>** items_;
	// Array regeneration flag
	bool regenerate_;

	public:
	// Clear the list of all references
	void clear()
	{
		// Clear the list 
		RefListItem<T>* xitem = listHead_;
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
	RefListItem<T>* first() const
	{
		return listHead_;
	}
	// Returns the last item in the list
	RefListItem<T>* last() const
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
	RefListItem<T>* append(T* item)
	{
		RefListItem<T>* newitem = new RefListItem<T>;
		// Add the pointer to the list
		listHead_ == NULL ? listHead_ = newitem : listTail_->next_ = newitem;
		newitem->prev_ = listTail_;
		listTail_ = newitem;
		newitem->item_ = item;
		++nItems_;
		regenerate_ = true;
		return newitem;
	}
	// Prepend reference to the beginning of the list
	RefListItem<T>* prepend(T* item)
	{
		RefListItem<T>* newitem = new RefListItem<T>;
		// Add the pointer to the beginning of the list
		newitem->next_ = listHead_;
		listHead_ == NULL ? listHead_ = newitem : listHead_->prev_ = newitem;
		listHead_ = newitem;
		newitem->item_ = item;
		++nItems_;
		regenerate_ = true;
		return newitem;
	}
	// Add reference after the specified item
	RefListItem<T>* addAfter(RefListItem<T>* target, T* item)
	{
		if (target == NULL) return add(item);

		RefListItem<T>* newitem = new RefListItem<T>;
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
	RefListItem<T>* addAfter(T* targetItem, T* item)
	{
		// Find the specified item
		RefListItem<T>* target = contains(targetItem);
		if (target) return addAfter(target, item);

		printf("Couldn't find specified item %p in RefList, so adding to end.\n", item);
		return add(item);
	}
	// Add reference before the specified item
	RefListItem<T>* addBefore(RefListItem<T>* target, T* item)
	{
		if (target == NULL) return add(item);

		RefListItem<T>* newitem = new RefListItem<T>;
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
	RefListItem<T>* addBefore(T* targetItem, T* item)
	{
		// Find the specified item
		RefListItem<T>* target = contains(targetItem);
		if (target) return addBefore(target, item);

		printf("Couldn't find specified item %p in RefList, so adding to start.\n", item);
		return addStart(item);
	}
	// Add reference to list, unless already there
	RefListItem<T>* addUnique(T* item)
	{
		RefListItem<T>* srch = contains(item);
		if (srch == NULL) return append(item);
		else return srch;
	}
	// Cut item from list (orphan it)
	void cut(RefListItem<T>* item)
	{
		if (item == NULL)
		{
			printf("Internal Error: NULL pointer passed to RefList<T>::cut().\n");
			return;
		}
		RefListItem<T>* prev, *next;
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
	void own(RefListItem<T>* item)
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
	void remove(RefListItem<T>* item)
	{
		if (item == NULL)
		{
			printf("Internal Error: NULL pointer passed to RefList<T>::remove().\n");
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
		RefListItem<T>* r = contains(item);
		if (r != NULL) remove(r);
	}
	// Remove the first item in the list
	void removeFirst()
	{
		if (listHead_ == NULL)
		{
			printf("Internal Error: No item to delete in  RefList<T>::removeFirst().\n");
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
			printf("Internal Error: No item to delete in  RefList<T>::removeFirst().\n");
			return;
		}
		remove(listTail_);
		regenerate_ = true;
	}
	// Swap the two items specified
	void swap(T* item1, T* item2)
	{
		if ((item1 == NULL) || (item2 == NULL))
		{
			printf("Internal Error: NULL pointer(s) passed to RefList<T>::swap().\n", item1, item2);
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
		RefListItem<T>* ri = listHead_;
		while (ri != NULL)
		{
			destArray[count] = ri->item;
			count ++;
			if (count == n) break;
			ri = ri->next_;
			if (ri == NULL) printf("Internal Error: Not enough items in list (requested %i, had %i) in RefList::fillArray()\n", n, nItems_);
		}
		regenerate_ = true;
	}
	// Return nth item in list
	T* item(int n)
	{
	#ifdef CHECKS
		if ((n < 0) || (n >= nItems_))
		{
			printf("Array index (%i) out of bounds (%i items in RefList).\n", n, nItems_);
			return NULL;
		}
	#endif
		// Use array() function to return item
		return array()[n]->item();
	}
	// Return array of items
	RefListItem<T>** array()
	{
		if (regenerate_ == 0) return items_;

		// Delete old atom list (if there is one)
		if (items_ != NULL) delete[] items_;

		// Create new list
		items_ = new RefListItem<T>*[nItems_];

		// Fill in pointers
		int count = 0;
		for (RefListItem<T>* ri = listHead_; ri != NULL; ri = ri->next_) items_[count++] = ri;

		regenerate_ = false;

		return items_;
	}


	/*
	 * Search
	 */
	public:
	// Search references for item
	RefListItem<T>* contains(const T* item) const
	{
		// Search references for specified item
		for (RefListItem<T>* r = listHead_; r != NULL; r = r->next_) if (r->item_ == item) return r;

		return NULL;
	}
};

#endif
