// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include <string>
#include <typeindex>
#include <vector>

template <typename T> class Parameter;

// Base type for all parameter templates to inherit from
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

    // Access the full parameter from the base
    template <typename T> std::shared_ptr<Parameter<T>> upcast()
    {
        if (std::type_index(typeid(T)) != type_)
            return nullptr;
        auto casted = static_cast<Parameter<T> *>(this);
        return casted->shared_from_this();
    }

    /*
     * I/O
     */
    public:
    // Express as a serialised value
    virtual SerialisedValue serialise() const = 0;
    // Read from a serialised value
    virtual void deserialise(const SerialisedValue &node) = 0;
};

// Primary type for a Parameter to a value of type T
template <typename T> class Parameter : public ParameterBase, public std::enable_shared_from_this<Parameter<T>>
{
    public:
    Parameter(std::string_view name, std::string_view description, T &value)
        : ParameterBase(name, description, std::type_index(typeid(T))), data_(value), default_(value)
    {
    }

    /*
     * Data
     */
    protected:
    // Reference to target data
    T &data_;
    // Initial value
    const T default_;

    public:
    // Set the parameter value
    virtual void set(const T &value) { data_ = value; }
    // Return the parameter value
    T &get() { return data_; }
    const T &get() const { return data_; }
    // Return whether the contained data represents the default value
    bool isDefault() const override { return data_ == default_; }

    /*
     * I/O
     */
    public:
    // Express as a serialised value
    SerialisedValue serialise() const override
    {
        SerialisedValue result = {};
        result["name"] = name_;
        result["description"] = description_;

        // Serialise non-pointer values
        if constexpr (std::is_convertible<T, double>::value)
            result["data"] = data_;
        else if constexpr (std::is_convertible<T, std::string>::value)
            result["data"] = data_;
        else if constexpr (std::is_convertible<T, std::optional<double>>::value)
        {
            if (data_)
                result["data"] = *data_;
        }
        return result;
    };
    // Read from a serialised value
    void deserialise(const SerialisedValue &node) override
    {
        name_ = toml::find<std::string>(node, "name");
        description_ = toml::find<std::string>(node, "description");
        if constexpr (std::is_pointer<T>::value)
        {
            data_ = nullptr;
        }
        else if constexpr (std::is_convertible<T, std::optional<double>>::value)
        {
            if (node.contains("data"))
                data_ = toml::find<double>(node, "data");
            else
                data_ = {};
        }
        else
        {
            data_ = toml::find<T>(node, "data");
        }
    }
};

// Parameters which are bounded and might sit in a gui spinbox
template <typename T> class BoundedParameter : public Parameter<T>
{
    public:
    BoundedParameter(std::string_view name, std::string_view description, T &value, std::optional<T> lower = {},
                     std::optional<T> upper = {}, std::optional<T> step = {})
        : Parameter<T>(name, description, value), lower_(lower), upper_(upper), step_(step)
    {
    }

    /*
     * Data
     */
    private:
    // Bounds to apply
    std::optional<T> lower_, upper_;
    // Stepsize for UI controls
    std::optional<T> step_;

    public:
    // Set the parameter value
    void set(const T &value) override
    {
        if (lower_ && value < *lower_)
            data_ = *lower_;
        else if (upper_ && value > upper_)
            data_ = *upper_;
    }
    // Return lower bound
    std::optional<T> lowerBound() { return lower_; }
    // Return upper bound
    std::optional<T> upperBound() { return upper_; }
    // Return step size
    std::optional<T> stepSize() { return step_; }
};
