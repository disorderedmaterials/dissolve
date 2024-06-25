// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "items/list.h"
#include "items/deserialisers.h"
#include "items/serialisers.h"
#include "main/version.h"
#include <cassert>
#include <fmt/format.h>
#include <typeindex>

// Static Singletons
GenericList::DeserialisableDataVersion GenericList::baseDataVersion_(GenericList::DeserialisableDataVersion::Current);

// Clear all items (except those that are marked protected)
void GenericList::clear()
{
    for (auto it = items_.begin(); it != items_.end();)
        if (!(std::get<GenericItem::Flags>(it->second) & GenericItem::ProtectedFlag))
            items_.erase(it++);
        else
            ++it;
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

// Remove all items with specified prefix
void GenericList::removeWithPrefix(std::string_view prefix)
{
    auto delimitedPrefix = fmt::format("{}//", prefix);
    for (auto it = items_.begin(); it != items_.end();)
        if (DissolveSys::startsWith(it->first, delimitedPrefix))
            items_.erase(it++);
        else
            ++it;
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

// Rename prefix of items
void GenericList::renamePrefix(std::string_view oldPrefix, std::string_view newPrefix)
{
    auto delimitedPrefix = fmt::format("{}//", oldPrefix);

    /*
     * Because we are using a std::map there is no easy way to achieve a bulk renaming of keys. The only sane approach,
     * avoiding copying data, is to extract the items into a separate map, rename them, then merge them back in to the
     * original map.  This is complicated by the fact that as soon as we extract an item from the map we potentially
     * invalidate iterators and cause segfaults or, at the very least, we don't actually rename all the target data.
     * So, here we sequentially extract matching items, with the loop restarting after every match and running until we
     * get no match. This obviously isn't terribly efficient but the function is sparingly used and so the general performance
     * impact will be minimal.
     */
    std::map<std::string, GenericItem::Type> renamedItems;
    auto found = true;
    while (found)
    {
        found = false;
        for (auto &[key, value] : items_)
        {
            if (DissolveSys::startsWith(key, delimitedPrefix))
            {
                auto handle = items_.extract(key);
                handle.key() = fmt::format("{}//{}", newPrefix, DissolveSys::afterString(key, "//"));
                renamedItems.insert(std::move(handle));

                found = true;
            }
            if (found)
                break;
        }
    }

    // Move the renamed data back into our items_ map
    while (!renamedItems.empty())
    {
        auto &&[key, value] = *renamedItems.begin();
        auto handle = renamedItems.extract(key);
        items_.insert(std::move(handle));
    }
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

// Return EnumOptions for DataVersion
EnumOptions<GenericList::DeserialisableDataVersion> GenericList::deserialisableDataVersions()
{
    return EnumOptions<GenericList::DeserialisableDataVersion>(
        "AveragingScheme", {{GenericList::DeserialisableDataVersion::Version08X, "v0.8."},
                            {GenericList::DeserialisableDataVersion::Current, fmt::format("v{}", Version::semantic())}});
}

// Set current data version being deserialised by detecting it from the supplied string
void GenericList::setBaseDataVersionFromString(std::string_view s)
{
    // Search the supplied string for any of our defined DeserialisableDataVersion enumeration keywords
    for (auto n = 0; n < deserialisableDataVersions().nOptions(); ++n)
    {
        if (s.find(deserialisableDataVersions().keywordByIndex(n)) != std::string::npos)
        {
            Messenger::print("Detected data version as {}.\n", deserialisableDataVersions().keywordByIndex(n));
            baseDataVersion_ = deserialisableDataVersions().enumerationByIndex(n);
            return;
        }
    }

    Messenger::print("Unable to auto-detect data version - assuming current ({}).\n", Version::semantic());
    baseDataVersion_ = DeserialisableDataVersion::Current;
}

// Return current data version being deserialised
GenericList::DeserialisableDataVersion GenericList::baseDataVersion() { return baseDataVersion_; }

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
bool GenericList::deserialise(LineParser &parser, CoreData &coreData, const std::string &name, const std::string &itemClass,
                              int dataVersion, int flags)
{
    // Create the item
    items_[name] = GenericItem::Type(GenericItemProducer::create(itemClass), itemClass, dataVersion, flags);
    auto &data = std::get<GenericItem::AnyObject>(items_[name]);

    // Find its deserialiser and call it
    if (!GenericItemDeserialiser::deserialise(data, parser, coreData))
        return Messenger::error(fmt::format("Deserialisation of item '{}' failed.\n", name));

    // Check for legacy objects - we don't store them in the items_ map
    if (GenericItemDeserialiser::isLegacyObject(data))
    {
        Messenger::warn("Legacy data '{}' ({}) will not be captured in written restart files.\n", name, itemClass);
        items_.erase(name);
    }

    return true;
}
