// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "expression/variable.h"
#include "base/messenger.h"
#include <cstring>

ExpressionVariable::ExpressionVariable(const ExpressionValue &value)
{
    // Private variables
    static int count = 0;
    baseName_ = fmt::format("_ExpressionVariable{:02d}", count++);
    name_ = fmt::format("_ExpressionVariable{:02d}", count++);
    value_ = value;
}

ExpressionVariable::ExpressionVariable(std::string_view name, const ExpressionValue &value)
    : baseName_(name), name_(name), value_(value)
{
}

// Update full name
void ExpressionVariable::updateName()
{
    constexpr std::string_view separator = ".";
    if (namePrefix_.empty())
        name_ = baseName_;
    else
        name_ = fmt::format("{}{}{}", namePrefix_, separator, baseName_);
}

// Set name of variable
void ExpressionVariable::setBaseName(std::string_view s)
{
    baseName_ = s;
    updateName();
}

// Return base name of the variable
std::string_view ExpressionVariable::baseName() const { return baseName_; }

// Set prefix to prepend to the base name
void ExpressionVariable::setNamePrefix(std::string_view s)
{
    namePrefix_ = s;
    updateName();
}

// Get full name of variable
std::string_view ExpressionVariable::name() const { return name_; }

// Set value
void ExpressionVariable::setValue(const ExpressionValue &value) { value_ = value; }

// Return value
const ExpressionValue &ExpressionVariable::value() const { return value_; }

// Return pointer to value
ExpressionValue *ExpressionVariable::valuePointer() { return &value_; }

// Express as a serialisable value
SerialisedValue ExpressionVariable::serialise() const { return {{"name", baseName_}, {"value", value_}}; }

// Read values from a serialisable value
void ExpressionVariable::deserialise(const SerialisedValue &node)
{
    value_ = toml::find<ExpressionValue>(node, "value");
    setBaseName(toml::find<std::string>(node, "name"));
}
