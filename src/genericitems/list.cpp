// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "genericitems/list.h"
#include "genericitems/deserialisers.h"
#include "genericitems/serialisers.h"
#include <cassert>
#include <fmt/core.h>
#include <typeindex>

// Clear all items (except those that are marked protected)
void GenericList::clear()
{
    for (auto &[key, value] : items_)
        if (!(std::get<GenericItem::Flags>(value) & GenericItem::ProtectedFlag))
            items_.erase(key);
}

// Clear all items, including protected items
void GenericList::clearAll() { items_.clear(); }

// Return whether the named item is contained in the list
bool GenericList::contains(std::string_view name, std::string_view prefix) const
{
    auto it = items_.find(prefix.empty() ? std::string(name) : fmt::format("{}//{}", prefix, name));
    return (it != items_.end());
}

// Return item list
const std::map<std::string, GenericItem::Type> &GenericList::items() const { return items_; }

// Return the version of the named item from the list
int GenericList::version(std::string_view name, std::string_view prefix) const
{
    auto it = items_.find(prefix.empty() ? std::string(name) : fmt::format("{}//{}", prefix, name));
    assert(it != items_.end());
    return std::get<GenericItem::Version>(it->second);
}

// Remove named item
void GenericList::remove(std::string_view name, std::string_view prefix)
{
    auto it = items_.find(prefix.empty() ? std::string(name) : fmt::format("{}//{}", prefix, name));
    if (it == items_.end())
        throw(std::runtime_error(fmt::format("GenericList::remove() - No item named '{}' exists.\n",
                                             prefix.empty() ? std::string(name) : fmt::format("{}//{}", prefix, name))));

    items_.erase(it);
}

// Rename item
void GenericList::rename(std::string_view oldName, std::string_view oldPrefix, std::string_view newName,
                         std::string_view newPrefix)
{
    std::string oldVarName = oldPrefix.empty() ? std::string(oldName) : fmt::format("{}//{}", oldPrefix, oldName);
    std::string newVarName = newPrefix.empty() ? std::string(newName) : fmt::format("{}//{}", newPrefix, newName);

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
 * Serialisation
 */

// Serialise all objects via the specified LineParser
bool GenericList::serialiseAll(LineParser &parser, std::string_view headerPrefix) const
{
    for (auto &[key, value] : items_)
    {
        // If it is not flagged to be saved in the restart file, skip it
        if (!(std::get<GenericItem::Flags>(value) & GenericItem::InRestartFileFlag))
            continue;

        if (!parser.writeLineF("{}  {}  {}  {}  {}\n", headerPrefix, key, std::get<GenericItem::ClassName>(value),
                               std::get<GenericItem::Version>(value), std::get<GenericItem::Flags>(value)))
            return false;

        // Find a suitable serialiser and call it
        auto &data = std::get<GenericItem::AnyObject>(value);
        if (!GenericItemSerialiser::serialise(data, parser))
            return Messenger::error(fmt::format("Serialisation of item '{}' failed.\n", key));
    }

    return true;
}

// Deserialise an object from the LineParser into our map
bool GenericList::deserialise(LineParser &parser, CoreData &coreData, std::string name, std::string itemClass, int version,
                              int flags)
{
    // Create the item
    items_[std::string(name)] = GenericItem::Type(GenericItemProducer::create(itemClass), itemClass, version, flags);
    auto &data = std::get<GenericItem::AnyObject>(items_[std::string(name)]);

    // Find its deserialiser and call it
    if (!GenericItemDeserialiser::deserialise(data, parser, coreData))
        return Messenger::error(fmt::format("Deerialisation of item '{}' failed.\n", name));

    return true;
}
