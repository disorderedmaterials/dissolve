// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

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

// Set current group and section organisation
void KeywordStore::setOrganisation(std::string_view groupName, std::string_view sectionName)
{
    currentGroupName_ = groupName;
    currentSectionName_ = sectionName;
}

// Find named keyword
OptionalReferenceWrapper<KeywordStoreData> KeywordStore::find(std::string_view name)
{
    auto it = std::find_if(keywords_.begin(), keywords_.end(), [name](auto &kd) { return kd.keyword()->name() == name; });
    if (it == keywords_.end())
        return {};
    return *it;
}
OptionalReferenceWrapper<const KeywordStoreData> KeywordStore::find(std::string_view name) const
{
    auto it = std::find_if(keywords_.begin(), keywords_.end(), [name](const auto &kd) { return kd.keyword()->name() == name; });
    if (it == keywords_.end())
        return {};
    return *it;
}

// Return keywords
const std::vector<KeywordStoreData> &KeywordStore::keywords() const { return keywords_; }

// Return "Target" group keywords
std::vector<KeywordBase *> KeywordStore::targetKeywords()
{
    std::vector<KeywordBase *> targets;
    for (auto &kd : keywords_)
        if (kd.type() == KeywordStoreData::KeywordType::Target)
            targets.push_back(kd.keyword());
    return targets;
}

// Return keyword organisation based on group and section names
std::pair<KeywordStore::KeywordStoreIndex, KeywordStore::KeywordStoreMap> KeywordStore::keywordOrganisation()
{
    // Organise keywords together by group and section names, if they have them
    KeywordStore::KeywordStoreIndex index;
    KeywordStore::KeywordStoreMap map;

    // Iterate over the defined keywords
    for (auto &kd : keywords_)
    {
        // Don't include deprecated keywords or those with no defined group
        if (kd.type() == KeywordStoreData::KeywordType::Deprecated || kd.groupName().empty())
            continue;

        // The order of the defined keywords is important, since it reflects the desired group/section order in the GUI
        // Check for the presence of the current group / section
        auto groupIt = map.find(kd.groupName());
        if (groupIt != map.end())
        {
            auto sectionIt = groupIt->second.find(kd.sectionName());
            if (sectionIt != groupIt->second.end())
                sectionIt->second.push_back(kd.keyword());
            else
            {
                groupIt->second[kd.sectionName()].push_back(kd.keyword());

                // Find the existing index entry
                auto it =
                    std::find_if(index.begin(), index.end(), [kd](const auto &idx) { return kd.groupName() == idx.first; });
                it->second.push_back(kd.sectionName());
            }
        }
        else
        {
            map[kd.groupName()][kd.sectionName()].push_back(kd.keyword());
            index.emplace_back(kd.groupName(), std::vector{kd.sectionName()});
        }
    }

    return {index, map};
}

/*
 * Read / Write
 */

// Try to deserialise a single keyword through the specified LineParser
KeywordBase::ParseResult KeywordStore::deserialise(LineParser &parser, const CoreData &coreData, int startArg)
{
    // Do we recognise the first item (the 'keyword')?
    auto optKwdData = find(parser.argsv(startArg));
    if (!optKwdData)
        return KeywordBase::ParseResult::Unrecognised;

    auto &keywordData = optKwdData->get();
    auto *keyword = keywordData.keyword();
    auto deprecated = keywordData.type() == KeywordStoreData::KeywordType::Deprecated;

    // We recognised the keyword - check the number of arguments we have against the min / max for the keyword
    if (!keyword->validNArgs(parser.nArgs() - startArg - 1))
        return deprecated ? KeywordBase::ParseResult::Deprecated : KeywordBase::ParseResult::Failed;

    // All OK, so parse the keyword
    if (!keyword->deserialise(parser, startArg + 1, coreData))
    {
        Messenger::error("Failed to parse arguments for keyword '{}'.\n", keywordData.keyword()->name());
        return deprecated ? KeywordBase::ParseResult::Deprecated : KeywordBase::ParseResult::Failed;
    }

    return deprecated ? KeywordBase::ParseResult::Deprecated : KeywordBase::ParseResult::Success;
}

// Write all keywords to specified LineParser
bool KeywordStore::serialise(LineParser &parser, std::string_view prefix, bool onlyIfSet) const
{
    for (const auto &kd : keywords_)
    {
        if (kd.type() == KeywordStoreData::KeywordType::Deprecated)
            continue;
        if (!kd.keyword()->serialise(parser, kd.keyword()->name(), prefix))
            return false;
    }

    return true;
}

// Local template for handling boilerplate of casting the keyword
template <typename K> K *getKeyword(std::vector<KeywordStoreData> &keywords, std::string_view name)
{
    auto it = std::find_if(keywords.begin(), keywords.end(), [name](auto &kd) { return kd.keyword()->name() == name; });
    if (it == keywords.end())
        throw(std::runtime_error(fmt::format("Keyword '{}' cannot be retrieved as it doesn't exist.\n", name)));
    K *result = dynamic_cast<K *>(it->keyword());
    if (!result)
        throw(std::runtime_error(fmt::format("Keyword '{}' is not of type '{}'.\n", name, typeid(K).name())));
    return result;
}
template <typename K> const K *getKeyword(const std::vector<KeywordStoreData> &keywords, std::string_view name)
{
    auto it = std::find_if(keywords.begin(), keywords.end(), [name](auto &kd) { return kd.keyword()->name() == name; });
    if (it == keywords.end())
        throw(std::runtime_error(fmt::format("Keyword '{}' cannot be retrieved as it doesn't exist.\n", name)));
    const K *result = dynamic_cast<const K *>(it->keyword());
    if (!result)
        throw(std::runtime_error(fmt::format("Keyword '{}' is not of type '{}'.\n", name, typeid(K).name())));
    return result;
}

bool KeywordStore::set(std::string_view name, const bool value)
{
    getKeyword<BoolKeyword>(keywords_, name)->setData(value);
    return true;
}
bool KeywordStore::set(std::string_view name, const double value)
{
    return getKeyword<DoubleKeyword>(keywords_, name)->setData(value);
}
bool KeywordStore::set(std::string_view name, const int value)
{
    return getKeyword<IntegerKeyword>(keywords_, name)->setData(value);
}
bool KeywordStore::set(std::string_view name, const std::shared_ptr<Collect1DProcedureNode> value)
{
    return getKeyword<NodeKeyword<Collect1DProcedureNode>>(keywords_, name)->setData(value);
}
bool KeywordStore::set(std::string_view name, const std::vector<std::shared_ptr<const Collect1DProcedureNode>> value)
{
    return getKeyword<NodeVectorKeyword<Collect1DProcedureNode>>(keywords_, name)->setData(value);
}
bool KeywordStore::set(std::string_view name, const std::shared_ptr<RegionProcedureNodeBase> value)
{
    return getKeyword<NodeKeyword<RegionProcedureNodeBase>>(keywords_, name)->setData(value);
}
bool KeywordStore::set(std::string_view name, const std::shared_ptr<SelectProcedureNode> value)
{
    return getKeyword<NodeKeyword<SelectProcedureNode>>(keywords_, name)->setData(value);
}
bool KeywordStore::set(std::string_view name, const ConstNodeVector<SelectProcedureNode> value)
{
    return getKeyword<NodeVectorKeyword<SelectProcedureNode>>(keywords_, name)->setData(value);
}
bool KeywordStore::set(std::string_view name, const std::vector<Module *> value)
{
    getKeyword<ModuleVectorKeyword>(keywords_, name)->data() = value;
    return true;
}
bool KeywordStore::set(std::string_view name, const Module *value)
{
    getKeyword<ModuleKeywordBase>(keywords_, name)->setData(value);
    return true;
}
bool KeywordStore::set(std::string_view name, Configuration *value)
{
    getKeyword<ConfigurationKeyword>(keywords_, name)->data() = value;
    return true;
}
bool KeywordStore::set(std::string_view name, const std::vector<Configuration *> value)
{
    getKeyword<ConfigurationVectorKeyword>(keywords_, name)->data() = value;
    return true;
}
bool KeywordStore::set(std::string_view name, const Species *value)
{
    getKeyword<SpeciesKeyword>(keywords_, name)->data() = value;
    return true;
}
bool KeywordStore::set(std::string_view name, const std::string value)
{
    getKeyword<StringKeyword>(keywords_, name)->data() = value;
    return true;
}
bool KeywordStore::set(std::string_view name, const Functions::Function1DWrapper value)
{
    return getKeyword<Function1DKeyword>(keywords_, name)->setData(value);
}
bool KeywordStore::set(std::string_view name, const NodeValueProxy value)
{
    return getKeyword<NodeValueKeyword>(keywords_, name)->setData(value);
}
bool KeywordStore::set(std::string_view name, const Vec3<int> value)
{
    return getKeyword<Vec3IntegerKeyword>(keywords_, name)->setData(value);
}
bool KeywordStore::set(std::string_view name, const Vec3<double> value)
{
    return getKeyword<Vec3DoubleKeyword>(keywords_, name)->setData(value);
}
bool KeywordStore::set(std::string_view name, const Vec3<NodeValue> value)
{
    return getKeyword<Vec3NodeValueKeyword>(keywords_, name)->setData(value);
}
bool KeywordStore::set(std::string_view name, const Range value)
{
    return getKeyword<RangeKeyword>(keywords_, name)->setData(value);
}

// Retrieve a Configuration by keyword name
Configuration *KeywordStore::getConfiguration(std::string_view name) const
{
    return getKeyword<ConfigurationKeyword>(keywords_, name)->data();
}

// Retrieve a Species by keyword name
const Species *KeywordStore::getSpecies(std::string_view name) const
{
    return getKeyword<SpeciesKeyword>(keywords_, name)->data();
}

// Retrieve a vector of Configurations by keyword name
std::vector<Configuration *> KeywordStore::getVectorConfiguration(std::string_view name) const
{
    return getKeyword<ConfigurationVectorKeyword>(keywords_, name)->data();
}

// Retrieve an Integer by keyword name
int KeywordStore::getInt(std::string_view name) const { return getKeyword<IntegerKeyword>(keywords_, name)->data(); }

// Retrieve a vector of Modules by keyword name
const std::vector<Module *> &KeywordStore::getVectorModule(std::string_view name) const
{
    return getKeyword<ModuleVectorKeyword>(keywords_, name)->data();
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
    for (auto &k : keywords())
        if (!k.keyword()->isDefault())
        {
            auto value = k.keyword()->serialise();
            if (k.type() != KeywordStoreData::KeywordType::Deprecated && !value.is_uninitialized())
                node[toml_format(k.keyword()->name())] = value;
        }
    return node;
}

// Pull keywords from entries in table
void KeywordStore::deserialiseFrom(const SerialisedValue &node, const CoreData &coreData)
{
    for (auto &k : keywords_)
        if (node.contains(toml_format(k.keyword()->name())))
            k.keyword()->deserialise(node.at(toml_format(k.keyword()->name())), coreData);
}
