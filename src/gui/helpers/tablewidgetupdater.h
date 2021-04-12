// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "templates/array.h"
#include "templates/list.h"
#include "templates/refdatalist.h"
#include "templates/reflist.h"
#include <QTableWidget>
#include <memory>

#pragma once

// ConstTableWidgetUpdater - Constructor-only template class to update contents of a QTableWidget, preserving original items as
// much as possible
template <class T, class I, typename Raw = const I *, typename... Args> class ConstTableWidgetUpdater
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
    ConstTableWidgetUpdater(QTableWidget *table, const std::vector<I> &vector, T *functionParent,
                            TableWidgetRowUpdateFunction updateRow)
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
    ConstTableWidgetUpdater(QTableWidget *table, const std::list<I> &vector, T *functionParent,
                            TableWidgetRowUpdateFunction updateRow)
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
    ConstTableWidgetUpdater(QTableWidget *table, const List<I> &list, T *functionParent, TableWidgetRowUpdateFunction updateRow)
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
    ConstTableWidgetUpdater(QTableWidget *table, const RefList<I> &list, T *functionParent,
                            TableWidgetRowUpdateFunction updateRow)
    {
        int rowCount = 0;

        for (I *dataItem : list)
        {
            updateItemAtIndex(table, rowCount, dataItem, functionParent, updateRow);
            ++rowCount;
        }
        table->setRowCount(rowCount);
    }
    ConstTableWidgetUpdater(QTableWidget *table, const std::vector<std::shared_ptr<I>> &list, T *functionParent,
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
};

// TableWidgetUpdater - Constructor-only template class to update contents of a QTableWidget, preserving original items as
// much as possible. This is present as a temporary fix whilst the GUI transitions to MVC-style operation.
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
    TableWidgetUpdater(QTableWidget *table, std::list<I> &list, T *functionParent, TableWidgetRowUpdateFunction updateRow)
    {
        int rowCount = 0;
        for (auto &dataItem : list)
        {
            updateItemAtIndex(table, rowCount, &dataItem, functionParent, updateRow);
            ++rowCount;
        }
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
    TableWidgetUpdater(QTableWidget *table, std::vector<std::shared_ptr<I>> &list, T *functionParent,
                       TableWidgetRowUpdateFunction updateRow)
    {

        int rowCount = 0;

        for (const auto dataItem : list)
        {
            updateItemAtIndex(table, rowCount, dataItem, functionParent, updateRow);
            ++rowCount;
        }

        // Set the number of table rows again here in order to catch the case where there were zero data items to
        // iterate over
        table->setRowCount(rowCount);
    }
};
