// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "expression/value.h"

// Variable
class ExpressionVariable : public Serialisable<>
{
    public:
    ExpressionVariable(const ExpressionValue &value = ExpressionValue(0));
    ExpressionVariable(std::string_view baseName, const ExpressionValue &value = ExpressionValue(0));
    ~ExpressionVariable() = default;

    /*
     * Variable Data
     */
    private:
    // Update full name
    void updateName();

    protected:
    // Base name of the variable (without any prefix)
    std::string baseName_;
    // Optional prefix to prepend to the base name in order to get the full variable name
    std::string namePrefix_;
    // Full name of the variable
    std::string name_;
    // Value of variable
    ExpressionValue value_;

    public:
    // Set base name of the variable
    void setBaseName(std::string_view s);
    // Return base name of the variable
    std::string_view baseName() const;
    // Set prefix to prepend to the base name
    void setNamePrefix(std::string_view s);
    // Get full name of variable
    std::string_view name() const;
    // Set value
    void setValue(const ExpressionValue &value);
    // Return value
    const ExpressionValue &value() const;
    // Return pointer to value
    ExpressionValue *valuePointer();
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node);
};
