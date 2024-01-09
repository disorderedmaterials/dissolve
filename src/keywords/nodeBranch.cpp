// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/nodeBranch.h"
#include "base/lineParser.h"
#include "procedure/nodes/node.h"
#include "procedure/nodes/sequence.h"

NodeBranchKeyword::NodeBranchKeyword(ProcedureNodeSequence &data) : KeywordBase(typeid(this)), data_(data) {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int NodeBranchKeyword::minArguments() const { return 0; }

// Return maximum number of arguments accepted
std::optional<int> NodeBranchKeyword::maxArguments() const { return 0; }

// Deserialise from supplied LineParser, starting at given argument offset
bool NodeBranchKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    return data_.deserialise(parser, coreData);
}

// Serialise data to specified LineParser
bool NodeBranchKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (data_.nNodes() == 0)
        return true;

    // Write keyword name as the start of the branch
    if (!parser.writeLineF("{}{}\n", prefix, name()))
        return false;

    // Write branch information
    if (!data_.serialise(parser, fmt::format("{}  ", prefix)))
        return false;

    // Write end keyword based on the name
    if (!parser.writeLineF("{}End{}\n", prefix, name()))
        return false;

    return true;
}

// Has not changed from initial value
bool NodeBranchKeyword::isDefault() const { return data_.nNodes() == 0; }

// Express as a serialisable value
SerialisedValue NodeBranchKeyword::serialise() const
{
    if (isDefault())
        return {};
    return data_.serialise();
}

// Read values from a serialisable value
void NodeBranchKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    data_.deserialise(node, coreData);
}
