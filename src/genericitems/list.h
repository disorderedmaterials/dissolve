// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "base/sysfunc.h"
#include "genericitems/item.h"
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
    bool contains(std::string_view name, std::string_view prefix = "");
    // Return if named item, if it exists, is of specified type
    bool isItemOfType(std::string_view type, std::string_view name, std::string_view prefix = "");
    // Return item list
    List<GenericItem> &items();
    // Return the named item from the list
    GenericItem *find(std::string_view name);
    // Return the named item from the list (with prefix)
    GenericItem *find(std::string_view name, std::string_view prefix);
    // Return the version of the named item from the list
    int version(std::string_view name, std::string_view prefix = "") const;
    // Return list of all items with specified prefix (before first '_')
    RefList<GenericItem> itemsWithPrefix(std::string_view prefix);
    // Return list of all items with specified class type
    RefList<GenericItem> itemsWithClassName(std::string_view className);
    // List all items
    void listItems();
    // Remove named item
    bool remove(std::string_view name, std::string_view prefix);
    // Rename item
    bool rename(std::string_view oldName, std::string_view oldPrefix, std::string_view newName, std::string_view newPrefix);
    // Prune all items with '@suffix'
    void pruneWithSuffix(std::string_view suffix);

    /*
     * Parallel Comms
     */
    public:
    // Broadcast all data
    bool broadcast(ProcessPool &procPool, const int root, const CoreData &coreData);
    // Check equality of all data
    bool equality(ProcessPool &procPool);
};
