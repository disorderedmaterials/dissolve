// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "genericitems/list.h"
#include <cassert>
#include <classes/partialset.h>
#include <fmt/core.h>
#include <typeindex>

GenericList::GenericList()
{
    registerBroadcasters();
    registerDeserialisers();
    registerProducers();
    registerSerialisers();
}

// Clear all items (except those that are marked protected)
void GenericList::clear()
{
    for (auto &[key, value] : items_)
        if (!(std::get<ItemData::Flags>(value) & GenericList::ProtectedFlag))
            items_.erase(key);
}

// Clear all items, including protected items
void GenericList::clearAll() { items_.clear(); }

// Return whether the named item is contained in the list
bool GenericList::contains(std::string_view name, std::string_view prefix) const
{
    auto it = items_.find(prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name));
    return (it != items_.end());
}

// Return item list
const std::map<std::string, GenericList::GenericItem> &GenericList::items() const { return items_; }

// Return the named item from the map, if it exists
GenericList::GenericItem &GenericList::find(std::string_view name, std::string_view prefix)
{
    auto it = items_.find(prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name));
    assert(it != items_.end());
    return it->second;
}

const GenericList::GenericItem &GenericList::find(std::string_view name, std::string_view prefix) const
{
    auto it = items_.find(prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name));
    assert(it != items_.end());
    return it->second;
}

// Return the version of the named item from the list
int GenericList::version(std::string_view name, std::string_view prefix) const
{
    auto it = items_.find(prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name));
    assert(it != items_.end());
    return std::get<ItemData::Version>(it->second);
}

// Remove named item
void GenericList::remove(std::string_view name, std::string_view prefix)
{
    auto it = items_.find(prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name));
    if (it == items_.end())
        throw(std::runtime_error(fmt::format("GenericList::remove() - No item named '{}' exists.\n",
                                             prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name))));

    items_.erase(it);
}

// Rename item
void GenericList::rename(std::string_view oldName, std::string_view oldPrefix, std::string_view newName,
                         std::string_view newPrefix)
{
    std::string oldVarName = oldPrefix.empty() ? std::string(oldName) : fmt::format("{}_{}", oldPrefix, oldName);
    std::string newVarName = newPrefix.empty() ? std::string(newName) : fmt::format("{}_{}", newPrefix, newName);

    auto it = items_.find(oldVarName);
    if (it == items_.end())
        throw(std::runtime_error(fmt::format("GenericList::rename() - No item named '{}' exists.\n", oldVarName)));

    auto handle = items_.extract(oldVarName);
    handle.key() = newVarName;
    items_.insert(std::move(handle));
}

// Prune all items with '@suffix'
void GenericList::pruneWithSuffix(std::string_view suffix)
{
    for (auto &[key, value] : items_)
        if (DissolveSys::endsWith(key, suffix))
            items_.erase(key);
}

/*
 * Item Creation
 */

// Create new item of the named class type
GenericList::GenericItem &GenericList::create(std::string_view name, std::string_view className, int version, int flags)
{
    auto it =
        std::find_if(classNames_.begin(), classNames_.end(), [&className](auto &item) { return item.second == className; });
    if (it == classNames_.end())
        throw(std::runtime_error(
            fmt::format("Item '{}' cannot be created as no producer has been registered for class '{}'.\n", name, className)));

    items_.emplace(name, GenericItem(producers_[it->first](), className, version, flags));

    return items_[std::string(name)];
}

/*
 * Serialisation
 */

// Serialise all objects via the specified LineParser
bool GenericList::serialiseAll(LineParser &parser, const CoreData &coreData, std::string_view headerPrefix) const
{
    for (auto &[key, value] : items_)
    {
        // If it is not flagged to be saved in the restart file, skip it
        if (!(std::get<ItemData::Flags>(value) & GenericList::InRestartFileFlag))
            continue;

        if (!parser.writeLineF("{}  {}  {}  {}  {}\n", headerPrefix, key, std::get<ItemData::ClassName>(value),
                               std::get<ItemData::Version>(value), std::get<ItemData::Flags>(value)))
            return false;

        // Find a suitable serialiser and call it
        auto &data = std::get<ItemData::AnyObject>(value);
        auto it = serialisers_.find(data.type());
        if (it == serialisers_.end())
            throw(std::runtime_error(
                fmt::format("Item '{}' cannot be serialised as no suitable serialiser has been registered for class '{}'.\n",
                            key, std::get<ItemData::ClassName>(value))));
        if (!(it->second)(data, parser, coreData))
            return Messenger::error(fmt::format("Serialisation of item '{}' failed.\n", key));
    }

    return true;
}

// Deserialise an object from the LineParser into our map
bool GenericList::deserialise(LineParser &parser, CoreData &coreData, std::string_view name, std::string_view itemClass,
                              int version, int flags)
{
    // Create the item
    auto &item = create(name, itemClass, version, flags);
    auto &data = std::get<ItemData::AnyObject>(item);

    // Find its deserialiser and call it
    auto it = deserialisers_.find(data.type());
    if (it == deserialisers_.end())
        throw(std::runtime_error(
            fmt::format("Item '{}' cannot be deserialised as no suitable deserialiser has been registered for class '{}'.\n",
                        name, itemClass)));
    if (!(it->second)(data, parser, coreData))
        return Messenger::error(fmt::format("Deserialisation of item '{}' failed.\n", name));

    return true;
}

/*
 * Parallel Comms
 */

// Broadcast all data
bool GenericList::broadcast(ProcessPool &procPool, const int root, const CoreData &coreData)
{
    for (auto &[key, value] : items_)
    {
        Messenger::printVerbose("Broadcasting item '{}'...\n", key);
        auto &data = std::get<ItemData::AnyObject>(value);
        auto it = broadcasters_.find(data.type());
        if (it == broadcasters_.end())
            throw(std::runtime_error(
                fmt::format("Item '{}' cannot be broadcast as no suitable broadcaster has been registered for class '{}'.\n",
                            key, data.type().name())));
        if (!(it->second)(data, procPool, root, coreData))
            return Messenger::error(fmt::format("Broadcast of item '{}' failed.\n", key));
    }
    return true;
}
