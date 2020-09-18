/*
    *** Generic List Helper
    *** src/genericitems/listhelper.h
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

#pragma once

#include "base/sysfunc.h"
#include "genericitems/items.h"
#include "genericitems/list.h"

// Generic List Helper
template <class T> class GenericListHelper
{
    public:
    // Add new named item of template-guided type to specified list
    static T &add(GenericList &targetList, std::string_view name, std::string_view prefix = "", int flags = -1)
    {
        // Construct full name
        std::string varName = prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name);

        // Does the named variable already exist in the list?
        GenericItem *existingItem = targetList.find(varName);
        if (existingItem)
        {
            Messenger::warn("Item '{}' already exists in the list - a dummy value will be returned instead.\n", varName);
            static T dummy;
            return dummy;
        }

        // Create new item
        GenericItemContainer<T> *newItem = new GenericItemContainer<T>(varName);
        if (flags >= 0)
            newItem->setFlags(flags);
        targetList.add(newItem);
        return newItem->data();
    }
    // Return named (const) item from specified list as template-guided type
    static const T &value(GenericList &sourceList, std::string_view name, std::string_view prefix = "", T defaultValue = T(),
                          bool *found = nullptr)
    {
        // Construct full name
        std::string varName = prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name);

        // Find item in the list
        GenericItem *item = sourceList.find(varName);
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
        GenericItemContainer<T> *castItem = dynamic_cast<GenericItemContainer<T> *>(item);
        if (!castItem)
            throw std::runtime_error(
                fmt::format("GenericListHelper::value({}) failed, because the target item is of the wrong type.", name));

        if (found != nullptr)
            (*found) = true;
        return castItem->data();
    }
    // Retrieve named item from specified list as template-guided type, assuming that it is going to be modified
    static T &retrieve(GenericList &sourceList, std::string_view name, std::string_view prefix = "", T defaultValue = T(),
                       bool *found = nullptr)
    {
        // Construct full name
        std::string varName = prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name);

        // Find item in the list
        GenericItem *item = sourceList.find(varName);
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
        GenericItemContainer<T> *castItem = dynamic_cast<GenericItemContainer<T> *>(item);
        if (!castItem)
            throw std::runtime_error(
                fmt::format("GenericListHelper::retrieve({}) failed, because the item is not of the specified type.", name));

        // Bump the version of the item
        item->bumpVersion();

        if (found != nullptr)
            (*found) = true;
        return castItem->data();
    }
    // Create or retrieve named item from specified list as template-guided type
    static T &realise(GenericList &sourceList, std::string_view name, std::string_view prefix = "", int flags = -1,
                      bool *created = nullptr)
    {
        // Construct full name
        std::string varName = prefix.empty() ? std::string(name) : fmt::format("{}_{}", prefix, name);

        // Find item in the list - if it isn't there, create it and return
        GenericItem *item = sourceList.find(varName);
        if (!item)
        {
            if (created != nullptr)
                (*created) = true;
            return add(sourceList, name, prefix, flags);
        }

        // Cast to correct type
        GenericItemContainer<T> *castItem = dynamic_cast<GenericItemContainer<T> *>(item);
        if (!castItem)
            throw std::runtime_error(fmt::format(
                "GenericListHelper::realise({}) failed, because the item couldn't be cast to the desired type.", name));

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
    static RefList<T> items(GenericList &sourceList)
    {
        RefList<T> items;
        ListIterator<GenericItem> itemIterator(sourceList.items());
        while (GenericItem *item = itemIterator.iterate())
            if (DissolveSys::sameString(item->itemClassName(), T::itemClassName()))
            {
                // Cast to correct type
                GenericItemContainer<T> *castItem = dynamic_cast<GenericItemContainer<T> *>(item);
                if (!castItem)
                    throw std::runtime_error(
                        fmt::format("GenericListHelper::items() failed to retrieve item {} as it is not actually class {}.",
                                    item->itemClassName(), T::itemClassName()));
                {
                    Messenger::error("That didn't work, because its of the wrong type.\n");
                    continue;
                }

                items.append(&castItem->data());
            }

        return items;
    }
};
