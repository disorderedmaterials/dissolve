// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "keywords/isotopologueSet.h"
#include "base/lineParser.h"
#include "classes/coreData.h"
#include "classes/species.h"

IsotopologueSetKeyword::IsotopologueSetKeyword(IsotopologueSet &data) : KeywordBase(typeid(this)), data_(data) {}

/*
 * Data
 */

// Return reference to data
IsotopologueSet &IsotopologueSetKeyword::data() { return data_; }
const IsotopologueSet &IsotopologueSetKeyword::data() const { return data_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int IsotopologueSetKeyword::minArguments() const { return 3; }

// Return maximum number of arguments accepted
std::optional<int> IsotopologueSetKeyword::maxArguments() const { return 3; }

// Deserialise from supplied LineParser, starting at given argument offset
bool IsotopologueSetKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Find specified Species (first argument)
    Species *sp = coreData.findSpecies(DissolveSys::niceName(parser.argsv(startArg)));
    if (!sp)
        return Messenger::error("Error defining Isotopologue reference - no Species named '{}' exists.\n",
                                DissolveSys::niceName(parser.argsv(startArg)));

    // Finally, locate isotopologue definition for species (second argument)
    const Isotopologue *iso = sp->findIsotopologue(parser.argsv(startArg + 1));
    if (!iso)
        return Messenger::error("Error defining Isotopologue reference - no Isotopologue named '{}' exists for Species '{}'.\n",
                                parser.argsv(startArg + 1), sp->name());

    // Add the isotopologue to the set
    data_.add(iso, parser.argd(startArg + 2));

    return true;
}

// Serialise data to specified LineParser
bool IsotopologueSetKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    for (auto topes : data_.isotopologues())
        for (const auto &[iso, weight] : topes.mix())
        {
            if (!parser.writeLineF("{}{}  '{}'  '{}'  {}\n", prefix, keywordName, topes.species()->name(), iso->name(), weight))
                return false;
        }

    return true;
}

/*
 * Data Management
 */

// Prune any references to the supplied Species in the contained data
void IsotopologueSetKeyword::removeReferencesTo(Species *sp) { data_.remove(sp); }

// Prune any references to the supplied Isotopologue in the contained data
void IsotopologueSetKeyword::removeReferencesTo(Isotopologue *iso) { data_.remove(iso); }

// Express as a serialisable value
SerialisedValue IsotopologueSetKeyword::serialise() const { return data_; }

// Read values from a serialisable value
void IsotopologueSetKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    data_.deserialise(node, coreData);
}

// Has not changed from initial value
bool IsotopologueSetKeyword::isDefault() const { return false; }
