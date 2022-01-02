// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "keywords/store.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/configurationvector.h"
#include "keywords/double.h"
#include "keywords/integer.h"
#include "keywords/module.h"
#include "keywords/modulevector.h"
#include "keywords/nodevector.h"
#include "keywords/stdstring.h"
#include "keywords/vec3double.h"
#include "procedure/nodes/nodes.h"

// Static Singletons
std::vector<KeywordBase *> KeywordStore::allKeywords_;

/*
 * Keyword Map
 */

KeywordTypeMap::KeywordTypeMap()
{
    // PODs
    registerDirectMapping<bool, BoolKeyword>([](BoolKeyword *keyword, const double value) { return keyword->setData(value); });
    // -- Double and int keywords must use the setData() function as they have validation
    registerDirectMapping<double, DoubleKeyword>(
        [](DoubleKeyword *keyword, const double value) { return keyword->setData(value); });
    registerDirectMapping<int, IntegerKeyword>(
        [](IntegerKeyword *keyword, const int value) { return keyword->setData(value); });

    // Custom classes
    registerDirectMapping<std::shared_ptr<Collect1DProcedureNode>, NodeKeyword<Collect1DProcedureNode>>();
    registerDirectMapping<std::vector<std::shared_ptr<const Collect1DProcedureNode>>,
                          NodeVectorKeyword<Collect1DProcedureNode>>();
    registerDirectMapping<std::shared_ptr<SelectProcedureNode>, NodeKeyword<SelectProcedureNode>>();
    registerDirectMapping<std::vector<std::shared_ptr<const SelectProcedureNode>>, NodeVectorKeyword<SelectProcedureNode>>();
    registerDirectMapping<std::vector<Module *>, ModuleVectorKeyword>();
    registerDirectMapping<const Module *, ModuleKeywordBase>(
        [](ModuleKeywordBase *keyword, const Module *module) { return keyword->setData(module); },
        [](ModuleKeywordBase *keyword) { return keyword->module(); });
    registerDirectMapping<Configuration *, ConfigurationKeyword>();
    registerDirectMapping<std::vector<Configuration *>, ConfigurationVectorKeyword>();

    // STL / Common Classes
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
 * Keyword Store
 */

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
 * Set
 */

// Return the setter instance
const KeywordTypeMap &KeywordStore::setters()
{
    static const KeywordTypeMap setters;

    return setters;
}

// Set specified keyword with supplied data
void KeywordStore::set(std::string_view name, const std::any value)
{
    auto it = keywords_.find(name);
    if (it == keywords_.end())
        throw(std::runtime_error(fmt::format("No keyword named '{}' exists to set.\n", name)));

    // Attempt to set the keyword
    setters().set(it->second, value);
}

/*
 * Read / Write
 */

// Try to deserialise a single keyword through the specified LineParser
KeywordBase::ParseResult KeywordStore::deserialise(LineParser &parser, const CoreData &coreData, int startArg)
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
    if (!keyword->deserialise(parser, startArg + 1, coreData))
    {
        Messenger::error("Failed to parse arguments for keyword '{}'.\n", keyword->name());
        return KeywordBase::Failed;
    }

    return KeywordBase::Success;
}

// Write all keywords to specified LineParser
bool KeywordStore::serialise(LineParser &parser, std::string_view prefix, bool onlyIfSet) const
{
    for (const auto &[name, keyword] : keywords_)
        if (!keyword->serialise(parser, name, prefix))
            return false;

    return true;
}
