/*
	*** QComboBox Updater
	*** src/gui/helpers/comboboxupdater.h
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

#include "base/charstring.h"
#include "templates/variantpointer.h"
#include "templates/list.h"
#include "templates/refdatalist.h"
#include <QComboBox>

#ifndef DISSOLVE_COMBOBOXUPDATER_H
#define DISSOLVE_COMBOBOXUPDATER_H

// ComboBoxUpdater - Constructor-only template class to update items in a QComboBox, preserving original items as much as possible
template <class I> class ComboBoxUpdater
{
	public:
	// Update QComboBox from supplied List, assuming that class I implements a name() function for the item
	ComboBoxUpdater(QComboBox* comboBox, const List<I>& data, const I* currentItem, int startIndex = 0, int indexIfNoCurrentItem = -1)
	{
		comboBox_ = comboBox;
		currentIndex_ = startIndex;

		ListIterator<I> dataIterator(data);
		while (I* dataItem = dataIterator.iterate()) updateItem(dataItem->name(), dataItem, dataItem == currentItem);

		// If there are still rows remaining in the widget, delete them now
		while (currentIndex_ < comboBox_->count()) comboBox_->removeItem(currentIndex_);

		// If there is no valid current item, make sure this is reflected in the combobox
		if (currentItem == NULL) comboBox->setCurrentIndex(indexIfNoCurrentItem);
	}

	// Update QComboBox from supplied RefList, assuming that class I implements a name() function for the item
	ComboBoxUpdater(QComboBox* comboBox, const RefList<I>& data, const I* currentItem, int startIndex = 0, int indexIfNoCurrentItem = -1)
	{
		comboBox_ = comboBox;
		currentIndex_ = startIndex;

		for (I* dataItem : data) updateItem(dataItem->name(), dataItem, dataItem == currentItem);

		// If there are still rows remaining in the widget, delete them now
		while (currentIndex_ < comboBox_->count()) comboBox_->removeItem(currentIndex_);

		// If there is no valid current item, make sure this is reflected in the combobox
		if (currentItem == NULL) comboBox->setCurrentIndex(indexIfNoCurrentItem);
	}

	// Update QComboBox from supplied RefDataList, using the CharString data as the item's name
	ComboBoxUpdater(QComboBox* comboBox, const RefDataList<I,CharString>& data, const I* currentItem, int startIndex = 0, int indexIfNoCurrentItem = -1)
	{
		comboBox_ = comboBox;
		currentIndex_ = startIndex;

		RefDataListIterator<I,CharString> dataIterator(data);
		while (I* dataItem = dataIterator.iterate()) updateItem(dataIterator.currentData(), dataItem, dataItem == currentItem);

		// If there are still rows remaining in the widget, delete them now
		while (currentIndex_ < comboBox_->count()) comboBox_->removeItem(currentIndex_);

		// If there is no valid current item, make sure this is reflected in the combobox
		if (currentItem == NULL) comboBox->setCurrentIndex(indexIfNoCurrentItem);
	}

	private:
	// Target combo box
	QComboBox* comboBox_;
	// Item index
	int currentIndex_;

	private:
	// Update item at specified position in combo box (if it exists)
	void updateItem(const char* itemText, I* itemData, bool isCurrent)
	{
		/*
		 * If there is an entry already at this index, check it:
		 * - If it represents the current pointer data, just update it and move on.
		 * - Otherwise, delete it and check the next one.
		 *
		 * If no entry exists, create a new one.
		 */
		while (currentIndex_ < comboBox_->count())
		{
			I* oldData = VariantPointer<I>(comboBox_->itemData(currentIndex_, Qt::UserRole));
			if (oldData == itemData)
			{
				// Data pointer matches - make sure the text is up to date
				comboBox_->setItemText(currentIndex_, itemText);

				break;
			}
			else
			{
				// Data pointer does not match - remove the offending item
				comboBox_->removeItem(currentIndex_);
			}
		}

		// If the current row index is (now) out of range, add a new row to the list
		if (currentIndex_ == comboBox_->count())
		{
			// Create new item
			comboBox_->addItem(itemText, VariantPointer<I>(itemData));
		}

		// Select this item if it is the current one
		if (isCurrent) comboBox_->setCurrentIndex(currentIndex_);

		// Increase index
		++currentIndex_;
	}
};

// ComboBoxUpdater - Constructor-only template class to update items in a QComboBox, preserving original items as much as possible
template <class T, class I> class ComboBoxTextUpdater
{
	// Typedefs for passed functions
	typedef const char* (T::*ComboBoxItemTextFunction)(const I* item);

	public:
	// Update QComboBox from supplied RefList, using provided function to acquire text to display for item
	ComboBoxTextUpdater(QComboBox* comboBox, const RefList<I>& data, const I* currentItem, T* functionParent, ComboBoxItemTextFunction textForItem)
	{
		int currentIndex = 0;

		for (I* dataItem : data)
		{
			/*
			 * If there is an item already on this row, check it
			 * If it represents the current pointer data, just update it and move on.
			 * Otherwise, delete it and check again.
			 */
			while (currentIndex < comboBox->count())
			{
				I* oldData = VariantPointer<I>(comboBox->itemData(currentIndex, Qt::UserRole));
				if (oldData == dataItem)
				{
					// Data pointer matches - make sure the text is up to date
					comboBox->setItemText(currentIndex, (functionParent->*textForItem)(dataItem));

					break;
				}
				else
				{
					// Data pointer does not match - remove the offending item
					comboBox->removeItem(currentIndex);
				}
			}

			// If the current row index is (now) out of range, add a new row to the list
			if (currentIndex == comboBox->count())
			{
				// Create new item
				comboBox->addItem((functionParent->*textForItem)(dataItem), VariantPointer<I>(dataItem));
			}

			// Select this item if it is the current one
			if (currentItem == dataItem) comboBox->setCurrentIndex(currentIndex);

			++currentIndex;
		}

		// If there are still rows remaining in the widget, delete them now
		while (currentIndex < comboBox->count()) comboBox->removeItem(currentIndex);

		// If there is no valid current item, make sure this is reflected in the combobox
		if (currentItem == NULL) comboBox->setCurrentIndex(-1);
	}
};

#endif
