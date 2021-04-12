// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "templates/list.h"
#include "templates/reflist.h"
#include "templates/variantpointer.h"
#include <QListWidget>
#include <memory>

#pragma once

// ListWidgetUpdater - Constructor-only template class to update contents of a QListWidget, preserving original items as much as
// possible
template <class T, class I> class ListWidgetUpdater
{
    // Typedefs for passed functions
    typedef void (T::*ListWidgetRowUpdateFunction)(int row, I *item, bool createItem);
    typedef void (T::*ListWidgetRowUpdateRefFunction)(int row, I &item, bool createItem);
    typedef void (T::*ListWidgetRowUpdateSharedFunction)(int row, std::shared_ptr<I> item, bool createItem);

    public:
    // Update widget from supplied List, calling supplied function to create / modify data
    ListWidgetUpdater(QListWidget *listWidget, const List<I> &data, T *functionParent, ListWidgetRowUpdateFunction updateRow)
    {
        QListWidgetItem *listWidgetItem;

        int currentRow = 0;

        ListIterator<I> dataIterator(data);
        while (I *dataItem = dataIterator.iterate())
        {
            // Our table may or may not be populated, and with different items to those in the list.

            // If there is an item already on this row, check it
            // If it represents the current pointer data, just update it and move on. Otherwise, delete it and check
            // again
            while (currentRow < listWidget->count())
            {
                listWidgetItem = listWidget->item(currentRow);
                I *rowData = (listWidgetItem ? VariantPointer<I>(listWidgetItem->data(Qt::UserRole)) : nullptr);
                if (rowData == dataItem)
                {
                    // Update the current row and quit the loop
                    (functionParent->*updateRow)(currentRow, dataItem, false);

                    break;
                }
                else
                {
                    QListWidgetItem *oldItem = listWidget->takeItem(currentRow);
                    if (oldItem)
                        delete oldItem;
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

        // If there are still rows remaining in the widget, delete them now
        while (currentRow < listWidget->count())
        {
            QListWidgetItem *oldItem = listWidget->takeItem(currentRow);
            if (oldItem)
                delete oldItem;
        }
    }

    // Update widget from supplied List, calling supplied function to create / modify data
    ListWidgetUpdater(QListWidget *listWidget, const List<I> &data, T *functionParent, ListWidgetRowUpdateRefFunction updateRow)
    {
        QListWidgetItem *listWidgetItem;

        int currentRow = 0;

        ListIterator<I> dataIterator(data);
        while (I *dataItem = dataIterator.iterate())
        {
            // Our table may or may not be populated, and with different items to those in the list.

            // If there is an item already on this row, check it
            // If it represents the current pointer data, just update it and move on. Otherwise, delete it and check
            // again
            while (currentRow < listWidget->count())
            {
                listWidgetItem = listWidget->item(currentRow);
                I *rowData = (listWidgetItem ? VariantPointer<I>(listWidgetItem->data(Qt::UserRole)) : nullptr);
                if (rowData == dataItem)
                {
                    // Update the current row and quit the loop
                    (functionParent->*updateRow)(currentRow, *dataItem, false);

                    break;
                }
                else
                {
                    QListWidgetItem *oldItem = listWidget->takeItem(currentRow);
                    if (oldItem)
                        delete oldItem;
                }
            }

            // If the current row index is (now) out of range, add a new row to the list
            if (currentRow == listWidget->count())
            {
                // Create new items
                (functionParent->*updateRow)(currentRow, *dataItem, true);
            }

            ++currentRow;
        }

        // If there are still rows remaining in the widget, delete them now
        while (currentRow < listWidget->count())
        {
            QListWidgetItem *oldItem = listWidget->takeItem(currentRow);
            if (oldItem)
                delete oldItem;
        }
    }

    // Update widget from supplied List, calling supplied function to create / modify data
    ListWidgetUpdater(QListWidget *listWidget, const std::vector<std::shared_ptr<I>> &data, T *functionParent,
                      ListWidgetRowUpdateSharedFunction updateRow)
    {
        QListWidgetItem *listWidgetItem;

        int currentRow = 0;

        for (auto dataItem : data)
        {
            // Our table may or may not be populated, and with different items to those in the list.

            // If there is an item already on this row, check it
            // If it represents the current pointer data, just update it and move on. Otherwise, delete it and check
            // again
            while (currentRow < listWidget->count())
            {
                std::shared_ptr<I> rowData;
                listWidgetItem = listWidget->item(currentRow);
                if (listWidgetItem)
                {
                    rowData = listWidgetItem->data(Qt::UserRole).value<std::shared_ptr<I>>();
                }
                else
                {
                    rowData = nullptr;
                }

                if (rowData == dataItem)
                {
                    // Update the current row and quit the loop
                    (functionParent->*updateRow)(currentRow, dataItem, false);

                    break;
                }
                else
                {
                    QListWidgetItem *oldItem = listWidget->takeItem(currentRow);
                    if (oldItem)
                        delete oldItem;
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

        // If there are still rows remaining in the widget, delete them now
        while (currentRow < listWidget->count())
        {
            QListWidgetItem *oldItem = listWidget->takeItem(currentRow);
            if (oldItem)
                delete oldItem;
        }
    }
    // Update widget from supplied List, calling supplied function to create / modify data
    ListWidgetUpdater(QListWidget *listWidget, const std::vector<std::unique_ptr<I>> &data, T *functionParent,
                      ListWidgetRowUpdateFunction updateRow)
    {
        QListWidgetItem *listWidgetItem;
        int currentRow = 0;

        for (const auto &dataItem : data)
        {
            // Our table may or may not be populated, and with different items to those in the list.

            // If there is an item already on this row, check it
            // If it represents the current pointer data, just update it and move on. Otherwise, delete it and check
            // again
            while (currentRow < listWidget->count())
            {
                I *rowData;
                listWidgetItem = listWidget->item(currentRow);
                if (listWidgetItem)
                {
                    rowData = (listWidgetItem ? VariantPointer<I>(listWidgetItem->data(Qt::UserRole)) : nullptr);
                }
                if (rowData == dataItem.get())
                {
                    // Update the current row and quit the loop
                    (functionParent->*updateRow)(currentRow, dataItem.get(), false);

                    break;
                }
                else
                {
                    QListWidgetItem *oldItem = listWidget->takeItem(currentRow);
                    if (oldItem)
                        delete oldItem;
                }
            }

            // If the current row index is (now) out of range, add a new row to the list
            if (currentRow == listWidget->count())
            {
                // Create new items
                (functionParent->*updateRow)(currentRow, dataItem.get(), true);
            }

            ++currentRow;
        }

        // If there are still rows remaining in the widget, delete them now
        while (currentRow < listWidget->count())
        {
            QListWidgetItem *oldItem = listWidget->takeItem(currentRow);
            if (oldItem)
                delete oldItem;
        }
    }

    // Update widget from supplied List, assuming that the name() function in class I is the desired text to show in the
    // list
    ListWidgetUpdater(QListWidget *listWidget, const List<I> &list, Qt::ItemFlags flags = Qt::NoItemFlags,
                      I *currentItem = nullptr)
    {
        QListWidgetItem *listWidgetItem;

        int currentRow = 0;

        ListIterator<I> dataIterator(list);
        while (I *dataItem = dataIterator.iterate())
        {
            // Our table may or may not be populated, and with different items to those in the list.

            // If there is an item already on this row, check it
            // If it represents the current pointer data, just update it and move on. Otherwise, delete it and check
            // again
            while (currentRow < listWidget->count())
            {
                listWidgetItem = listWidget->item(currentRow);
                I *rowData = (listWidgetItem ? VariantPointer<I>(listWidgetItem->data(Qt::UserRole)) : nullptr);
                if (rowData == dataItem)
                {
                    // Update the current row and quit the loop
                    listWidgetItem->setText(QString::fromStdString(std::string(dataItem->name())));

                    break;
                }
                else
                {
                    QListWidgetItem *oldItem = listWidget->takeItem(currentRow);
                    if (oldItem)
                        delete oldItem;
                }
            }

            // If the current row index is (now) out of range, add a new row to the list
            if (currentRow == listWidget->count())
            {
                // Create new items
                listWidgetItem = new QListWidgetItem;
                listWidget->addItem(listWidgetItem);
                listWidgetItem->setData(Qt::UserRole, VariantPointer<I>(dataItem));
                listWidgetItem->setText(QString::fromStdString(std::string(dataItem->name())));
                if (flags != Qt::NoItemFlags)
                    listWidgetItem->setFlags(listWidgetItem->flags() | flags);
            }

            // Is this the current item?
            if (currentItem == dataItem)
                listWidget->setCurrentRow(currentRow);

            ++currentRow;
        }

        // If there are still rows remaining in the widget, delete them now
        while (currentRow < listWidget->count())
        {
            QListWidgetItem *oldItem = listWidget->takeItem(currentRow);
            if (oldItem)
                delete oldItem;
        }
    }
    // Update widget from supplied List, assuming that the name() function in class I is the desired text to show in the
    // list
    ListWidgetUpdater(QListWidget *listWidget, const std::vector<std::unique_ptr<I>> &data,
                      Qt::ItemFlags flags = Qt::NoItemFlags, I *currentItem = nullptr)
    {
        QListWidgetItem *listWidgetItem;
        int currentRow = 0;

        for (const auto &dataItem : data)
        {
            // Our table may or may not be populated, and with different items to those in the list.

            // If there is an item already on this row, check it
            // If it represents the current pointer data, just update it and move on. Otherwise, delete it and check
            // again
            while (currentRow < listWidget->count())
            {
                listWidgetItem = listWidget->item(currentRow);
                I *rowData = (listWidgetItem ? VariantPointer<I>(listWidgetItem->data(Qt::UserRole)) : nullptr);
                if (rowData == dataItem.get())
                {
                    // Update the current row and quit the loop
                    listWidgetItem->setText(QString::fromStdString(std::string(dataItem->name())));

                    break;
                }
                else
                {
                    QListWidgetItem *oldItem = listWidget->takeItem(currentRow);
                    if (oldItem)
                        delete oldItem;
                }
            }

            // If the current row index is (now) out of range, add a new row to the list
            if (currentRow == listWidget->count())
            {
                // Create new items
                listWidgetItem = new QListWidgetItem;
                listWidget->addItem(listWidgetItem);
                listWidgetItem->setData(Qt::UserRole, VariantPointer<I>(dataItem.get()));
                listWidgetItem->setText(QString::fromStdString(std::string(dataItem->name())));
                if (flags != Qt::NoItemFlags)
                    listWidgetItem->setFlags(listWidgetItem->flags() | flags);
            }

            // Is this the current item?
            if (currentItem == dataItem.get())
                listWidget->setCurrentRow(currentRow);

            ++currentRow;
        }

        // If there are still rows remaining in the widget, delete them now
        while (currentRow < listWidget->count())
        {
            QListWidgetItem *oldItem = listWidget->takeItem(currentRow);
            if (oldItem)
                delete oldItem;
        }
    }

    ListWidgetUpdater(QListWidget *listWidget, const std::vector<I> &data, Qt::ItemFlags flags = Qt::NoItemFlags,
                      I *currentItem = nullptr)
    {
        QListWidgetItem *listWidgetItem;
        int currentRow = 0;
        for (const auto &dataItem : data)
        {
            // Our table may or may not be populated, and with different items to those in the list.

            // If there is an item already on this row, check it
            // If it represents the current pointer data, just update it and move on. Otherwise, delete it and check
            // again
            while (currentRow < listWidget->count())
            {
                listWidgetItem = listWidget->item(currentRow);
                I *rowData = (listWidgetItem ? VariantPointer<I>(listWidgetItem->data(Qt::UserRole)) : nullptr);
                if (rowData == &dataItem)
                {
                    // Update the current row and quit the loop
                    listWidgetItem->setText(QString::fromStdString(std::string(dataItem.name())));

                    break;
                }
                else
                {
                    QListWidgetItem *oldItem = listWidget->takeItem(currentRow);
                    if (oldItem)
                        delete oldItem;
                }
            }

            // If the current row index is (now) out of range, add a new row to the list
            if (currentRow == listWidget->count())
            {
                // Create new items
                listWidgetItem = new QListWidgetItem;
                listWidget->addItem(listWidgetItem);
                listWidgetItem->setData(Qt::UserRole, VariantPointer<I>(&dataItem));
                listWidgetItem->setText(QString::fromStdString(std::string(dataItem.name())));
                if (flags != Qt::NoItemFlags)
                    listWidgetItem->setFlags(listWidgetItem->flags() | flags);
            }

            // Is this the current item?
            if (currentItem == &dataItem)
                listWidget->setCurrentRow(currentRow);

            ++currentRow;
        }

        // If there are still rows remaining in the widget, delete them now
        while (currentRow < listWidget->count())
        {
            QListWidgetItem *oldItem = listWidget->takeItem(currentRow);
            if (oldItem)
                delete oldItem;
        }
    }

    // Update widget from supplied RefList, calling supplied function to create / modify data
    ListWidgetUpdater(QListWidget *listWidget, const RefList<I> &list, T *functionParent, ListWidgetRowUpdateFunction updateRow)
    {
        QListWidgetItem *listWidgetItem;

        int currentRow = 0;

        for (I *dataItem : list)
        {
            // Our table may or may not be populated, and with different items to those in the list.

            // If there is an item already on this row, check it
            // If it represents the current pointer data, just update it and move on. Otherwise, delete it and check
            // again
            while (currentRow < listWidget->count())
            {
                listWidgetItem = listWidget->item(currentRow);
                I *rowData = (listWidgetItem ? VariantPointer<I>(listWidgetItem->data(Qt::UserRole)) : nullptr);
                if (rowData == dataItem)
                {
                    // Update the current row and quit the loop
                    (functionParent->*updateRow)(currentRow, dataItem, false);

                    break;
                }
                else
                {
                    QListWidgetItem *oldItem = listWidget->takeItem(currentRow);
                    if (oldItem)
                        delete oldItem;
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

        // If there are still rows remaining in the widget, delete them now
        while (currentRow < listWidget->count())
        {
            QListWidgetItem *oldItem = listWidget->takeItem(currentRow);
            if (oldItem)
                delete oldItem;
        }
    }
};
