// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/vec3nodevalue.h"
#include "base/lineparser.h"
#include "procedure/nodes/node.h"

Vec3NodeValueKeyword::Vec3NodeValueKeyword(ProcedureNode *parentNode, Vec3<double> value, Vec3Labels::LabelType labelType)
    : KeywordData<Vec3<NodeValue>>(KeywordBase::Vec3NodeValueData, Vec3<NodeValue>(value.x, value.y, value.z))
{
    parentNode_ = parentNode;
    labelType_ = labelType;
}

Vec3NodeValueKeyword::~Vec3NodeValueKeyword() {}

/*
 * Label Type
 */

// Label type to display in GUI
Vec3Labels::LabelType Vec3NodeValueKeyword::labelType() const { return labelType_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int Vec3NodeValueKeyword::minArguments() const { return 3; }

// Return maximum number of arguments accepted
int Vec3NodeValueKeyword::maxArguments() const { return 3; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool Vec3NodeValueKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
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

        hasBeenSet();

        return true;
    }

    return false;
}

// Write keyword data to specified LineParser
bool Vec3NodeValueKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  {}  {}  {}\n", prefix, keywordName, data_.x.asString(true), data_.y.asString(true),
                             data_.z.asString(true));
}

/*
 * Set
 */

// Set the value from supplied expression text
bool Vec3NodeValueKeyword::setValue(int index, std::string_view expressionText)
{
    assert(index >= 0 && index < 3);

    // Get any variables currently in scope
    auto vars = parentNode_->parametersInScope();

    if (!data_[index].set(expressionText, vars))
        return false;

    set_ = true;

    return true;
}

/*
 * Conversion
 */

// Return value (as Vec3<int>)
Vec3<int> Vec3NodeValueKeyword::asVec3Int() { return Vec3<int>(data_.x.asInteger(), data_.y.asInteger(), data_.z.asInteger()); }

// Return value (as Vec3<NodeValue>)
Vec3<double> Vec3NodeValueKeyword::asVec3Double()
{
    return Vec3<double>(data_.x.asDouble(), data_.y.asDouble(), data_.z.asDouble());
}
