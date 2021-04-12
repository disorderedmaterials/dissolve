// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/list.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "keywords/linkto.h"

KeywordList::KeywordList() {}

KeywordList::~KeywordList() {}

/*
 * Keyword List
 */

// Add keyword
bool KeywordList::add(KeywordBase *object, std::string_view name, std::string_view description, int optionMask)
{
    return add(object, name, description, "", optionMask);
}

// Add keyword (including argument description)
bool KeywordList::add(KeywordBase *object, std::string_view name, std::string_view description, std::string_view arguments,
                      int optionMask)
{
    // Take ownership of the passed object, and set its basic information
    keywords_.own(object);
    object->set(name, description, arguments, optionMask);

    return true;
}

// Add keyword to named group
bool KeywordList::add(std::string_view groupName, KeywordBase *object, std::string_view name, std::string_view description,
                      int optionMask)
{
    KeywordGroup *group = addGroup(groupName);
    return group->add(object, name, description, optionMask);
}

// Add keyword to named group (including argument description)
bool KeywordList::add(std::string_view groupName, KeywordBase *object, std::string_view name, std::string_view description,
                      std::string_view arguments, int optionMask)
{
    KeywordGroup *group = addGroup(groupName);
    return group->add(object, name, description, arguments, optionMask);
}

// Add link to specified keyword that exists elsewhere
bool KeywordList::link(std::string_view groupName, KeywordBase *object, std::string_view name, std::string_view description,
                       int optionMask)
{
    if (!object)
        return Messenger::error("NULL KeywordBase* passed to KeywordList::link().\n");

    // Create a new LinkToKeyword
    return add(groupName, new LinkToKeyword(object), name, description, optionMask);
}

// Add link to specified keyword that exists elsewhere (including argument description)
bool KeywordList::link(std::string_view groupName, KeywordBase *object, std::string_view name, std::string_view description,
                       std::string_view arguments, int optionMask)
{
    if (!object)
        return Messenger::error("NULL KeywordBase* passed to KeywordList::link().\n");

    // Create a new LinkToKeyword
    return add(groupName, new LinkToKeyword(object), name, description, arguments, optionMask);
}

// Find named keyword
KeywordBase *KeywordList::find(std::string_view name) const
{
    for (auto *kwd = keywords_.first(); kwd != nullptr; kwd = kwd->next())
        if (DissolveSys::sameString(name, kwd->name()))
            return kwd->base();

    return nullptr;
}

// Cut keyword from list
void KeywordList::cut(KeywordBase *kwd) { keywords_.cut(kwd); }

// Return first keyword in list
const List<KeywordBase> &KeywordList::keywords() const { return keywords_; }

/*
 * Groups
 */

// Create and/or return named keyword group
KeywordGroup *KeywordList::addGroup(std::string_view name)
{
    // Check that a group with the specified name doesn't already exist
    KeywordGroup *group = nullptr;
    for (group = groups_.first(); group != nullptr; group = group->next())
        if (DissolveSys::sameString(name, group->name()))
            break;

    if (!group)
    {
        group = new KeywordGroup(*this);
        group->setName(name);
        groups_.own(group);
    }

    return group;
}

// Return defined groups
const List<KeywordGroup> &KeywordList::groups() const { return groups_; }

/*
 * Conversion
 */

// Return simple keyword value (as bool)
bool KeywordList::asBool(std::string_view name) const
{
    // Find the named keyword
    KeywordBase *keyword = find(name);
    if (!keyword)
    {
        Messenger::warn("No Module keyword named '{}' exists to return as a bool. Returning 'false'...\n", name);
        return false;
    }

    return keyword->asBool();
}

// Return simple keyword value (as int)
int KeywordList::asInt(std::string_view name) const
{
    // Find the named keyword
    KeywordBase *keyword = find(name);
    if (!keyword)
    {
        Messenger::warn("No Module keyword named '{}' exists to return as an int. Returning '0'...\n", name);
        return 0;
    }

    return keyword->asInt();
}

// Return simple keyword value (as double)
double KeywordList::asDouble(std::string_view name) const
{
    // Find the named keyword
    KeywordBase *keyword = find(name);
    if (!keyword)
    {
        Messenger::warn("No Module keyword named '{}' exists to return as a double. Returning '0.0'...\n", name);
        return 0.0;
    }

    return keyword->asDouble();
}

// Return simple keyword value (as string)
std::string KeywordList::asString(std::string_view name) const
{
    // Find the named keyword
    KeywordBase *keyword = find(name);
    if (!keyword)
    {
        Messenger::warn("No Module keyword named '{}' exists to return as a string. Returning 'NULL'...\n", name);
        return "NULL";
    }

    return keyword->asString();
}

// Return simple keyword value (as Vec3<int>)
Vec3<int> KeywordList::asVec3Int(std::string_view name) const
{
    // Find the named keyword
    KeywordBase *keyword = find(name);
    if (!keyword)
    {
        Messenger::warn("No Module keyword named '{}' exists to return as a Vec3<int>. Returning '(0,0,0)'...\n", name);
        return Vec3<int>(0, 0, 0);
    }

    return keyword->asVec3Int();
}

// Return simple keyword value (as Vec3<double>)
Vec3<double> KeywordList::asVec3Double(std::string_view name) const
{
    // Find the named keyword
    KeywordBase *keyword = find(name);
    if (!keyword)
    {
        Messenger::warn("No Module keyword named '{}' exists to return as a Vec3<double>. Returning '(0.0,0.0,0.0)'...\n",
                        name);
        return Vec3<double>(0.0, 0.0, 0.0);
    }

    return keyword->asVec3Double();
}

// Return whether the specified keyword data has ever been set
bool KeywordList::isSet(std::string_view name) const
{
    // Find the named keyword
    KeywordBase *keyword = find(name);
    if (!keyword)
    {
        Messenger::warn("No Module keyword named '{}' exists to check whether it is set. Returning 'false'...\n", name);
        return false;
    }

    return keyword->isSet();
}

// Flag that the specified keyword has been set by some external means
void KeywordList::hasBeenSet(std::string_view name)
{
    // Find the named keyword
    KeywordBase *keyword = find(name);
    if (!keyword)
    {
        Messenger::warn("No Module keyword named '{}' exists to check whether it is set. Returning 'false'...\n", name);
        return;
    }

    keyword->hasBeenSet();
}

/*
 * Read / Write
 */

// Try to parse node keyword in specified LineParser
KeywordBase::ParseResult KeywordList::parse(LineParser &parser, const CoreData &coreData)
{
    // Do we recognise the first item (the 'keyword')?
    KeywordBase *keyword = find(parser.argsv(0));
    if (!keyword)
        return KeywordBase::Unrecognised;

    // We recognised the keyword - check the number of arguments we have against the min / max for the keyword
    if (!keyword->validNArgs(parser.nArgs() - 1))
        return KeywordBase::Failed;

    // All OK, so parse the keyword
    if (!keyword->read(parser, 1, coreData))
    {
        Messenger::error("Failed to parse arguments for keyword '{}'.\n", keyword->name());
        return KeywordBase::Failed;
    }

    return KeywordBase::Success;
}

// Write all keywords to specified LineParser
bool KeywordList::write(LineParser &parser, std::string_view prefix, bool onlyIfSet) const
{
    ListIterator<KeywordBase> keywordIterator(keywords_);
    while (KeywordBase *keyword = keywordIterator.iterate())
    {
        // If the keyword has never been set (i.e. it still has its default value) don't bother to write it
        if (onlyIfSet && (!keyword->base()->isSet()))
            continue;

        // Make sure we are calling the write() function of the base() keyword class, but with the parent object's
        // name()...
        if (!keyword->base()->write(parser, keyword->name(), prefix))
            return false;
    }

    return true;
}

// Write all keywords in groups to specified LineParser
bool KeywordList::writeGroups(LineParser &parser, std::string_view prefix, bool onlyIfSet) const
{
    // Loop over keyword groups
    auto firstGroup = true;
    ListIterator<KeywordGroup> groupsIterator(groups_);
    while (KeywordGroup *group = groupsIterator.iterate())
    {
        // Loop over keywords in group
        auto firstWritten = true;
        for (KeywordBase *keyword : group->keywords())
        {
            // If the keyword has never been set (i.e. it still has its default value) don't bother to write it
            if (onlyIfSet && (!keyword->base()->isSet()))
                continue;

            // If this is the first keyword to be written in the group, write the group name first as a comment
            if (firstWritten)
            {
                // If this is *not* the first group to be written, write a newline for formatting
                if ((!firstGroup) && (!parser.writeLineF("\n")))
                    return false;

                if (!parser.writeLineF("{}# {}\n", prefix, group->name()))
                    return false;
            }

            // Make sure we are calling the write() function of the base() keyword class...
            if (!keyword->base()->write(parser, keyword->name(), prefix))
                return false;

            // Falsify flags
            firstWritten = false;
            firstGroup = false;
        }
    }

    return true;
}
