/*
    *** TableWidgetUpdater
    *** src/gui/helpers/tablewidgetupdater.h
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

#include "templates/array.h"
#include "templates/dynamicarray.h"
#include "templates/list.h"
#include "templates/refdatalist.h"
#include "templates/reflist.h"
#include <QTableWidget>
#include <memory>

#pragma once

// TableWidgetUpdater - Constructor-only template class to update contents of a QTableWidget, preserving original items as much
// as possible
template <class T, class I, typename Raw = I *, typename... Args> class TableWidgetUpdater
{
    // Typedefs for passed functions
    typedef void (T::*TableWidgetRowUpdateFunction)(int row, Raw item, bool createItems, Args... args);

    private:
    // For a given QTableWidget, ensure that the given dataItem is at
    // the given rowCount index.  If there are other items in the way,
    // remove them.  If the item isn't in the table, add it.  Finally,
    // updateRow.
    static void updateItemAtIndex(QTableWidget *table, int rowCount, Raw dataItem, T *functionParent,
                                  TableWidgetRowUpdateFunction updateRow, Args... args)
    {
        // Our table may or may not be populated, and with different items to those in the list.

        // If there is an item already on this row, check it
        // If it represents the current pointer data, just update it and move on. Otherwise, delete it and check
        // again
        while (rowCount < table->rowCount())
        {
            auto tableItem = table->item(rowCount, 0);
            auto rowData = (tableItem ? tableItem->data(Qt::UserRole).value<Raw>() : nullptr);
            if (rowData == dataItem)
            {
                // Update the current row and quit the loop
                (functionParent->*updateRow)(rowCount, dataItem, false, args...);

                break;
            }
            else
                table->removeRow(rowCount);
        }

        // If the current row index is (now) out of range, add a new row to the table
        if (rowCount == table->rowCount())
        {
            // Increase row count
            table->setRowCount(rowCount + 1);

            // Create new items
            (functionParent->*updateRow)(rowCount, dataItem, true);
        }
    }

    public:
    TableWidgetUpdater(QTableWidget *table, std::vector<I> &vector, T *functionParent, TableWidgetRowUpdateFunction updateRow)
    {

        int rowCount = 0;

        for (auto &dataItem : vector)
        {
            updateItemAtIndex(table, rowCount, &dataItem, functionParent, updateRow);
            ++rowCount;
        }

        // Set the number of table rows again here in order to catch the case where there were zero data items to
        // iterate over
        table->setRowCount(rowCount);
    }
    TableWidgetUpdater(QTableWidget *table, const List<I> &list, T *functionParent, TableWidgetRowUpdateFunction updateRow)
    {

        int rowCount = 0;

        ListIterator<I> dataIterator(list);
        while (I *dataItem = dataIterator.iterate())
        {
            updateItemAtIndex(table, rowCount, dataItem, functionParent, updateRow);
            ++rowCount;
        }

        // Set the number of table rows again here in order to catch the case where there were zero data items to
        // iterate over
        table->setRowCount(rowCount);
    }
    TableWidgetUpdater(QTableWidget *table, const RefList<I> &list, T *functionParent, TableWidgetRowUpdateFunction updateRow)
    {
        int rowCount = 0;

        for (I *dataItem : list)
        {
            updateItemAtIndex(table, rowCount, dataItem, functionParent, updateRow);
            ++rowCount;
        }
        table->setRowCount(rowCount);
    }
    TableWidgetUpdater(QTableWidget *table, DynamicArray<I> &array, T *functionParent, TableWidgetRowUpdateFunction updateRow)
    {
        int rowCount = 0;

        DynamicArrayIterator<I> dataIterator(array);
        while (I *dataItem = dataIterator.iterate())
        {
            updateItemAtIndex(table, rowCount, dataItem, functionParent, updateRow);
            ++rowCount;
        }

        // Set the number of table rows again here in order to catch the case where there were zero data items to
        // iterate over
        table->setRowCount(rowCount);
    }
    TableWidgetUpdater(QTableWidget *table, const std::vector<std::shared_ptr<I>> &list, T *functionParent,
                       TableWidgetRowUpdateFunction updateRow)
    {

        int rowCount = 0;

        for (auto dataItem : list)
        {
            updateItemAtIndex(table, rowCount, dataItem, functionParent, updateRow);
            ++rowCount;
        }

        // Set the number of table rows again here in order to catch the case where there were zero data items to
        // iterate over
        table->setRowCount(rowCount);
    }
    template <typename D>
    TableWidgetUpdater(QTableWidget *table, RefDataList<I, D> &list, T *functionParent, TableWidgetRowUpdateFunction updateRow)
    {
        int rowCount = 0;

        auto itemIterator(list);
        while (I *dataItem = itemIterator.iterate())
        {
            updateItemAtIndex(table, rowCount, dataItem, functionParent, updateRow, itemIterator.currentData());
            ++rowCount;
        }
    }
};
