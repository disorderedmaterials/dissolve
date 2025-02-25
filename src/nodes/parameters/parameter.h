// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include <string>
#include <typeindex>
#include <vector>

class ParameterBase
{
    public:
    ParameterBase(std::string_view name, std::string_view description, std::type_index type)
        : name_(name), description_(description), type_(type)
    {
    }

    protected:
    // Name of the parameter
    std::string_view name_;
    // Description of parameter (used as tooltip in the GUI)
    std::string_view description_;
    // Type of the parameter
    std::type_index type_;

    /*
     * Data
     */
    public:
    // Return whether the contained data represents the default value
    virtual bool isDefault() const = 0;

    /*
     * I/O
     */
    public:
    // Express as a serialised value
    virtual SerialisedValue serialise() const = 0;
    // Read from a serialised value
    virtual void deserialise(const SerialisedValue &node) = 0;
};

template <typename T> class Parameter : public ParameterBase
{
    public:
    Parameter(std::string_view name, std::string_view description, T &value, T defValue)
        : ParameterBase(name, description, std::type_index(typeid(T))), data_(value), default_(defValue)
    {
    }

    protected:
    // Reference to target data
    T &data_;
    // Initial value
    const T default_;

    public:
    bool isDefault() const { return data_ == default_; }

    SerialisedValue serialise() const
    {
        SerialisedValue result = {};
        result["name"] = name_;
        result["description"] = description_;
        result["data"] = data_;
        return result;
    };

    void deserialise(const SerialisedValue &node)
    {
        name_ = toml::find<std::string>(node, "name");
        description_ = toml::find<std::string>(node, "description");
        if constexpr (std::is_pointer<T>::value)
        {
            data_ = nullptr;
        }
        else
        {
            data_ = toml::find<T>(node, "data");
        }
    }
};
