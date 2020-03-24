/*
	*** TreeWidgetUpdater
	*** src/gui/helpers/treewidgetupdater.h
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

#include "templates/variantpointer.h"
#include "templates/list.h"
#include "templates/refdatalist.h"
#include <QTreeWidget>

#ifndef DISSOLVE_TREEWIDGETUPDATER_H
#define DISSOLVE_TREEWIDGETUPDATER_H

// TreeWidgetUpdater - Constructor-only template class to update QTreeWidget items, preserving original items as much as possible
template <class T, class I> class TreeWidgetUpdater
{
	// Typedefs for passed functions
	typedef void (T::*TreeWidgetTopLevelUpdateFunction)(QTreeWidget* treeWidget, int topLevelItemIndex, I* data, bool createItem);
	typedef void (T::*TreeWidgetChildUpdateFunction)(QTreeWidgetItem* parentItem, int childIndex, I* data, bool createItem);

	public:
	// Update the top-level items of the specified parent QTreeWidget
	TreeWidgetUpdater(QTreeWidget* treeWidget, const List<I>& data, T* functionParent, TreeWidgetTopLevelUpdateFunction updateTopLevelFunction)
	{
		int count = 0;

		ListIterator<I> dataIterator(data);
		while (I* dataItem = dataIterator.iterate())
		{
			// Our QTreeWidget may or may not be populated, and with different items to those in the list.

			// If there is an item already at this child position, check it
			// If it represents the current pointer data, just update it and move on. Otherwise, delete it and check again
			while (count < treeWidget->topLevelItemCount())
			{
				QTreeWidgetItem* treeItem = treeWidget->topLevelItem(count);
				I* rowData = (treeItem ? VariantPointer<I>(treeItem->data(0, Qt::UserRole)) : NULL);
				if (rowData == dataItem)
				{
					// Update the current row and quit the loop
					(functionParent->*updateTopLevelFunction)(treeWidget, count, dataItem, false);

					break;
				}
				else
				{
					treeWidget->takeTopLevelItem(count);
					delete treeItem;
				}
			}

			// If the current child index is (now) out of range, add a new item to the parent
			if (count == treeWidget->topLevelItemCount())
			{
				// Create new item
				(functionParent->*updateTopLevelFunction)(treeWidget, count, dataItem, true);
			}

			++count;
		}

		// If there are still items remaining in the widget, delete them now
		while (count < treeWidget->topLevelItemCount())
		{
			QTreeWidgetItem* item = treeWidget->takeTopLevelItem(count);
			delete item;
		}
	}

	// Update the children of the specified parent QTreeWidgetItem
	TreeWidgetUpdater(QTreeWidgetItem* parentItem, const List<I>& data, T* functionParent, TreeWidgetChildUpdateFunction updateChildFunction)
	{
		int count = 0;

		ListIterator<I> dataIterator(data);
		while (I* dataItem = dataIterator.iterate())
		{
			// Our QTreeWidgetItem may or may not be populated, and with different items to those in the list.

			// If there is an item already at this child position, check it
			// If it represents the current pointer data, just update it and move on. Otherwise, delete it and check again
			while (count < parentItem->childCount())
			{
				QTreeWidgetItem* treeItem = parentItem->child(count);
				I* rowData = (treeItem ? VariantPointer<I>(treeItem->data(0, Qt::UserRole)) : NULL);
				if (rowData == dataItem)
				{
					// Update the current row and quit the loop
					(functionParent->*updateChildFunction)(parentItem, count, dataItem, false);

					break;
				}
				else
				{
					parentItem->removeChild(treeItem);
					delete treeItem;
				}
			}

			// If the current child index is (now) out of range, add a new item to the parent
			if (count == parentItem->childCount())
			{
				// Create new item
				(functionParent->*updateChildFunction)(parentItem, count, dataItem, true);
			}

			++count;
		}

		// If there are still items remaining in the widget, delete them now
		while (count < parentItem->childCount()) parentItem->removeChild(parentItem->child(count));
	}
};

// TreeWidgetRefListUpdater - Constructor-only template class to update contents of a QTreeWidget from a RefList, preserving original items as much as possible
template <class T, class I> class TreeWidgetRefListUpdater
{
	// Typedefs for passed functions
	typedef void (T::*TreeWidgetTopLevelUpdateFunction)(QTreeWidget* treeWidget, int topLevelItemIndex, I* data, bool createItem);
	typedef void (T::*TreeWidgetChildUpdateFunction)(QTreeWidgetItem* parentItem, int childIndex, I* item, bool createItem);

	public:
	// Update the top-level items of the specified parent QTreeWidget
	TreeWidgetRefListUpdater(QTreeWidget* treeWidget, const RefList<I>& data, T* functionParent, TreeWidgetTopLevelUpdateFunction updateTopLevelFunction)
	{
		int count = 0;

		for (I* dataItem : data)
		{
			// Our QTreeWidget may or may not be populated, and with different items to those in the list.

			// If there is an item already at this child position, check it
			// If it represents the current pointer data, just update it and move on. Otherwise, delete it and check again
			while (count < treeWidget->topLevelItemCount())
			{
				QTreeWidgetItem* treeItem = treeWidget->topLevelItem(count);
				I* rowData = (treeItem ? VariantPointer<I>(treeItem->data(0, Qt::UserRole)) : NULL);
				if (rowData == dataItem)
				{
					// Update the current row and quit the loop
					(functionParent->*updateTopLevelFunction)(treeWidget, count, dataItem, false);

					break;
				}
				else
				{
					treeWidget->takeTopLevelItem(count);
					delete treeItem;
				}
			}

			// If the current child index is (now) out of range, add a new item to the parent
			if (count == treeWidget->topLevelItemCount())
			{
				// Create new item
				(functionParent->*updateTopLevelFunction)(treeWidget, count, dataItem, true);
			}

			++count;
		}

		// If there are still items remaining in the widget, delete them now
		while (count < treeWidget->topLevelItemCount())
		{
			QTreeWidgetItem* item = treeWidget->takeTopLevelItem(count);
			delete item;
		}
	}

	// Constructor
	TreeWidgetRefListUpdater(QTreeWidgetItem* parentItem, const RefList<I>& list, T* functionParent, TreeWidgetChildUpdateFunction updateChild)
	{
		QTreeWidgetItem* treeItem;

		int count = 0;

		for (I* dataItem : list)
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

		// If there are still items remaining in the widget, delete them now
		while (count < parentItem->childCount()) parentItem->removeChild(parentItem->child(count));
	}
};

// TreeWidgetRefListUpdater - Constructor-only template class to update contents of a QTreeWidget from a RefDataList, preserving original items as much as possible
template <class T, class I, class D> class TreeWidgetRefDataListUpdater
{
	// Typedefs for passed functions
	typedef void (T::*TreeWidgetChildUpdateFunction)(QTreeWidgetItem* parentItem, int childIndex, I* item, D data, bool createItem);

	public:
	// Constructor
	TreeWidgetRefDataListUpdater(QTreeWidgetItem* parentItem, const RefDataList<I,D>& list, T* functionParent, TreeWidgetChildUpdateFunction updateChildFunction)
	{
		QTreeWidgetItem* treeItem;

		int count = 0;

		RefDataListIterator<I,D> itemIterator(list);
		while (I* dataItem = itemIterator.iterate())
		{
			// Our QTreeWidgetItem may or may not be populated, and with different items to those in the list.

			// If there is an item already at this child position, check it
			// If it represents the current pointer data, just update it and move on. Otherwise, delete it and check again
			while (count < parentItem->childCount())
			{
				treeItem = parentItem->child(count);
				I* rowData = (treeItem ? VariantPointer<I>(treeItem->data(1, Qt::UserRole)) : NULL);
				if (rowData == dataItem)
				{
					// Update the current row and quit the loop
					(functionParent->*updateChildFunction)(parentItem, count, dataItem, itemIterator.currentData(), false);

					break;
				}
				else parentItem->removeChild(parentItem->child(count));
			}

			// If the current child index is (now) out of range, add a new item to the parent
			if (count == parentItem->childCount())
			{
				// Create new item
				(functionParent->*updateChildFunction)(parentItem, count, dataItem, itemIterator.currentData(), true);
			}

			++count;
		}

		// If there are still items remaining in the widget, delete them now
		while (count < parentItem->childCount()) parentItem->removeChild(parentItem->child(count));
	}
};

#endif
