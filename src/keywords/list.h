/*
    *** Keyword List
    *** src/keywords/list.h
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

#include "base/enumoptions.h"
#include "keywords/data.h"
#include "keywords/group.h"
#include "templates/list.h"

// Forward Declarations
/* none */

// Keyword List
class KeywordList
{
    public:
    KeywordList();
    ~KeywordList();

    /*
     * Keyword List
     */
    private:
    // List of defined keywords
    List<KeywordBase> keywords_;

    public:
    // Add keyword
    bool add(KeywordBase *object, std::string_view name, std::string_view description, int optionMask = KeywordBase::NoOptions);
    // Add keyword (including argument description)
    bool add(KeywordBase *object, std::string_view name, std::string_view description, std::string_view arguments,
             int optionMask = KeywordBase::NoOptions);
    // Add keyword to named group
    bool add(std::string_view groupName, KeywordBase *object, std::string_view name, std::string_view description,
             int optionMask = KeywordBase::NoOptions);
    // Add keyword to named group (including argument description)
    bool add(std::string_view groupName, KeywordBase *object, std::string_view name, std::string_view description,
             std::string_view arguments, int optionMask = KeywordBase::NoOptions);
    // Add link to specified keyword that exists elsewhere
    bool link(std::string_view groupName, KeywordBase *object, std::string_view name, std::string_view description,
              int optionMask = KeywordBase::NoOptions);
    // Add link to specified keyword that exists elsewhere (including argument description)
    bool link(std::string_view groupName, KeywordBase *object, std::string_view name, std::string_view description,
              std::string_view arguments, int optionMask = KeywordBase::NoOptions);
    // Find named keyword
    KeywordBase *find(std::string_view name) const;
    // Return keywords list
    const List<KeywordBase> &keywords() const;

    /*
     * Groups
     */
    private:
    // Keywords organised by group
    List<KeywordGroup> groups_;

    private:
    // Create and/or return named keyword group
    KeywordGroup *addGroup(std::string_view name);

    public:
    // Return defined groups
    const List<KeywordGroup> &groups() const;

    /*
     * Set / Get
     */
    public:
    // Retrieve named item from specified list as template-guided type
    template <class T> T &retrieve(std::string_view name, T defaultValue = T(), bool *found = nullptr)
    {
        // Find item in the list
        KeywordBase *item = find(name);
        if (!item)
        {
            Messenger::printVerbose("No item named '{}' in the keyword list - default value item will be returned.\n", name);
            static T dummy;
            dummy = defaultValue;
            if (found != nullptr)
                (*found) = false;
            return dummy;
        }

        // Attempt to cast to specified type
        KeywordData<T> *castItem = dynamic_cast<KeywordData<T> *>(item);
        if (!castItem)
            throw std::runtime_error(
                fmt::format("KeywordList::retrieve({}) failed, because the target item is of the wrong type.", name));

        if (found != nullptr)
            (*found) = true;
        return castItem->data();
    }
    // Set named item from specified list as a template-guided type
    template <class T> bool set(std::string_view name, T value)
    {
        // Find item in the list
        KeywordBase *item = find(name);
        if (!item)
        {
            Messenger::warn("No item named '{}' in the keyword list - cannot set it's value.\n", name);
            return false;
        }

        // Attempt to cast to specified type
        KeywordData<T> *castItem = dynamic_cast<KeywordData<T> *>(item);
        if (!castItem)
            throw std::runtime_error(
                fmt::format("KeywordList::set({}) failed, because the target item is of the wrong type.", name));

        // Set the new value
        castItem->setData(value);

        return true;
    }
    // Set named EnumOptions from specified list as a template-guided type
    template <class E> bool setEnumeration(std::string_view name, E value)
    {
        // Find item in the list
        KeywordBase *item = find(name);
        if (!item)
        {
            Messenger::warn("No item named '{}' in the keyword list - cannot set it's value.\n", name);
            return false;
        }

        // Attempt to cast to specified type
        KeywordData<EnumOptions<E>> *castItem = dynamic_cast<KeywordData<EnumOptions<E>> *>(item);
        if (!castItem)
            throw std::runtime_error(
                fmt::format("KeywordList::setEnumeration({}) failed, because the target enum is of the wrong type.", name));

        // Set the new value
        castItem->data() = value;

        return true;
    }
    // Retrieve named EnumOptions with specified class, and return its current enumeration
    template <class E> E enumeration(std::string_view name, bool *found = nullptr)
    {
        // Find item in the list
        KeywordBase *item = find(name);
        if (!item)
        {
            Messenger::error("No item named '{}' in the keyword list - default enumeration of -1 will be returned.\n", name);
            if (found != nullptr)
                (*found) = false;
            return (E)-1;
        }

        // Attempt to cast to EnumOptionsBase
        KeywordData<EnumOptions<E>> *castItem = dynamic_cast<KeywordData<EnumOptions<E>> *>(item);
        if (!castItem)
        {
            Messenger::error("Failed to cast keyword '{}' into EnumOptions<E> because it's of a different type.\n", name);
            if (found != nullptr)
                (*found) = false;
            return (E)-1;
        }

        if (found != nullptr)
            (*found) = true;
        return castItem->data().enumeration();
    }

    /*
     * Conversion
     */
    public:
    // Return simple keyword value (as bool)
    bool asBool(std::string_view name) const;
    // Return simple keyword value (as int)
    int asInt(std::string_view name) const;
    // Return simple keyword value (as double)
    double asDouble(std::string_view name) const;
    // Return simple keyword value (as string)
    std::string asString(std::string_view name) const;
    // Return simple keyword value (as Vec3<int>)
    Vec3<int> asVec3Int(std::string_view name) const;
    // Return simple keyword value (as Vec3<double>)
    Vec3<double> asVec3Double(std::string_view name) const;
    // Return whether the specified keyword data has ever been set
    bool isSet(std::string_view name) const;
    // Flag that the specified keyword has been set by some external means
    void hasBeenSet(std::string_view name);

    /*
     * Read / Write
     */
    public:
    // Try to parse keyword in specified LineParser
    KeywordBase::ParseResult parse(LineParser &parser, CoreData &coreData);
    // Write all keywords to specified LineParser
    bool write(LineParser &parser, std::string_view prefix, bool onlyIfSet = true);
    // Write all keywords in groups to specified LineParser
    bool writeGroups(LineParser &parser, std::string_view prefix, bool onlyIfSet = true);
};
