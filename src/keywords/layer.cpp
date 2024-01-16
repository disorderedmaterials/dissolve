// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/layer.h"
#include "base/lineParser.h"
#include "module/layer.h"

LayerKeyword::LayerKeyword(const ModuleLayer *&data) : KeywordBase(typeid(this)), data_(data) {}

/*
 * Data
 */

// Return reference to data
const ModuleLayer *&LayerKeyword::data() { return data_; }
const ModuleLayer *&LayerKeyword::data() const { return data_; }

// Set data
void LayerKeyword::setData(ModuleLayer *data) { data_ = data; }

/*
 * Arguments
 */

// Deserialise from supplied LineParser, starting at given argument offset
bool LayerKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    data_ = coreData.findProcessingLayer(parser.argsv(startArg));
    if (!data_)
        return Messenger::error("Layer '{}' given to keyword {} doesn't exist.\n", parser.argsv(startArg), KeywordBase::name());
    return true;
}

// Serialise data to specified LineParser
bool LayerKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    if (!data_)
        return true;

    return parser.writeLineF("{}{}  '{}'\n", prefix, keywordName, data_->name());
}

// Read values from a serialisable value
void LayerKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    data_ = coreData.findProcessingLayer(std::string(node.as_string()));
    if (!data_)
        throw toml::type_error(
            fmt::format("Layer '{}' given to keyword {} doesn't exist.\n", std::string(node.as_string()), KeywordBase::name()),
            node.location());
}

// Express as a serialisable value
SerialisedValue LayerKeyword::serialise() const
{
    if (data_)
        return data_->name();
    return {};
}
