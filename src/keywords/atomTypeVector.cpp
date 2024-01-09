// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/atomTypeVector.h"
#include "base/lineParser.h"
#include "classes/atomType.h"
#include "classes/coreData.h"
#include "templates/algorithms.h"

AtomTypeVectorKeyword::AtomTypeVectorKeyword(std::vector<std::shared_ptr<AtomType>> &data)
    : KeywordBase(typeid(this)), data_(data)
{
}

/*
 * Data
 */

// Return reference to data vector
std::vector<std::shared_ptr<AtomType>> &AtomTypeVectorKeyword::data() { return data_; }
const std::vector<std::shared_ptr<AtomType>> &AtomTypeVectorKeyword::data() const { return data_; }

/*
 * Arguments
 */

// Return maximum number of arguments accepted
std::optional<int> AtomTypeVectorKeyword::maxArguments() const { return std::nullopt; }

// Deserialise from supplied LineParser, starting at given argument offset
bool AtomTypeVectorKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Loop over arguments which are AtomType names
    for (auto n = startArg; n < parser.nArgs(); ++n)
    {
        // Do we recognise the AtomType?
        auto it = std::find_if(coreData.atomTypes().begin(), coreData.atomTypes().end(),
                               [&parser, n](const auto atomType)
                               { return DissolveSys::sameString(atomType->name(), parser.argsv(n)); });
        if (it == coreData.atomTypes().end())
            return Messenger::error("Unrecognised AtomType '{}' given to '{}' keyword.\n", parser.argsv(n), name());
        auto atomType = *it;

        // If the AtomType is already present, complain
        if (std::find(data_.begin(), data_.end(), atomType) != data_.end())
            return Messenger::error("AtomType '{}' specified in selection twice.\n", parser.argsv(n));

        // All OK - add it to our vector
        data_.push_back(atomType);
    }

    return true;
}

// Serialise data to specified LineParser
bool AtomTypeVectorKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (data_.empty())
        return true;

    return parser.writeLineF("{}{}  {}\n", prefix, keywordName,
                             joinStrings(data_, "  ", [](const auto &at) { return at->name(); }));
}

/*
 * Object Management
 */

// Prune any references to the supplied AtomType in the contained data
void AtomTypeVectorKeyword::removeReferencesTo(std::shared_ptr<AtomType> at)
{
    auto it = std::find(data_.begin(), data_.end(), at);
    if (it != data_.end())
        data_.erase(it);
}

// Express as a serialisable value
SerialisedValue AtomTypeVectorKeyword::serialise() const
{
    return fromVector(data_, [](const auto &item) { return item->name(); });
}

// Read values from a serialisable value
void AtomTypeVectorKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    toVector(node,
             [this, &coreData](const auto &item)
             {
                 auto it = std::find_if(
                     coreData.atomTypes().begin(), coreData.atomTypes().end(),
                     [&item](const auto atomType)
                     { return DissolveSys::sameString(atomType->name(), std::string_view(std::string(item.as_string()))); });
                 if (it == coreData.atomTypes().end())
                     throw toml::type_error(fmt::format("Unrecognised AtomType '{}' given to '{}' keyword.\n",
                                                        std::string(item.as_string()), name()),
                                            item.location());
                 auto atomType = *it;

                 // If the AtomType is already present, complain
                 if (std::find(data_.begin(), data_.end(), atomType) != data_.end())
                     throw toml::type_error(
                         fmt::format("AtomType '{}' specified in selection twice.\n", std::string(item.as_string())),
                         item.location());

                 // All OK - add it to our vector
                 data_.push_back(atomType);
             });
}

// Has not changed from initial value
bool AtomTypeVectorKeyword::isDefault() const { return data_.empty(); }
