// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

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
void LayerKeyword::setData(ModuleLayer *data)
{
    data_ = data;
}

/*
 * Arguments
 */

// Deserialise from supplied LineParser, starting at given argument offset
bool LayerKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    data_ = coreData.findProcessingLayer(parser.argsv(startArg));
    if (!data_)
        return Messenger::error("Error setting Layer - no Layer named '{}' exists.\n", parser.argsv(startArg));
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
   data_ = coreData.findProcessingLayer(std::string_view(std::string(node.as_string()))); 
}


// Express as a serialisable value
SerialisedValue LayerKeyword::serialise() const { return data_->name(); }

