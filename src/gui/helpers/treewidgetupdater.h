/*
	*** TreeWidgetUpdater
	*** src/gui/helpers/treewidgetupdater.h
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

#include "templates/variantpointer.h"
#include "templates/list.h"
#include <QTreeWidget>

#ifndef DISSOLVE_TREEWIDGETUPDATER_H
#define DISSOLVE_TREEWIDGETUPDATER_H

// TreeWidgetUpdater - Constructor-only template class to update contents of a QTreeWidget, preserving original items as much as possible
template <class T, class I> class TreeWidgetUpdater
{
	// Typedefs for passed functions
	typedef void (T::*TreeWidgetChildUpdateFunction)(QTreeWidgetItem* parentItem, int childIndex, I* item, bool createItem);

	public:
	// Constructor
	TreeWidgetUpdater(QTreeWidgetItem* parentItem, const List<I>& data, T* functionParent, TreeWidgetChildUpdateFunction updateChild)
	{
		QTreeWidgetItem* treeItem;

		int count = 0;

		ListIterator<I> dataIterator(data);
		while (I* dataItem = dataIterator.iterate())
		{
			// Our QTreeWidgetItem may or may not be populated, and with different items to those in the list.

			// If there is an item already at this child position, check it
			// If it represents the current pointer data, just update it and move on. Otherwise, delete it and check again
			while (count < parentItem->childCount())
			{
				treeItem = parentItem->child(count);
				I* rowData = (treeItem ? VariantPointer<I>(treeItem->data(0, Qt::UserRole)) : NULL);
				if (rowData == dataItem)
				{
					// Update the current row and quit the loop
					(functionParent->*updateChild)(parentItem, count, dataItem, false);

					break;
				}
				else parentItem->removeChild(parentItem->child(count));
			}

			// If the current child index is (now) out of range, add a new item to the parent
			if (count == parentItem->childCount())
			{
				// Create new item
				(functionParent->*updateChild)(parentItem, count, dataItem, true);
			}

			++count;
		}
	}
};

// TreeWidgetRefListUpdater - Constructor-only template class to update contents of a QTreeWidget from a RefList, preserving original items as much as possible
template <class T, class I, class D> class TreeWidgetRefListUpdater
{
	// Typedefs for passed functions
	typedef void (T::*TreeWidgetChildUpdateFunction)(QTreeWidgetItem* parentItem, int childIndex, I* item, bool createItem);

	public:
	// Constructor
	TreeWidgetRefListUpdater(QTreeWidgetItem* parentItem, const RefList<I,D>& data, T* functionParent, TreeWidgetChildUpdateFunction updateChild)
	{
		QTreeWidgetItem* treeItem;

		int count = 0;

		RefListIterator<I,D> itemIterator(data);
		while (I* dataItem = itemIterator.iterate())
		{
			// Our QTreeWidgetItem may or may not be populated, and with different items to those in the list.

			// If there is an item already at this child position, check it
			// If it represents the current pointer data, just update it and move on. Otherwise, delete it and check again
			while (count < parentItem->childCount())
			{
				treeItem = parentItem->child(count);
				I* rowData = (treeItem ? VariantPointer<I>(treeItem->data(0, Qt::UserRole)) : NULL);
				if (rowData == dataItem)
				{
					// Update the current row and quit the loop
					(functionParent->*updateChild)(parentItem, count, dataItem, false);

					break;
				}
				else parentItem->removeChild(parentItem->child(count));
			}

			// If the current child index is (now) out of range, add a new item to the parent
			if (count == parentItem->childCount())
			{
				// Create new item
				(functionParent->*updateChild)(parentItem, count, dataItem, true);
			}

			++count;
		}
	}
};

#endif
