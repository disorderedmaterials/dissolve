// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/sysfunc.h"
#include "genericitems/container.h"
#include "genericitems/items.h"
#include "templates/list.h"
#include "templates/reflist.h"

// Generic List
class GenericList
{
    /*
     * List Contents
     */
    private:
    // List of generic items
    List<GenericItem> items_;

    public:
    // Clear all items (except those that are marked protected)
    void clear();
    // Clear all items, including protected items
    void clearAll();
    // Add specified item to list (from base class pointer)
    void add(GenericItem *item);
    // Create an item of the specified type
    GenericItem *create(std::string_view name, std::string_view itemClassName, int version = 0, int flags = 0);
    // Return whether the named item is contained in the list
    bool contains(std::string_view name, std::string_view prefix = "") const;
    // Return item list
    List<GenericItem> &items();
    // Return the named item from the list
    GenericItem *find(std::string_view name);
    const GenericItem *find(std::string_view name) const;
    // Return the named item from the list (with prefix)
    GenericItem *find(std::string_view name, std::string_view prefix);
    // Return the version of the named item from the list
    int version(std::string_view name, std::string_view prefix = "") const;
    // Remove named item
    bool remove(std::string_view name, std::string_view prefix);
    // Rename item
    bool rename(std::string_view oldName, std::string_view oldPrefix, std::string_view newName, std::string_view newPrefix);
    // Prune all items with '@suffix'
    void pruneWithSuffix(std::string_view suffix);

    /*
     * Item Retrieval
     */
    public:
    // Add new named item of template-guided type to specified list
    template <class T> T &add(std::string_view name, std::string_view prefix = "", int flags = -1)
    {
        // Construct full name
        std::string varName = prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name);

        // Does the named variable already exist in the list?
        auto *existingItem = find(varName);
        if (existingItem)
        {
            Messenger::warn("Item '{}' already exists in the list - a dummy value will be returned instead.\n", varName);
            static T dummy;
            return dummy;
        }

        // Create new item
        auto *newItem = new GenericItemContainer<T>(varName);
        if (flags >= 0)
            newItem->setFlags(flags);
        add(newItem);
        return newItem->data();
    }
    // Return named (const) item from specified list as template-guided type
    template <class T>
    const T &value(std::string_view name, std::string_view prefix = "", T defaultValue = T(), bool *found = nullptr) const
    {
        // Construct full name
        std::string varName = prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name);

        // Find item in the list
        auto *item = find(varName);
        if (!item)
        {
            Messenger::printVerbose("No item named '{}' in list - default value item will be returned.\n", varName);
            static T dummy;
            dummy = defaultValue;
            if (found != nullptr)
                (*found) = false;
            return dummy;
        }

        // Cast to correct type
        auto *castItem = dynamic_cast<const GenericItemContainer<T> *>(item);
        if (!castItem)
            throw std::runtime_error(
                fmt::format("GenericList.value({}) failed, because the target item is of the wrong type.", name));

        if (found != nullptr)
            (*found) = true;
        return castItem->data();
    }
    // Retrieve named item from specified list as template-guided type, assuming that it is going to be modified
    template <class T>
    T &retrieve(std::string_view name, std::string_view prefix = "", T defaultValue = T(), bool *found = nullptr)
    {
        // Construct full name
        std::string varName = prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name);

        // Find item in the list
        auto *item = find(varName);
        if (!item)
        {
            Messenger::printVerbose("No item named '{}' in list - default value item will be returned.\n", varName);
            static T dummy;
            dummy = defaultValue;
            if (found != nullptr)
                (*found) = false;
            return dummy;
        }

        // Cast to correct type
        auto *castItem = dynamic_cast<GenericItemContainer<T> *>(item);
        if (!castItem)
            throw std::runtime_error(
                fmt::format("GenericList.retrieve({}) failed, because the item is not of the specified type.", name));

        // Bump the version of the item
        item->bumpVersion();

        if (found != nullptr)
            (*found) = true;
        return castItem->data();
    }
    // Create or retrieve named item from specified list as template-guided type
    template <class T> T &realise(std::string_view name, std::string_view prefix = "", int flags = -1, bool *created = nullptr)
    {
        // Construct full name
        std::string varName = prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name);

        // Find item in the list - if it isn't there, create it and return
        auto *item = find(varName);
        if (!item)
        {
            if (created != nullptr)
                (*created) = true;
            return add<T>(name, prefix, flags);
        }

        // Cast to correct type
        auto *castItem = dynamic_cast<GenericItemContainer<T> *>(item);
        if (!castItem)
            throw std::runtime_error(
                fmt::format("GenericList.realise({}) failed, because the item couldn't be cast to the desired type.", name));

        // Update flags
        if (flags >= 0)
            item->setFlags(flags);

        // Bump the version of the item
        item->bumpVersion();

        if (created != nullptr)
            (*created) = false;
        return castItem->data();
    }
    // Create or retrieve named item from specified list as template-guided type
    template <class T> RefList<T> items() const
    {
        RefList<T> items;
        ListIterator<GenericItem> itemIterator(items_);
        while (GenericItem *item = itemIterator.iterate())
            if (DissolveSys::sameString(item->itemClassName(), T::itemClassName()))
            {
                // Cast to correct type
                auto *castItem = dynamic_cast<GenericItemContainer<T> *>(item);
                if (!castItem)
                    throw std::runtime_error(
                        fmt::format("GenericList.items() failed to retrieve item {} as it is not actually class {}.",
                                    item->itemClassName(), T::itemClassName()));

                items.append(&castItem->data());
            }

        return items;
    }

    /*
     * Parallel Comms
     */
    public:
    // Broadcast all data
    bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData);
    // Check equality of all data
    bool equality(ProcessPool &procPool);
};
