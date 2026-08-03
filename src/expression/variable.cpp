// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "expression/variable.h"
#include "base/messenger.h"
#include "base/serialiserLibrary.h"
#include <cstring>

ExpressionVariable::ExpressionVariable(const ExpressionValue &value)
{
    // Private variables
    static int count = 0;
    baseName_ = std::format("_ExpressionVariable{:02d}", count++);
    name_ = std::format("_ExpressionVariable{:02d}", count++);
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
        name_ = std::format("{}{}{}", namePrefix_, separator, baseName_);
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
void ExpressionVariable::serialise(std::string tag, SerialisedValue &target) const
{
    target[tag] = {{"name", Serialisable::ser(baseName_)}, {"value", Serialisable::ser(value_)}};
}

// Read values from a serialisable value
void ExpressionVariable::deserialise(const SerialisedValue &node)
{
    value_ = Deserialisable::deser<ExpressionValue>(node.at("value"));
    setBaseName(Deserialisable::deser<std::string>(node.at("name")));
}
