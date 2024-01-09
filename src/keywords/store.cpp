// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/store.h"
#include "base/lineParser.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/configurationVector.h"
#include "keywords/double.h"
#include "keywords/function1D.h"
#include "keywords/integer.h"
#include "keywords/module.h"
#include "keywords/moduleVector.h"
#include "keywords/node.h"
#include "keywords/nodeValue.h"
#include "keywords/nodeVector.h"
#include "keywords/range.h"
#include "keywords/species.h"
#include "keywords/stdString.h"
#include "keywords/vec3Double.h"
#include "keywords/vec3Integer.h"
#include "keywords/vec3NodeValue.h"
#include "procedure/nodes/collect1D.h"
#include "procedure/nodes/regionBase.h"
#include "procedure/nodes/select.h"

// Static Singletons
std::vector<KeywordBase *> KeywordStore::allKeywords_;

/*
 * Keyword Data
 */

// Add keyword to current section / group
void KeywordStore::addKeywordToCurrentGroup(KeywordBase *keyword, KeywordBase::KeywordType type)
{
    if (!currentGroup_)
        setOrganisation("_DEFAULT_SECTION", "_NO_HEADER");

    currentGroup_->get().addKeyword(keyword, type);
}

// Return named group, if it exists
OptionalReferenceWrapper<KeywordStoreGroup> KeywordStore::getGroup(std::string_view sectionName, std::string_view groupName,
                                                                   std::optional<std::string_view> groupDescription,
                                                                   bool createIfRequired)
{
    // Get the specified section
    auto sectionIt = std::find_if(sections_.begin(), sections_.end(),
                                  [sectionName](const auto &section) { return section.name() == sectionName; });
    if (sectionIt == sections_.end())
    {
        if (createIfRequired)
        {
            auto &newSection = sections_.emplace_back(sectionName);
            return newSection.getGroup(groupName, groupDescription, true);
        }
        else
            return {};
    }

    return sectionIt->getGroup(groupName, groupDescription, createIfRequired);
}

// Set current group and section organisation
void KeywordStore::setOrganisation(std::string_view sectionName, std::optional<std::string_view> groupName,
                                   std::optional<std::string_view> groupDescription)
{
    currentGroup_ = getGroup(sectionName, groupName ? *groupName : "_NO_HEADER", groupDescription, true);
}

// Find named keyword
std::optional<KeywordStoreEntry> KeywordStore::find(std::string_view name)
{
    for (const auto &section : sections_)
    {
        auto optKeyword = section.find(name);
        if (optKeyword)
            return optKeyword;
    }

    return {};
}
std::optional<KeywordStoreEntry> KeywordStore::find(std::string_view name) const
{
    for (const auto &section : sections_)
    {
        auto optKeyword = section.find(name);
        if (optKeyword)
            return optKeyword;
    }

    return {};
}

// Return "Target" group keywords
std::vector<KeywordBase *> KeywordStore::targetKeywords()
{
    // Return the keywords in the Options / Target section / group, if it exists
    auto optGroup = getGroup("Options", "Targets");
    if (!optGroup)
        return {};

    // Flatten the keywords in this group into a plain vector of pointers
    auto &group = optGroup->get();
    std::vector<KeywordBase *> targets;
    std::transform(group.keywords().begin(), group.keywords().end(), std::back_inserter(targets),
                   [](const auto &kd) { return kd.first; });
    return targets;
}

// Return defined keyword sections
const std::vector<KeywordStoreSection> &KeywordStore::sections() const { return sections_; }

// Return number of visible keywords defined over all sections
int KeywordStore::nVisibleKeywords() const
{
    return std::accumulate(sections_.begin(), sections_.end(), 0,
                           [](const auto acc, const auto &section) { return acc + section.nKeywords(); });
}

/*
 * Read / Write
 */

// Try to deserialise a single keyword through the specified LineParser
KeywordBase::ParseResult KeywordStore::deserialise(LineParser &parser, const CoreData &coreData, int startArg)
{
    // Do we recognise the first item (the 'keyword')?
    auto optKeyword = find(parser.argsv(startArg));
    if (!optKeyword)
        return KeywordBase::ParseResult::Unrecognised;

    auto &[keyword, keywordType] = *optKeyword;
    auto deprecated = keywordType == KeywordBase::KeywordType::Deprecated;

    // We recognised the keyword - check the number of arguments we have against the min / max for the keyword
    if (!keyword->validNArgs(parser.nArgs() - startArg - 1))
        return deprecated ? KeywordBase::ParseResult::Deprecated : KeywordBase::ParseResult::Failed;

    // All OK, so parse the keyword
    if (!keyword->deserialise(parser, startArg + 1, coreData))
    {
        Messenger::error("Failed to parse arguments for keyword '{}'.\n", keyword->name());
        return deprecated ? KeywordBase::ParseResult::Deprecated : KeywordBase::ParseResult::Failed;
    }

    return deprecated ? KeywordBase::ParseResult::Deprecated : KeywordBase::ParseResult::Success;
}

// Write all keywords to specified LineParser
bool KeywordStore::serialise(LineParser &parser, std::string_view prefix, bool onlyIfSet) const
{
    for (const auto &section : sections_)
        for (const auto &group : section.groups())
            for (const auto &[keyword, keywordType] : group.keywords())
            {
                if (keywordType == KeywordBase::KeywordType::Deprecated)
                    continue;
                if (!keyword->serialise(parser, keyword->name(), prefix))
                    return false;
            }

    return true;
}

//// Local template for handling boilerplate of casting the keyword
template <typename K> K *getKeyword(std::string_view name, std::optional<KeywordStoreEntry> optKeyword)
{
    if (!optKeyword)
        throw(std::runtime_error(fmt::format("Keyword '{}' cannot be retrieved as it doesn't exist.\n", name)));
    K *keyword = dynamic_cast<K *>(optKeyword->first);
    if (!keyword)
        throw(std::runtime_error(fmt::format("Keyword '{}' is not of type '{}'.\n", name, typeid(K).name())));
    return keyword;
}

bool KeywordStore::set(std::string_view name, const bool value)
{
    getKeyword<BoolKeyword>(name, find(name))->setData(value);
    return true;
}
bool KeywordStore::set(std::string_view name, const double value)
{
    return getKeyword<DoubleKeyword>(name, find(name))->setData(value);
}
bool KeywordStore::set(std::string_view name, const int value)
{
    return getKeyword<IntegerKeyword>(name, find(name))->setData(value);
}
bool KeywordStore::set(std::string_view name, const std::shared_ptr<Collect1DProcedureNode> value)
{
    return getKeyword<NodeKeyword<Collect1DProcedureNode>>(name, find(name))->setData(value);
}
bool KeywordStore::set(std::string_view name, const std::vector<std::shared_ptr<const Collect1DProcedureNode>> value)
{
    return getKeyword<NodeVectorKeyword<Collect1DProcedureNode>>(name, find(name))->setData(value);
}
bool KeywordStore::set(std::string_view name, const std::shared_ptr<RegionProcedureNodeBase> value)
{
    return getKeyword<NodeKeyword<RegionProcedureNodeBase>>(name, find(name))->setData(value);
}
bool KeywordStore::set(std::string_view name, const std::shared_ptr<SelectProcedureNode> value)
{
    return getKeyword<NodeKeyword<SelectProcedureNode>>(name, find(name))->setData(value);
}
bool KeywordStore::set(std::string_view name, const ConstNodeVector<SelectProcedureNode> value)
{
    return getKeyword<NodeVectorKeyword<SelectProcedureNode>>(name, find(name))->setData(value);
}
bool KeywordStore::set(std::string_view name, const std::vector<Module *> value)
{
    getKeyword<ModuleVectorKeyword>(name, find(name))->data() = value;
    return true;
}
bool KeywordStore::set(std::string_view name, const Module *value)
{
    getKeyword<ModuleKeywordBase>(name, find(name))->setData(value);
    return true;
}
bool KeywordStore::set(std::string_view name, Configuration *value)
{
    getKeyword<ConfigurationKeyword>(name, find(name))->data() = value;
    return true;
}
bool KeywordStore::set(std::string_view name, const std::vector<Configuration *> value)
{
    getKeyword<ConfigurationVectorKeyword>(name, find(name))->data() = value;
    return true;
}
bool KeywordStore::set(std::string_view name, const Species *value)
{
    getKeyword<SpeciesKeyword>(name, find(name))->data() = value;
    return true;
}
bool KeywordStore::set(std::string_view name, const std::string value)
{
    getKeyword<StringKeyword>(name, find(name))->data() = value;
    return true;
}
bool KeywordStore::set(std::string_view name, const Functions::Function1DWrapper value)
{
    return getKeyword<Function1DKeyword>(name, find(name))->setData(value);
}
bool KeywordStore::set(std::string_view name, const NodeValueProxy value)
{
    return getKeyword<NodeValueKeyword>(name, find(name))->setData(value);
}
bool KeywordStore::set(std::string_view name, const Vec3<int> value)
{
    return getKeyword<Vec3IntegerKeyword>(name, find(name))->setData(value);
}
bool KeywordStore::set(std::string_view name, const Vec3<double> value)
{
    return getKeyword<Vec3DoubleKeyword>(name, find(name))->setData(value);
}
bool KeywordStore::set(std::string_view name, const Vec3<NodeValue> value)
{
    return getKeyword<Vec3NodeValueKeyword>(name, find(name))->setData(value);
}
bool KeywordStore::set(std::string_view name, const Range value)
{
    return getKeyword<RangeKeyword>(name, find(name))->setData(value);
}

// Retrieve a Configuration by keyword name
Configuration *KeywordStore::getConfiguration(std::string_view name) const
{
    return getKeyword<ConfigurationKeyword>(name, find(name))->data();
}

// Retrieve a Species by keyword name
const Species *KeywordStore::getSpecies(std::string_view name) const
{
    return getKeyword<SpeciesKeyword>(name, find(name))->data();
}

// Retrieve a vector of Configurations by keyword name
std::vector<Configuration *> KeywordStore::getVectorConfiguration(std::string_view name) const
{
    return getKeyword<ConfigurationVectorKeyword>(name, find(name))->data();
}

// Retrieve an Integer by keyword name
int KeywordStore::getInt(std::string_view name) const { return getKeyword<IntegerKeyword>(name, find(name))->data(); }

// Retrieve a vector of Modules by keyword name
const std::vector<Module *> &KeywordStore::getVectorModule(std::string_view name) const
{
    return getKeyword<ModuleVectorKeyword>(name, find(name))->data();
}

// Turn first character of keyword label to lower case to match
// convention with the rest of the file format.
std::string toml_format(const std::string_view original)
{
    auto result = std::string(original);
    result[0] = tolower(result[0]);
    return result;
}

// Apply the terms in the keyword store to a node
SerialisedValue KeywordStore::serialiseOnto(SerialisedValue node) const
{
    for (const auto &section : sections_)
        for (const auto &group : section.groups())
            for (const auto &[keyword, keywordType] : group.keywords())
                if (!keyword->isDefault())
                {
                    auto value = keyword->serialise();
                    if (keywordType != KeywordBase::KeywordType::Deprecated && !value.is_uninitialized())
                        node[toml_format(keyword->name())] = value;
                }
    return node;
}

// Pull keywords from entries in table
void KeywordStore::deserialiseFrom(const SerialisedValue &node, const CoreData &coreData)
{
    for (const auto &section : sections_)
        for (const auto &group : section.groups())
            for (const auto &[keyword, keywordType] : group.keywords())
                if (node.contains(toml_format(keyword->name())))
                    keyword->deserialise(node.at(toml_format(keyword->name())), coreData);
}
