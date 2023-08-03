// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "keywords/task.h"
#include "base/lineParser.h"
#include "module/layer.h"

TaskKeyword::TaskKeyword(const Task *&data) : KeywordBase(typeid(this)), data_(data) {}

/*
 * Data
 */

// Return reference to data
const Task *&TaskKeyword::data() { return data_; }
const Task *&TaskKeyword::data() const { return data_; }

// Set data
void TaskKeyword::setData(Task *data) { data_ = data; }

/*
 * Arguments
 */

// Deserialise from supplied LineParser, starting at given argument offset
bool TaskKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    return true;
}

// Serialise data to specified LineParser
bool TaskKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
   return true;
}

// Read values from a serialisable value
void TaskKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
   return;
}

SerialisedValue TaskKeyword::serialise() const
{
   return {};
}