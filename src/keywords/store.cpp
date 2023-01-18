// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "keywords/store.h"
#include "base/lineparser.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/configurationvector.h"
#include "keywords/double.h"
#include "keywords/integer.h"
#include "keywords/module.h"
#include "keywords/modulevector.h"
#include "keywords/nodevalue.h"
#include "keywords/nodevector.h"
#include "keywords/species.h"
#include "keywords/stdstring.h"
#include "keywords/vec3double.h"
#include "keywords/vec3nodevalue.h"
#include "procedure/nodes/nodes.h"

// Static Singletons
std::vector<KeywordBase *> KeywordStore::allKeywords_;

// Find named keyword
KeywordBase *KeywordStore::find(std::string_view name)
{
    auto it = keywords_.find(name);
    if (it == keywords_.end())
        return nullptr;
    return it->second;
}
const KeywordBase *KeywordStore::find(std::string_view name) const
{
    auto it = keywords_.find(name);
    if (it == keywords_.end())
        return nullptr;
    return it->second;
}

// Return keywords
const std::map<std::string_view, KeywordBase *> &KeywordStore::keywords() const { return keywords_; }

// Return "Target" group keywords
const std::vector<KeywordBase *> &KeywordStore::targetsGroup() const { return targetsGroup_; }

// Return restartable keywords
const std::vector<KeywordBase *> &KeywordStore::restartables() const { return restartables_; }

// Return keyword group mappings
const std::vector<std::pair<std::string_view, std::vector<KeywordBase *>>> &KeywordStore::displayGroups() const
{
    return displayGroups_;
};

/*
 * Read / Write
 */

// Try to deserialise a single keyword through the specified LineParser
KeywordBase::ParseResult KeywordStore::deserialise(LineParser &parser, const CoreData &coreData, int startArg)
{
    // Do we recognise the first item (the 'keyword')?
    auto deprecated = false;
    auto it = deprecatedKeywords_.find(parser.argsv(startArg));
    if (it != deprecatedKeywords_.end())
        deprecated = true;
    else
        it = keywords_.find(parser.argsv(startArg));
    if (it == keywords_.end())
        return KeywordBase::ParseResult::Unrecognised;
    auto *keyword = it->second;

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
    for (const auto &[name, keyword] : keywords_)
        if (!keyword->serialise(parser, name, prefix))
            return false;

    return true;
}

// Local template for handling boilerplate of casting the keyword
template <typename K> K *getKeyword(const std::map<std::string_view, KeywordBase *> &keywords, std::string_view name)
{
    auto it = keywords.find(name);
    if (it == keywords.end())
        throw(std::runtime_error(fmt::format("Keyword '{}' cannot be set as it doesn't exist.\n", name)));
    K *result = dynamic_cast<K *>(it->second);
    if (!result)
        throw(std::runtime_error(fmt::format("Keyword '{}' is not of type '{}'.\n", name, typeid(K).name())));
    return result;
}

void KeywordStore::set(std::string_view name, const bool value) { getKeyword<BoolKeyword>(keywords_, name)->setData(value); }
void KeywordStore::set(std::string_view name, const double value)
{
    getKeyword<DoubleKeyword>(keywords_, name)->setData(value);
}
void KeywordStore::set(std::string_view name, const int value) { getKeyword<IntegerKeyword>(keywords_, name)->setData(value); }
void KeywordStore::set(std::string_view name, const std::shared_ptr<Collect1DProcedureNode> value)
{
    getKeyword<NodeKeyword<Collect1DProcedureNode>>(keywords_, name)->data() = value;
}
void KeywordStore::set(std::string_view name, const std::vector<std::shared_ptr<const Collect1DProcedureNode>> value)
{
    getKeyword<NodeVectorKeyword<Collect1DProcedureNode>>(keywords_, name)->data() = value;
}
void KeywordStore::set(std::string_view name, const std::shared_ptr<RegionProcedureNodeBase> value)
{
    getKeyword<NodeKeyword<RegionProcedureNodeBase>>(keywords_, name)->data() = value;
}
void KeywordStore::set(std::string_view name, const std::shared_ptr<SelectProcedureNode> value)
{
    getKeyword<NodeKeyword<SelectProcedureNode>>(keywords_, name)->data() = value;
}
void KeywordStore::set(std::string_view name, const ConstNodeVector<SelectProcedureNode> value)
{
    getKeyword<NodeVectorKeyword<SelectProcedureNode>>(keywords_, name)->data() = value;
}
void KeywordStore::set(std::string_view name, const std::vector<Module *> value)
{
    getKeyword<ModuleVectorKeyword>(keywords_, name)->data() = value;
}
void KeywordStore::set(std::string_view name, const Module *value)
{
    getKeyword<ModuleKeywordBase>(keywords_, name)->setData(value);
}
void KeywordStore::set(std::string_view name, Configuration *value)
{
    getKeyword<ConfigurationKeyword>(keywords_, name)->data() = value;
}
void KeywordStore::set(std::string_view name, const std::vector<Configuration *> value)
{
    getKeyword<ConfigurationVectorKeyword>(keywords_, name)->data() = value;
}
void KeywordStore::set(std::string_view name, const Species *value)
{
    getKeyword<SpeciesKeyword>(keywords_, name)->data() = value;
}
void KeywordStore::set(std::string_view name, const std::string value)
{
    getKeyword<StringKeyword>(keywords_, name)->data() = value;
}
void KeywordStore::set(std::string_view name, const Vec3<double> value)
{
    getKeyword<Vec3DoubleKeyword>(keywords_, name)->setData(value);
}
void KeywordStore::set(std::string_view name, const Vec3<NodeValue> value)
{
    getKeyword<Vec3NodeValueKeyword>(keywords_, name)->setData(value);
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
std::vector<Module *> KeywordStore::getVectorModule(std::string_view name) const
{
    return getKeyword<ModuleVectorKeyword>(keywords_, name)->data();
}
