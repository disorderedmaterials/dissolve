// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/list.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "keywords/types.h"
#include "procedure/nodes/nodes.h"

/*
 * Keyword Setter
 */

KeywordTypeMap::KeywordTypeMap()
{
    // PODs
    registerDirectMapping<bool, BoolKeyword>();
    // -- Double and int keywords must use the setData() function as they have validation
    registerDirectMapping<double, DoubleKeyword>(
        [](DoubleKeyword *keyword, const double value) { return keyword->setData(value); });
    registerDirectMapping<int, IntegerKeyword>(
        [](IntegerKeyword *keyword, const int value) { return keyword->setData(value); });

    // Custom classes
    registerDirectMapping<Collect1DProcedureNode *, NodeKeyword<Collect1DProcedureNode>>();
    registerDirectMapping<std::vector<const Collect1DProcedureNode *>, NodeVectorKeyword<Collect1DProcedureNode>>();
    registerDirectMapping<SelectProcedureNode *, NodeKeyword<SelectProcedureNode>>();
    registerDirectMapping<std::vector<const SelectProcedureNode *>, NodeVectorKeyword<SelectProcedureNode>>();
    registerDirectMapping<std::vector<Module *>, ModuleVectorKeyword>();
    registerBaseMapping<const Module *, ModuleKeywordBase>();
    registerDirectMapping<std::string, StringKeyword>();
    registerDirectMapping<Vec3<double>, Vec3DoubleKeyword>(
        [](Vec3DoubleKeyword *keyword, const Vec3<double> value) { return keyword->setData(value); });
}

// Set keyword data
bool KeywordTypeMap::set(KeywordBase *keyword, const std::any data) const
{
    // Find a suitable setter and call it
    auto it = directMapSetter_.find(data.type());
    if (it == directMapSetter_.end())
        throw(std::runtime_error(fmt::format(
            "Item of type '{}' cannot be set as no suitable type mapping has been registered.\n", data.type().name())));

    return (it->second)(keyword, data);
}

/*
 * Keyword List
 */

// Find named keyword
KeywordBase *KeywordList::find(std::string_view name)
{
    auto it = keywords_.find(name);
    if (it == keywords_.end())
        return nullptr;
    return it->second;
}
const KeywordBase *KeywordList::find(std::string_view name) const
{
    auto it = keywords_.find(name);
    if (it == keywords_.end())
        return nullptr;
    return it->second;
}

// Return keywords
const std::map<std::string_view, KeywordBase *> KeywordList::keywords() const { return keywords_; }

// Return keyword group mappings
const std::map<std::string_view, std::vector<std::string_view>> KeywordList::displayGroups() const { return displayGroups_; };

// Return whether the keyword has been set, and is not currently empty (if relevant)
bool KeywordList::hasBeenSet(std::string_view name) const
{
    // Find the named keyword
    auto *keyword = find(name);
    if (!keyword)
        throw(std::runtime_error(fmt::format("No Module keyword named '{}' exists to check whether it is set.\n", name)));

    return keyword->hasBeenSet();
}

// Flag that the specified keyword has been set by some external means
void KeywordList::setAsModified(std::string_view name) const
{
    // Find the named keyword
    auto it = keywords_.find(name);
    if (it == keywords_.end())
        throw(std::runtime_error(fmt::format("No Module keyword named '{}' exists to set its modification status.\n", name)));

    it->second->setAsModified();
}

/*
 * Set
 */

// Return the setter instance
const KeywordTypeMap &KeywordList::setters()
{
    static const KeywordTypeMap setters;

    return setters;
}

// Set specified keyword with supplied data
void KeywordList::set(std::string_view name, const std::any value)
{
    auto it = keywords_.find(name);
    if (it == keywords_.end())
        throw(std::runtime_error(fmt::format("No keyword named '{}' exists to set.\n", name)));

    // Attempt to set the keyword
    setters().set(it->second, value);

    it->second->setAsModified();
}

/*
 * Read / Write
 */

// Try to parse a single keyword through the specified LineParser
KeywordBase::ParseResult KeywordList::parse(LineParser &parser, const CoreData &coreData, int startArg)
{
    // Do we recognise the first item (the 'keyword')?
    auto it = keywords_.find(parser.argsv(startArg));
    if (it == keywords_.end())
        return KeywordBase::Unrecognised;
    auto *keyword = it->second;

    // We recognised the keyword - check the number of arguments we have against the min / max for the keyword
    if (!keyword->validNArgs(parser.nArgs() - startArg - 1))
        return KeywordBase::Failed;

    // All OK, so parse the keyword
    if (!keyword->read(parser, startArg + 1, coreData))
    {
        Messenger::error("Failed to parse arguments for keyword '{}'.\n", keyword->name());
        return KeywordBase::Failed;
    }

    return KeywordBase::Success;
}

// Write all keywords to specified LineParser
bool KeywordList::write(LineParser &parser, std::string_view prefix, bool onlyIfSet) const
{
    for (const auto &[name, keyword] : keywords_)
    {
        // If the keyword has never been set (i.e. it still has its default value) don't bother to write it
        if (onlyIfSet && (!keyword->hasBeenSet()))
            continue;

        if (!keyword->write(parser, name, prefix))
            return false;
    }

    return true;
}
