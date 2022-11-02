// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "expression/variable.h"
#include "base/messenger.h"
#include <cstring>

ExpressionVariable::ExpressionVariable(const ExpressionValue &value)
{
    // Private variables
    static int count = 0;
    name_ = fmt::format("_ExpressionVariable{:02d}", count++);
    value_ = value;
}

ExpressionVariable::ExpressionVariable(std::string_view name, const ExpressionValue &value) : name_(name), value_(value) {}

// Set name of variable
void ExpressionVariable::setName(std::string_view s) { name_ = s; }

// Get name of variable
std::string_view ExpressionVariable::name() const { return name_; }

// Set value
void ExpressionVariable::setValue(const ExpressionValue &value) { value_ = value; }

// Return value
const ExpressionValue &ExpressionVariable::value() const { return value_; }

// Return pointer to value
ExpressionValue *ExpressionVariable::valuePointer() { return &value_; }

// Express as a tree node
SerialisedValue ExpressionVariable::serialise() const { return {{"name", name_}, {"value", value_}}; }

// Read values from a tree node
void ExpressionVariable::deserialise(const SerialisedValue &node)
{
    value_ = toml::find<ExpressionValue>(node, "value");
    setName(toml::find<std::string>(node, "name"));
}
