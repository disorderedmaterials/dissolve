// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "expression/value.h"

// Variable
class ExpressionVariable : public Serialisable<>
{
    public:
    ExpressionVariable(const ExpressionValue &value = ExpressionValue());
    ExpressionVariable(std::string_view name, const ExpressionValue &value = ExpressionValue());
    ~ExpressionVariable() = default;

    /*
     * Variable Data
     */
    protected:
    // Name of the variable
    std::string name_;
    // Value of variable
    ExpressionValue value_;

    public:
    // Set name of variable
    void setName(std::string_view s);
    // Get name of variable
    std::string_view name() const;
    // Set value
    void setValue(const ExpressionValue &value);
    // Return value
    const ExpressionValue &value() const;
    // Return pointer to value
    ExpressionValue *valuePointer();
    SerialisedValue serialise() const override;
    void deserialise(const SerialisedValue &node);
};
