// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/flags.h"
#include <fmt/core.h>
#include <string>
#include <variant>

namespace DataModel
{
// Property for an Item within a DataModel
class ItemProperty
{
    public:
    // Data property types
    enum class PropertyType
    {
        Integer,
        Double,
        String
    };

    // Data property flags
    enum PropertyFlag
    {
        ReadOnly,
        Disabled
    };
    ItemProperty(std::string_view name, PropertyType type, Flags<PropertyFlag> flags = {})
        : name_{name}, type_(type), flags_(flags)
    {
    }

    private:
    // Property name
    std::string name_;
    // Property type
    PropertyType type_;
    // Property flags
    Flags<PropertyFlag> flags_;

    public:
    // Return property name
    const std::string &name() const { return name_; }
    // Return property type
    PropertyType type() const { return type_; }
    // Return property flags
    const Flags<PropertyFlag> &flags() const { return flags_; }
};

// Property value variant
using PropertyValue = std::variant<int, double, std::string_view, std::string>;
// Return value as string
static std::string propertyAsString(const PropertyValue &value)
{
    if (std::holds_alternative<int>(value))
        return fmt::format("{}", std::get<int>(value));
    else if (std::holds_alternative<double>(value))
        return fmt::format("{}", std::get<double>(value));
    else if (std::holds_alternative<std::string_view>(value))
        return fmt::format("{}", std::get<std::string_view>(value));
    else
        return std::get<std::string>(value);
}

// Helper type for PropertyValue visitor
template <class... Ts> struct PropertyVisitor : Ts...
{
    using Ts::operator()...;
};
// Explicit deduction guide for PropertyValue visitor
template <class... Ts> PropertyVisitor(Ts...) -> PropertyVisitor<Ts...>;

// Modelable - Provides declarations for use by a class defining its properties
template <class D> class Modelable
{
    public:
    using ModelableGetter = std::function<PropertyValue(const D *)>;
    using ModelableSetter = std::function<bool(D *, PropertyValue)>;
    using ModelableProperty = std::tuple<std::string, ItemProperty::PropertyType, Flags<ItemProperty::PropertyFlag>,
                                         ModelableGetter, ModelableSetter>;

    /*
     * We now define a static templated function to return a vector of ModelableProperty definitions for the target class D. The
     * class D needs to provide this function as a templated function specialisation.
     */
    public:
    // Return basic property information including getter and setter (if relevant)
    static const std::vector<ModelableProperty> modelableProperties();
};
}; // namespace DataModel
