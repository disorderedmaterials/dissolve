/*
	*** ListWidgetUpdater
	*** src/gui/listwidgetupdater.h
	Copyright T. Youngs 2012-2018

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

#include "templates/variantpointer.h"
#include "templates/list.h"
#include <QListWidget>

#ifndef DUQ_LISTWIDGETUPDATER_H
#define DUQ_LISTWIDGETUPDATER_H

// ListWidgetUpdater - Constructor-only template class to update contents of a QListWidget, preserving original items as much as possible
template <class T, class I> class ListWidgetUpdater
{
	// Typedefs for passed functions
	typedef void (T::*ListWidgetRowUpdateFunction)(int row, I* item, bool createItem);

	public:
	// Update widget from supplied List, calling supplied function to create / modify data
	ListWidgetUpdater(QListWidget* listWidget, const List<I>& data, T* functionParent, ListWidgetRowUpdateFunction updateRow)
	{
		QListWidgetItem* listWidgetItem;

		int currentRow = 0;

		ListIterator<I> dataIterator(data);
		while (I* dataItem = dataIterator.iterate())
		{
			// Our table may or may not be populated, and with different items to those in the list.

			// If there is an item already on this row, check it
			// If it represents the current pointer data, just update it and move on. Otherwise, delete it and check again
			while (currentRow < listWidget->count())
			{
				listWidgetItem = listWidget->item(currentRow);
				I* rowData = (listWidgetItem ? VariantPointer<I>(listWidgetItem->data(Qt::UserRole)) : NULL);
				if (rowData == dataItem)
				{
					// Update the current row and quit the loop
					(functionParent->*updateRow)(currentRow, dataItem, false);

					break;
				}
				else
				{
					QListWidgetItem* oldItem = listWidget->takeItem(currentRow);
					if (oldItem) delete oldItem;
				}
			}

			// If the current row index is (now) out of range, add a new row to the list
			if (currentRow == listWidget->count())
			{
				// Create new items
				(functionParent->*updateRow)(currentRow, dataItem, true);
			}

			++currentRow;

		}
	}

	// Update widget from supplied List, assuming that the name() function in class I is the desired text to show in the list
	ListWidgetUpdater(QListWidget* listWidget, const List<I>& data)
	{
		QListWidgetItem* listWidgetItem;

		int currentRow = 0;

		ListIterator<I> dataIterator(data);
		while (I* dataItem = dataIterator.iterate())
		{
			// Our table may or may not be populated, and with different items to those in the list.

			// If there is an item already on this row, check it
			// If it represents the current pointer data, just update it and move on. Otherwise, delete it and check again
			while (currentRow < listWidget->count())
			{
				listWidgetItem = listWidget->item(currentRow);
				I* rowData = (listWidgetItem ? VariantPointer<I>(listWidgetItem->data(Qt::UserRole)) : NULL);
				if (rowData == dataItem)
				{
					// Update the current row and quit the loop
					listWidgetItem->setText(dataItem->name());

					break;
				}
				else
				{
					QListWidgetItem* oldItem = listWidget->takeItem(currentRow);
					if (oldItem) delete oldItem;
				}
			}

			// If the current row index is (now) out of range, add a new row to the list
			if (currentRow == listWidget->count())
			{
				// Create new items
				listWidgetItem = new QListWidgetItem;
				listWidget->addItem(listWidgetItem);
				listWidgetItem->setData(Qt::UserRole, VariantPointer<I>(dataItem));
				listWidgetItem->setText(dataItem->name());
			}

			++currentRow;

		}
	}
};

#endif
