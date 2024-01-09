// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/speciesSite.h"
#include "base/lineParser.h"
#include "classes/coreData.h"
#include "classes/species.h"

SpeciesSiteKeyword::SpeciesSiteKeyword(const SpeciesSite *&data, bool axesRequired)
    : KeywordBase(typeid(this)), data_(data), axesRequired_(axesRequired)
{
}

/*
 * Data
 */

// Return reference to data
const SpeciesSite *&SpeciesSiteKeyword::data() { return data_; }
const SpeciesSite *&SpeciesSiteKeyword::data() const { return data_; }

// Return whether axes are required for the site
bool SpeciesSiteKeyword::axesRequired() const { return axesRequired_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int SpeciesSiteKeyword::minArguments() const { return 2; }

// Return maximum number of arguments accepted
std::optional<int> SpeciesSiteKeyword::maxArguments() const { return 2; }

// Deserialise from supplied LineParser, starting at given argument offset
bool SpeciesSiteKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Find target Species (first argument)
    Species *sp = coreData.findSpecies(parser.argsv(startArg));
    if (!sp)
    {
        Messenger::error("Error setting SpeciesSite - no Species named '{}' exists.\n", parser.argsv(startArg));
        return false;
    }

    // Find specified Site (second argument) in the Species
    data_ = sp->findSite(parser.argsv(startArg + 1));
    if (!data_)
        return Messenger::error("Error setting SpeciesSite - no such site named '{}' exists in Species '{}'.\n",
                                parser.argsv(startArg + 1), sp->name());

    if (axesRequired_ && (!data_->hasAxes()))
        return Messenger::error(
            "Can't select site '{}' for keyword '{}', as the keyword requires axes specifications to be present.\n",
            data_->name(), name());

    return true;
}

// Serialise data to specified LineParser
bool SpeciesSiteKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (data_)
    {
        if (!parser.writeLineF("{}{}  '{}'  '{}'\n", prefix, keywordName, data_->parent()->name(), data_->name()))
            return false;
    }

    return true;
}

/*
 * Object Management
 */

// Prune any references to the supplied Species in the contained data
void SpeciesSiteKeyword::removeReferencesTo(Species *sp)
{
    if (data_ && (data_->parent() == sp))
        data_ = nullptr;
}

// Prune any references to the supplied Site in the contained data
void SpeciesSiteKeyword::removeReferencesTo(SpeciesSite *spSite)
{
    if (data_ == spSite)
        data_ = nullptr;
}

// Express as a serialisable value
SerialisedValue SpeciesSiteKeyword::serialise() const
{
    return {{"species", data_->parent()->name()}, {"site", data_->name()}};
}

// Read values from a serialisable value
void SpeciesSiteKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    KeywordBase::deserialise(node, coreData);
    std::string species = toml::find<std::string>(node, "species");
    std::string site = toml::find<std::string>(node, "site");
    // Find target Species (first argument)
    Species *sp = coreData.findSpecies(species);
    if (!sp)
    {
        throw toml::type_error(fmt::format("Error setting SpeciesSite - no Species named '{}' exists.\n", species),
                               node.location());
    }

    // Find specified Site (second argument) in the Species
    data_ = sp->findSite(site);
    if (!data_)
        throw toml::type_error(
            fmt::format("Error setting SpeciesSite - no such site named '{}' exists in Species '{}'.\n", site, sp->name()),
            node.location());

    if (axesRequired_ && (!data_->hasAxes()))
        throw toml::type_error(
            fmt::format("Can't select site '{}' for keyword '{}', as the keyword requires axes specifications to be present.\n",
                        data_->name(), name()),
            node.location());
}
