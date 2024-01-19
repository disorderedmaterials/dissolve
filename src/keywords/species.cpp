// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/species.h"
#include "base/lineParser.h"
#include "classes/coreData.h"
#include "classes/species.h"

SpeciesKeyword::SpeciesKeyword(const Species *&data) : KeywordBase(typeid(this)), data_(data) {}

/*
 * Data
 */

// Return reference to data
const Species *&SpeciesKeyword::data() { return data_; }
const Species *&SpeciesKeyword::data() const { return data_; }

/*
 * Arguments
 */

// Deserialise from supplied LineParser, starting at given argument offset
bool SpeciesKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Find target Species (first argument)
    data_ = coreData.findSpecies(parser.argsv(startArg));
    if (!data_)
        return Messenger::error("Error setting Species - no Species named '{}' exists.\n", parser.argsv(startArg));

    return true;
}

// Serialise data to specified LineParser
bool SpeciesKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (!data_)
        return true;

    return parser.writeLineF("{}{}  '{}'\n", prefix, keywordName, data_->name());
}

/*
 * Object Management
 */

// Prune any references to the supplied Species in the contained data
void SpeciesKeyword::removeReferencesTo(Species *sp)
{
    if (data_ == sp)
        data_ = nullptr;
}

// Express as a serialisable value
SerialisedValue SpeciesKeyword::serialise() const { return data_->name(); }

// Read values from a serialisable value
void SpeciesKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    data_ = coreData.findSpecies(std::string_view(std::string(node.as_string())));
}
