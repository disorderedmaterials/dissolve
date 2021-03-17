// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "expression/variable.h"
#include "base/messenger.h"
#include <string.h>

ExpressionVariable::ExpressionVariable(ExpressionValue value)
{
    // Private variables
    static int count = 0;
    name_ = fmt::format("_ExpressionVariable{:02d}", count++);
    value_ = value;
}

ExpressionVariable::ExpressionVariable(std::string_view name, ExpressionValue value) : name_(name), value_(value) {}

// Set name of variable
void ExpressionVariable::setName(std::string_view s) { name_ = s; }

// Get name of variable
std::string_view ExpressionVariable::name() const { return name_; }

// Set value
void ExpressionVariable::setValue(ExpressionValue value) { value_ = value; }

// Return value
const ExpressionValue &ExpressionVariable::value() const { return value_; }

// Return pointer to value
ExpressionValue *ExpressionVariable::valuePointer() { return &value_; }
