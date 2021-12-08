// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/vec3nodevalue.h"
#include "base/lineparser.h"
#include "procedure/nodes/node.h"

Vec3NodeValueKeyword::Vec3NodeValueKeyword(Vec3<NodeValue> &data, ProcedureNode *parentNode, Vec3Labels::LabelType labelType)
    : KeywordBase(typeid(this)), data_(data), parentNode_(parentNode), labelType_(labelType)
{
}

/*
 * Data
 */

// Return reference to data
const Vec3<NodeValue> &Vec3NodeValueKeyword::data() const { return data_; }

// Return label type to display in GUI
Vec3Labels::LabelType Vec3NodeValueKeyword::labelType() const { return labelType_; }

// Set the value from supplied expression text
bool Vec3NodeValueKeyword::setData(int index, std::string_view expressionText)
{
    assert(index >= 0 && index < 3);
    assert(parentNode_);

    return data_[index].set(expressionText, parentNode_->parametersInScope());
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int Vec3NodeValueKeyword::minArguments() const { return 3; }

// Return maximum number of arguments accepted
std::optional<int> Vec3NodeValueKeyword::maxArguments() const { return 3; }

// Deserialise from supplied LineParser, starting at given argument offset
bool Vec3NodeValueKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (!parentNode_)
        return Messenger::error("Can't read keyword {} since the parent ProcedureNode has not been set.\n", name());

    if (parser.hasArg(startArg + 2))
    {
        // Get any variables currently in scope
        auto vars = parentNode_->parametersInScope();

        if (!data_.x.set(parser.argsv(startArg), vars))
            return false;
        if (!data_.y.set(parser.argsv(startArg + 1), vars))
            return false;
        if (!data_.z.set(parser.argsv(startArg + 2), vars))
            return false;

        return true;
    }

    return false;
}

// Serialise data to specified LineParser
bool Vec3NodeValueKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  {}  {}  {}\n", prefix, keywordName, data_.x.asString(true), data_.y.asString(true),
                             data_.z.asString(true));
}
