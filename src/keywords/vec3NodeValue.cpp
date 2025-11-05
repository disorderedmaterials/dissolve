// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "keywords/vec3NodeValue.h"
#include "base/lineParser.h"
#include "generator/node.h"

Vec3NodeValueKeyword::Vec3NodeValueKeyword(Vector3NodeValue &data, GeneratorNode *parentNode, Vec3Labels::LabelType labelType)
    : KeywordBase(typeid(this)), data_(data), parentNode_(parentNode), labelType_(labelType)
{
}

/*
 * Data
 */

// Return reference to data
const Vector3NodeValue &Vec3NodeValueKeyword::data() const { return data_; }

// Set data
bool Vec3NodeValueKeyword::setData(const Vector3NodeValue &v)
{
    data_ = v;
    return true;
}

// Set the value from supplied expression text
bool Vec3NodeValueKeyword::setData(int index, std::string_view expressionText)
{
    assert(index >= 0 && index < 3);
    assert(parentNode_);

    switch (index)
    {
        case (0):
            return data_.x.set(expressionText, parentNode_->getParametersInScope());
        case (1):
            return data_.y.set(expressionText, parentNode_->getParametersInScope());
        case (2):
            return data_.z.set(expressionText, parentNode_->getParametersInScope());
        default:
            Messenger::exception("Tried to retrieve a bad index in Vec3NodeValueKeyword.\n");
    }
}

// Return label type to display in GUI
Vec3Labels::LabelType Vec3NodeValueKeyword::labelType() const { return labelType_; }

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
        return Messenger::error("Can't read keyword {} since the parent GeneratorNode has not been set.\n", name());

    if (parser.hasArg(startArg + 2))
    {
        // Get any variables currently in scope
        auto vars = parentNode_->getParametersInScope();

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

// Express as a serialisable value
void Vec3NodeValueKeyword::serialise(std::string tag, SerialisedValue &target) const { target[tag] = data_; }

// Read values from a serialisable value
void Vec3NodeValueKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    data_.deserialise(node, parentNode_->getParametersInScope());
}

// Has not changed from initial value
bool Vec3NodeValueKeyword::isDefault() const { return data_.x == default_.x && data_.y == default_.y && data_.z == default_.z; }
