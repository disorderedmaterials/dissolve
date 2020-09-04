/*
    *** Generic List
    *** src/genericitems/list.cpp
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

#include "genericitems/list.h"

// Clear all items (except those that are marked protected)
void GenericList::clear()
{
    GenericItem *item = items_.first(), *nextItem;
    while (item)
    {
        nextItem = item->next();
        if (!item->hasFlag(GenericItem::ProtectedFlag))
            items_.remove(item);
        item = nextItem;
    }
}

// Clear all items, including protected items
void GenericList::clearAll() { items_.clear(); }

// Add specified item to list (from base class pointer)
void GenericList::add(GenericItem *item) { items_.own(item); }

// Create an item of the specified type
GenericItem *GenericList::create(std::string_view name, std::string_view itemClassName, int version, int flags)
{
    // Check for existing item with this name
    GenericItem *newItem = find(name);
    if (newItem)
    {
        Messenger::printVerbose("Item named '{}' already exists in the list, so returning it instead of creating a new one.\n",
                                name);
        return newItem;
    }

    // Create a new item of the specified class type
    newItem = GenericItem::newItem(itemClassName, name);
    if (!newItem)
    {
        Messenger::error("GenericList::create() doesn't know how to create an item of type '{}'\n", itemClassName);
        return nullptr;
    }

    // Add the new item to our list
    add(newItem);

    // Set its version
    newItem->setVersion(version);

    // Set its flags
    newItem->setFlags(flags);

    return newItem;
}

// Return whether the named item is contained in the list
bool GenericList::contains(std::string_view name, std::string_view prefix)
{
    // Construct full name
    std::string varName = prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name);

    for (auto *item = items_.first(); item != nullptr; item = item->next())
        if (DissolveSys::sameString(item->name(), varName))
            return true;

    return false;
}

// Return if named item, if it exists, is of specified type
bool GenericList::isItemOfType(std::string_view type, std::string_view name, std::string_view prefix)
{
    GenericItem *item = find(name, prefix);
    if (!item)
        return false;

    return DissolveSys::sameString(type, item->itemClassName());
}

// Return item list
List<GenericItem> &GenericList::items() { return items_; }

// Return the named item from the list
GenericItem *GenericList::find(std::string_view name)
{
    for (auto *item = items_.first(); item != nullptr; item = item->next())
        if (DissolveSys::sameString(item->name(), name))
            return item;
    return nullptr;
}

// Return the named item from the list (with prefix)
GenericItem *GenericList::find(std::string_view name, std::string_view prefix)
{
    // Construct full name
    std::string varName = prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name);

    for (auto *item = items_.first(); item != nullptr; item = item->next())
        if (DissolveSys::sameString(item->name(), varName))
            return item;
    return nullptr;
}

// Return the version of the named item from the list
int GenericList::version(std::string_view name, std::string_view prefix) const
{
    // Construct full name
    std::string varName = prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name);

    for (auto *item = items_.first(); item != nullptr; item = item->next())
        if (DissolveSys::sameString(item->name(), varName))
            return item->version();

    return -99;
}

// Return list of all items with specified prefix (before first '_')
RefList<GenericItem> GenericList::itemsWithPrefix(std::string_view prefix)
{
    RefList<GenericItem> items;
    for (auto *item = items_.first(); item != nullptr; item = item->next())
    {
        if (DissolveSys::beforeChar(item->name(), '_') == prefix)
            items.append(item);
    }

    return items;
}

// Return list of all items with specified class name
RefList<GenericItem> GenericList::itemsWithClassName(std::string_view className)
{
    RefList<GenericItem> items;
    for (auto *item = items_.first(); item != nullptr; item = item->next())
        if (DissolveSys::sameString(item->itemClassName(), className))
            items.append(item);

    return items;
}

// List all items
void GenericList::listItems()
{
    auto count = 0;
    for (auto *item = items_.first(); item != nullptr; item = item->next(), ++count)
        Messenger::print("  {:3d}  {}", count, item->name());
}

// Remove named item
bool GenericList::remove(std::string_view name, std::string_view prefix)
{
    // First, find the named item
    GenericItem *item = find(name, prefix);
    if (!item)
    {
        Messenger::printVerbose("Couldn't find named item '{}' (prefix = '{}') in the list, so can't remove it.\n", name,
                                prefix);
        return false;
    }

    items_.remove(item);
    return true;
}

// Rename item
bool GenericList::rename(std::string_view oldName, std::string_view oldPrefix, std::string_view newName,
                         std::string_view newPrefix)
{
    // First, find the named item
    GenericItem *item = find(oldName, oldPrefix);
    if (!item)
    {
        Messenger::printVerbose("Couldn't find named item '{}' (prefix = '{}') in the list, so can't rename it.\n", oldName,
                                oldPrefix);
        return false;
    }

    if (newPrefix.empty())
        item->setName(newName);
    else
        item->setName(fmt::format("{}_{}", newPrefix, newName));

    return true;
}

// Prune all items with '@suffix'
void GenericList::pruneWithSuffix(std::string_view suffix)
{
    GenericItem *nextItem = nullptr;
    GenericItem *item = items_.first();
    while (item != nullptr)
    {
        if (DissolveSys::endsWith(item->name(), suffix))
        {
            nextItem = item->next();
            items_.remove(item);
            item = nextItem;
        }
        else
            item = item->next();
    }
}

/*
 * Parallel Comms
 */

// Broadcast all data
bool GenericList::broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
{
    for (auto *item = items_.first(); item != nullptr; item = item->next())
    {
        Messenger::printVerbose("Broadcasting data '{}' ({})...\n", item->name(), item->itemClassName());
        if (!item->broadcast(procPool, root, coreData))
            return false;
    }
    return true;
}

// Check equality of all data
bool GenericList::equality(ProcessPool &procPool)
{
    // Loop over processes - the target process will loop over its module data, sending out the name to the slaves.
    // If found, we note that we've already done this data, check it for equality, and move on.
    // If we can't find the data, we'll complain but move on.
    RefList<GenericItem> checkedItems;
    std::string itemName, itemClassName;
    auto nFailed = 0;
    for (int n = 0; n < procPool.nProcesses(); ++n)
    {
        // The master process - rank 'n' - will control the loop
        if (procPool.poolRank() == n)
        {
            // Loop over GenericItems in list
            for (auto *item = items_.first(); item != nullptr; item = item->next())
            {
                // If we have already checked this item, move on...
                if (checkedItems.contains(item))
                    continue;

                // Found an item that hasn't already been checked, so flag to the slaves that we will be sending
                // another name to check
                if (!procPool.decideTrue(n))
                    return false;

                Messenger::printVerbose("Checking equality of item '{}'...\n", item->name());

                // Send name of item - slaves will try to find it, so we need to check that everybody does...
                itemName = item->name();
                if (!procPool.broadcast(itemName, n))
                    return false;
                if (!procPool.allTrue(true))
                {
                    Messenger::error("GenericList equality check failed - item '{}' is not present on all "
                                     "processes.\n",
                                     item->name());
                    ++nFailed;
                    continue;
                }

                // Send class name of item - slaves will check it, and everybody must agree
                itemClassName = item->itemClassName();
                if (!procPool.broadcast(itemClassName, n))
                    return false;
                if (!procPool.allTrue(true))
                {
                    Messenger::error("GenericList equality check failed - item '{}' is not of the same "
                                     "type ({}) on all processes.\n",
                                     item->name(), item->itemClassName());
                    ++nFailed;
                    continue;
                }

                // Check the item for equality
                if (!item->equality(procPool))
                    return Messenger::error("Failed equality check for module data '{}'.\n", item->name());

                // Add the item to our checked list
                checkedItems.append(item);
            }

            // No more items, so send the 'false' signal
            if (!procPool.decideFalse(n))
                return false;
        }
        else
            while (procPool.decision(n))
            {
                // Receive the name of the item we're targetting
                if (!procPool.broadcast(itemName, n))
                    return false;

                Messenger::printVerbose("Checking equality of item '{}'...\n", itemName);

                // Do we have an item of this name?
                GenericItem *item = find(itemName);
                if (!procPool.allTrue(item))
                {
                    Messenger::error("GenericList equality check failed - item '{}' not found on process {}.\n", item->name(),
                                     procPool.poolRank());
                    ++nFailed;
                    continue;
                }

                // Receive the class name of the item and check it against ours
                if (!procPool.broadcast(itemClassName, n))
                    return false;
                if (!procPool.allTrue(DissolveSys::sameString(item->itemClassName(), itemClassName)))
                {
                    Messenger::error("GenericList equality check failed - item '{}' not of the correct "
                                     "type (is {}) on process {}.\n",
                                     item->name(), item->itemClassName(), procPool.poolRank());
                    ++nFailed;
                    continue;
                }

                // Check the item for equality
                if (!item->equality(procPool))
                    return Messenger::error("Failed equality check for module data '{}'.\n", item->name());

                // Add the item to our checked list
                checkedItems.append(item);
            }
    }

    // Did we succeed?
    return (nFailed == 0);
}
