// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "templates/list.h"
#include "templates/optionalref.h"
#include "templates/refdatalist.h"
#include "templates/reflist.h"
#include "templates/variantpointer.h"
#include <QTreeWidget>
#include <functional>

#pragma once

// TreeWidgetUpdater - Constructor-only template class to update QTreeWidget items, preserving original items as much as
// possible
template <class T, class I> class TreeWidgetUpdater
{
    // Typedefs for passed functions
    typedef void (T::*TreeWidgetTopLevelUpdateFunction)(QTreeWidget *treeWidget, int topLevelItemIndex, const I *data,
                                                        bool createItem);
    typedef void (T::*TreeWidgetTopLevelUpdateRefFunction)(QTreeWidget *treeWidget, int topLevelItemIndex, I &data,
                                                           bool createItem);
    typedef void (T::*TreeWidgetChildUpdateFunction)(QTreeWidgetItem *parentItem, int childIndex, const I *data,
                                                     bool createItem);
    typedef void (T::*TreeWidgetChildUpdateRefFunction)(QTreeWidgetItem *parentItem, int childIndex, I &data, bool createItem);
    typedef void (T::*TreeWidgetChildUpdateSharedFunction)(QTreeWidgetItem *parentItem, int childIndex,
                                                           std::shared_ptr<I> &data, bool createItem);

    private:
    // To update a tree, we need four helper functions and two values
    // The four helper functions are
    // accessor = a function to find the item at an index
    // counter = a function to find the current number of items in the tree
    // remover = a function to erase an item from an index
    // updater = a function to update an item at a location
    //
    // The two values are the index in question and the updated value
    // for the item.

    void genericTreeUpdater(std::function<QTreeWidgetItem *(int)> accessor, std::function<int()> counter,
                            std::function<void(int index, QTreeWidgetItem *)> remover,
                            std::function<void(int, const I *, bool)> updater, int count, const I *dataItem)
    {
        // Our QTreeWidgetItem may or may not be populated, and with different items to those in the list.

        // If there is an item already at this child position, check it
        // If it represents the current pointer data, just update it and move on. Otherwise, delete it and check
        // again
        while (count < counter())
        {
            QTreeWidgetItem *treeItem = accessor(count);
            I *rowData = (treeItem ? VariantPointer<I>(treeItem->data(0, Qt::UserRole)) : nullptr);
            if (rowData == dataItem)
            {
                // Update the current row and quit the loop
                updater(count, dataItem, false);

                break;
            }
            else
            {
                remover(count, treeItem);
                delete treeItem;
            }
        }

        // If the current child index is (now) out of range, add a new item to the parent
        if (count == counter())
        {
            // Create new item
            updater(count, dataItem, true);
        }

        ++count;
    }
    void genericTreeUpdater(std::function<QTreeWidgetItem *(int)> accessor, std::function<int()> counter,
                            std::function<void(int index, QTreeWidgetItem *)> remover,
                            std::function<void(int, I &, bool)> updater, int count, I &dataItem)
    {
        // Our QTreeWidgetItem may or may not be populated, and with different items to those in the list.

        // If there is an item already at this child position, check it
        // If it represents the current pointer data, just update it and move on. Otherwise, delete it and check
        // again
        while (count < counter())
        {
            QTreeWidgetItem *treeItem = accessor(count);
            I *rowData = (treeItem ? treeItem->data(0, Qt::UserRole).value<I *>() : nullptr);
            if (rowData == &dataItem)
            {
                // Update the current row and quit the loop
                updater(count, dataItem, false);

                break;
            }
            else
            {
                remover(count, treeItem);
                delete treeItem;
            }
        }

        // If the current child index is (now) out of range, add a new item to the parent
        if (count == counter())
        {
            // Create new item
            updater(count, dataItem, true);
        }

        ++count;
    }
    void genericTreeSharedUpdater(std::function<QTreeWidgetItem *(int)> accessor, std::function<int()> counter,
                                  std::function<void(int index, QTreeWidgetItem *)> remover,
                                  std::function<void(int, std::shared_ptr<I> &, bool)> updater, int count,
                                  std::shared_ptr<I> &dataItem)
    {
        // Our QTreeWidgetItem may or may not be populated, and with different items to those in the list.

        // If there is an item already at this child position, check it
        // If it represents the current pointer data, just update it and move on. Otherwise, delete it and check
        // again
        while (count < counter())
        {
            QTreeWidgetItem *treeItem = accessor(count);
            std::shared_ptr<I> rowData = (treeItem ? treeItem->data(0, Qt::UserRole).value<std::shared_ptr<I>>() : nullptr);
            if (rowData == dataItem)
            {
                // Update the current row and quit the loop
                updater(count, dataItem, false);

                break;
            }
            else
            {
                remover(count, treeItem);
                delete treeItem;
            }
        }

        // If the current child index is (now) out of range, add a new item to the parent
        if (count == counter())
        {
            // Create new item
            updater(count, dataItem, true);
        }

        ++count;
    }
    void updateTreeChildren(QTreeWidgetItem *parentItem, int count, I *dataItem, T *functionParent,
                            TreeWidgetChildUpdateFunction updateChildFunction)
    {
        genericTreeUpdater([parentItem](int index) { return parentItem->child(index); },
                           [parentItem]() { return parentItem->childCount(); },
                           [parentItem](int unused, auto child) { parentItem->removeChild(child); },
                           [parentItem, functionParent, updateChildFunction](int index, const I *item, bool something) {
                               return (functionParent->*updateChildFunction)(parentItem, index, item, something);
                           },
                           count, dataItem);
    }
    void updateTreeChildren(QTreeWidgetItem *parentItem, int count, const I &dataItem, T *functionParent,
                            TreeWidgetChildUpdateFunction updateChildFunction)
    {
        genericTreeUpdater([parentItem](int index) { return parentItem->child(index); },
                           [parentItem]() { return parentItem->childCount(); },
                           [parentItem](int unused, auto child) { parentItem->removeChild(child); },
                           [parentItem, functionParent, updateChildFunction](int index, const I *item, bool something) {
                               return (functionParent->*updateChildFunction)(parentItem, index, item, something);
                           },
                           count, &dataItem);
    }
    void updateTreeChildrenShared(QTreeWidgetItem *parentItem, int count, std::shared_ptr<I> &dataItem, T *functionParent,
                                  TreeWidgetChildUpdateSharedFunction updateChildFunction)
    {
        genericTreeSharedUpdater(
            [parentItem](int index) { return parentItem->child(index); }, [parentItem]() { return parentItem->childCount(); },
            [parentItem](int unused, auto child) { parentItem->removeChild(child); },
            [parentItem, functionParent, updateChildFunction](int index, std::shared_ptr<I> &item, bool something) {
                return (functionParent->*updateChildFunction)(parentItem, index, item, something);
            },
            count, dataItem);
    }
    void updateTreeTopLevel(QTreeWidget *treeWidget, int count, I *dataItem, T *functionParent,
                            TreeWidgetTopLevelUpdateFunction updateTopLevelFunction)
    {
        genericTreeUpdater([treeWidget](int index) { return treeWidget->topLevelItem(index); },
                           [treeWidget]() { return treeWidget->topLevelItemCount(); },
                           [treeWidget](int index, auto unused) { treeWidget->takeTopLevelItem(index); },
                           [treeWidget, functionParent, updateTopLevelFunction](int index, const I *item, bool something) {
                               return (functionParent->*updateTopLevelFunction)(treeWidget, index, item, something);
                           },
                           count, dataItem);
    }
    void updateTreeTopLevel(QTreeWidget *treeWidget, int count, I &dataItem, T *functionParent,
                            TreeWidgetTopLevelUpdateRefFunction updateTopLevelFunction)
    {
        genericTreeUpdater([treeWidget](int index) { return treeWidget->topLevelItem(index); },
                           [treeWidget]() { return treeWidget->topLevelItemCount(); },
                           [treeWidget](int index, auto unused) { treeWidget->takeTopLevelItem(index); },
                           [treeWidget, functionParent, updateTopLevelFunction](int index, I &item, bool something) {
                               return (functionParent->*updateTopLevelFunction)(treeWidget, index, item, something);
                           },
                           count, dataItem);
    }

    public:
    // Update the top-level items of the specified parent QTreeWidget
    TreeWidgetUpdater(QTreeWidget *treeWidget, const List<I> &data, T *functionParent,
                      TreeWidgetTopLevelUpdateFunction updateTopLevelFunction)
    {
        int count = 0;

        ListIterator<I> dataIterator(data);
        while (I *dataItem = dataIterator.iterate())
        {
            updateTreeTopLevel(treeWidget, count, dataItem, functionParent, updateTopLevelFunction);
            ++count;
        }

        // If there are still items remaining in the widget, delete them now
        while (count < treeWidget->topLevelItemCount())
        {
            QTreeWidgetItem *item = treeWidget->takeTopLevelItem(count);
            delete item;
        }
    }

    // Update the children of the specified parent QTreeWidgetItem
    TreeWidgetUpdater(QTreeWidgetItem *parentItem, const List<I> &data, T *functionParent,
                      TreeWidgetChildUpdateFunction updateChildFunction)
    {
        int count = 0;

        ListIterator<I> dataIterator(data);
        while (I *dataItem = dataIterator.iterate())
            updateTreeChildren(parentItem, count++, dataItem, functionParent, updateChildFunction);

        // If there are still items remaining in the widget, delete them now
        while (count < parentItem->childCount())
            parentItem->removeChild(parentItem->child(count));
    }

    // Update the children of the specified parent QTreeWidgetItem
    TreeWidgetUpdater(QTreeWidgetItem *parentItem, const RefList<I> &data, T *functionParent,
                      TreeWidgetChildUpdateFunction updateChildFunction)
    {
        int count = 0;

        for (I *dataItem : data)
            updateTreeChildren(parentItem, count++, dataItem, functionParent, updateChildFunction);

        // If there are still items remaining in the widget, delete them now
        while (count < parentItem->childCount())
            parentItem->removeChild(parentItem->child(count));
    }

    // Update the top-level items of the specified parent QTreeWidget
    TreeWidgetUpdater(QTreeWidget *treeWidget, std::vector<I> &data, T *functionParent,
                      TreeWidgetTopLevelUpdateRefFunction updateTopLevelFunction)
    {
        int count = 0;

        for (auto &dataItem : data)
            updateTreeTopLevel(treeWidget, count++, dataItem, functionParent, updateTopLevelFunction);

        // If there are still items remaining in the widget, delete them now
        while (count < treeWidget->topLevelItemCount())
        {
            QTreeWidgetItem *item = treeWidget->takeTopLevelItem(count);
            delete item;
        }
    }

    // Update the top-level items of the specified parent QTreeWidget
    TreeWidgetUpdater(QTreeWidget *treeWidget, std::list<I> &data, T *functionParent,
                      TreeWidgetTopLevelUpdateRefFunction updateTopLevelFunction)
    {
        int count = 0;

        for (auto &dataItem : data)
            updateTreeTopLevel(treeWidget, count++, dataItem, functionParent, updateTopLevelFunction);

        // If there are still items remaining in the widget, delete them now
        while (count < treeWidget->topLevelItemCount())
        {
            QTreeWidgetItem *item = treeWidget->takeTopLevelItem(count);
            delete item;
        }
    }

    // Update the children of the specified parent QTreeWidgetItem
    TreeWidgetUpdater(QTreeWidgetItem *parentItem, const std::vector<I> &data, T *functionParent,
                      TreeWidgetChildUpdateFunction updateChildFunction)
    {
        int count = 0;

        for (auto &dataItem : data)
            updateTreeChildren(parentItem, count++, dataItem, functionParent, updateChildFunction);

        // If there are still items remaining in the widget, delete them now
        while (count < parentItem->childCount())
            parentItem->removeChild(parentItem->child(count));
    }

    // Update the children of the specified parent QTreeWidgetItem
    TreeWidgetUpdater(QTreeWidgetItem *parentItem, const std::list<I> &data, T *functionParent,
                      TreeWidgetChildUpdateFunction updateChildFunction)
    {
        int count = 0;

        for (auto &dataItem : data)
            updateTreeChildren(parentItem, count++, dataItem, functionParent, updateChildFunction);

        // If there are still items remaining in the widget, delete them now
        while (count < parentItem->childCount())
            parentItem->removeChild(parentItem->child(count));
    }
    // Update the children of the specified parent QTreeWidgetItem
    TreeWidgetUpdater(QTreeWidgetItem *parentItem, std::vector<std::shared_ptr<I>> &data, T *functionParent,
                      TreeWidgetChildUpdateSharedFunction updateChildFunction)
    {
        int count = 0;

        for (auto &dataItem : data)
            updateTreeChildrenShared(parentItem, count++, dataItem, functionParent, updateChildFunction);

        // If there are still items remaining in the widget, delete them now
        while (count < parentItem->childCount())
            parentItem->removeChild(parentItem->child(count));
    }
};

// TreeWidgetRefListUpdater - Constructor-only template class to update contents of a QTreeWidget from a RefDataList, preserving
// original items as much as possible
template <class T, class I, class D> class TreeWidgetRefDataListUpdater
{
    // Typedefs for passed functions
    typedef void (T::*TreeWidgetChildUpdateFunction)(QTreeWidgetItem *parentItem, int childIndex, I item, D data,
                                                     bool createItem);

    public:
    TreeWidgetRefDataListUpdater(QTreeWidgetItem *parentItem, const std::vector<std::tuple<I, D>> &list, T *functionParent,
                                 TreeWidgetChildUpdateFunction updateChildFunction)
    {
        QTreeWidgetItem *treeItem;

        int count = 0;

        for (auto [dataItem, dataData] : list)
        {
            // Our QTreeWidgetItem may or may not be populated, and with different items to those in the list.

            // If there is an item already at this child position, check it
            // If it represents the current pointer data, just update it and move on. Otherwise, delete it and check
            // again
            while (count < parentItem->childCount())
            {
                treeItem = parentItem->child(count);
                I rowData = (treeItem ? treeItem->data(1, Qt::UserRole).value<I>() : nullptr);
                if (rowData == dataItem)
                {
                    // Update the current row and quit the loop
                    (functionParent->*updateChildFunction)(parentItem, count, dataItem, dataData, false);

                    break;
                }
                else
                    parentItem->removeChild(parentItem->child(count));
            }

            // If the current child index is (now) out of range, add a new item to the parent
            if (count == parentItem->childCount())
            {
                // Create new item
                (functionParent->*updateChildFunction)(parentItem, count, dataItem, dataData, true);
            }

            ++count;
        }

        // If there are still items remaining in the widget, delete them now
        while (count < parentItem->childCount())
            parentItem->removeChild(parentItem->child(count));
    }
};

// Tree Widget Top Level Item Manager
template <class P, class T> class TreeWidgetItemManager
{
    // Typedef for function pointers
    typedef void (P::*TreeWidgetItemUpdateFunction)(QTreeWidgetItem *item, T &data, bool itemIsNew);

    private:
    // Parent object for function pointers
    P *functionParent_;

    public:
    TreeWidgetItemManager(P *functionParent) : functionParent_(functionParent){};

    /*
     * Data References
     */
    private:
    // Map of references to objects represented by items anywhere in the tree, keyed by QTreeWidgetItem
    std::map<QTreeWidgetItem *, std::reference_wrapper<T>> referenceMap_;

    public:
    // Return whether the specified QTreeWidgetItem is mapped to a reference
    bool isMapped(QTreeWidgetItem *item) const { return (referenceMap_.find(item) != referenceMap_.end()); }
    // Retrieve reference associated to specified QTreeWidgetItem
    OptionalReferenceWrapper<T> reference(QTreeWidgetItem *item) const
    {
        auto it = referenceMap_.find(item);
        if (it == referenceMap_.end())
            return {};

        return it->second;
    }

    /*
     * Update Functions
     */
    public:
    // Perform update top level items, making them consistent with the supplied vector
    void update(QTreeWidget *treeWidget, std::vector<T> &items, TreeWidgetItemUpdateFunction itemUpdateFunction)
    {
        int count = 0;

        for (auto &dataItem : items)
        {
            // Our QTreeWidget may or may not be populated, and with different items to those in the list.

            // If there is an item already at this top level position, check its reference against that in our
            // vector If it is the same identity, just call the update function and move on. Otherwise, delete it
            // and check again
            while (count < treeWidget->topLevelItemCount())
            {
                QTreeWidgetItem *treeItem = treeWidget->topLevelItem(count);

                // Does our old map contain a referenced value for this tree item?
                auto it = referenceMap_.find(treeItem);
                if (it != referenceMap_.end())
                {
                    T &referencedData = it->second;

                    // Check consistency of the referenced value in the map with the current data
                    if (&referencedData == &dataItem)
                    {
                        // Update the item at the current position and quit the loop
                        (functionParent_->*itemUpdateFunction)(treeItem, dataItem, false);

                        break;
                    }
                    else
                    {
                        treeWidget->takeTopLevelItem(count);
                        delete treeItem;

                        referenceMap_.erase(it);
                    }
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
                // Create new item and update it
                QTreeWidgetItem *treeItem = new QTreeWidgetItem;
                treeWidget->insertTopLevelItem(count, treeItem);
                (functionParent_->*itemUpdateFunction)(treeItem, dataItem, true);

                // Add reference to the internal vector
                referenceMap_.insert(std::make_pair(treeItem, std::reference_wrapper<T>(dataItem)));
            }

            ++count;
        }

        // If there are still items remaining in the tree, delete them now
        while (count < treeWidget->topLevelItemCount())
        {
            QTreeWidgetItem *treeItem = treeWidget->takeTopLevelItem(count);
            referenceMap_.erase(treeItem);
            delete treeItem;
        }
    }
    // Perform update on children of the specified item, making them consistent with the supplied vector
    void updateChildren(QTreeWidgetItem *parentItem, std::vector<T> &items, TreeWidgetItemUpdateFunction itemUpdateFunction)
    {
        int count = 0;

        for (auto &dataItem : items)
        {
            // Our QTreeWidgetItem may or may not have children, and with different items to those in the list.

            // If there is an item already at this child position, check it
            // If it represents the current pointer data, just update it and move on. Otherwise, delete it and check
            // again
            while (count < parentItem->childCount())
            {
                QTreeWidgetItem *treeItem = parentItem->child(count);

                // Does our old map contain a referenced value for this tree item?
                auto it = referenceMap_.find(treeItem);
                if (it != referenceMap_.end())
                {
                    const T &referencedData = it->second;

                    if (&referencedData == &dataItem)
                    {
                        // Update the current row and quit the loop
                        (functionParent_->*itemUpdateFunction)(treeItem, dataItem, false);

                        break;
                    }
                    else
                    {
                        parentItem->removeChild(treeItem);
                        delete treeItem;

                        referenceMap_.erase(it);
                    }
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
                // Create new item and update it
                QTreeWidgetItem *treeItem = new QTreeWidgetItem;
                parentItem->insertChild(count, treeItem);
                (functionParent_->*itemUpdateFunction)(treeItem, dataItem, true);

                // Add reference to the internal vector
                referenceMap_.insert(std::make_pair(treeItem, std::reference_wrapper<T>(dataItem)));
            }

            ++count;
        }

        // If there are still items remaining in the parent, delete them now
        while (count < parentItem->childCount())
        {
            QTreeWidgetItem *treeItem = parentItem->child(count);
            parentItem->removeChild(treeItem);
            referenceMap_.erase(treeItem);
        }
    }
};
