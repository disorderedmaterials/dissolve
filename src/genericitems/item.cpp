// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "genericitems/item.h"

// Static Members
List<GenericItem> GenericItem::itemClasses_;

GenericItem::GenericItem(std::string_view name, int flags) : ListItem<GenericItem>()
{
    name_ = name;
    flags_ = flags;
    version_ = 1;
}

GenericItem::~GenericItem(){};

/*
 * Item Class
 */

// Add class to list of representative itemClasses_
void GenericItem::addItemClass(GenericItem *item) { itemClasses_.own(item); }

// Return new, empty GenericItem containing the class specified
GenericItem *GenericItem::newItem(std::string_view className, std::string_view name, int flags)
{
    // Search through registered item classes list for one matching the class name provided
    for (auto *item = itemClasses_.first(); item != nullptr; item = item->next())
    {
        // See if the item can create a GenericItem of the desired type. If it can't, move on
        GenericItem *newItem = item->createItem(className, name, flags);
        if (newItem != nullptr)
            return newItem;
    }

    return nullptr;
}

/*
 * Item Contents
 */

// Set item name
void GenericItem::setName(std::string_view name) { name_ = name; }

// Return item name
std::string_view GenericItem::name() const { return name_; }

// Return version of the item
int GenericItem::version() const { return version_; }

// Set version of the item
void GenericItem::setVersion(int version) { version_ = version; }

// Bump the version of the item
void GenericItem::bumpVersion() { ++version_; }

// Set flags for item, overwriting any existing flags
void GenericItem::setFlags(int flags) { flags_ = flags; }

// Add (set) flag for item
void GenericItem::addFlag(ItemFlag flag)
{
    if (!hasFlag(flag))
        flags_ += flag;
}

// Remove (uneset) flag for item
void GenericItem::removeFlag(ItemFlag flag)
{
    if (hasFlag(flag))
        flags_ -= flag;
}

// Return flags
int GenericItem::flags() const { return flags_; }

// Return whether specified flag is set
bool GenericItem::hasFlag(GenericItem::ItemFlag flag) const { return (flags_ & flag); }
