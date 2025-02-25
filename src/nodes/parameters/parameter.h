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

        // Serialise non-pointer values
        if constexpr (!std::is_pointer<T>::value)
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

// A concept to capture numerical types
template <class T>
concept Numeric = std::integral<T> || std::floating_point<T>;

// Parameters which are bounded and might sit in a gui spinbox
template <Numeric T> class BoundedParameter : public Parameter<T>
{
    public:
    BoundedParameter(std::string_view name, std::string_view description, T &value, T defValue, std::optional<T> lower = {},
                     std::optional<T> upper = {}, std::optional<T> step = {})
        : Parameter<T>(name, description, value, defValue), lower_(lower), upper_(upper), step_(step)
    {
    }

    private:
    std::optional<T> lower_, upper_, step_;

    public:
    std::optional<T> getLower() { return lower_; }
    std::optional<T> getUpper() { return upper_; }
    std::optional<T> getStep() { return step_; }
    void setLower(std::optional<T> value) { lower_ = value; }
    void setUpper(std::optional<T> value) { upper_ = value; }
    void setStep(std::optional<T> value) { step_ = value; }
    // Increase the parameter value
    void increment()
    {
        if (step_)
            this->data_ += *step_;
        if (this->data_ > *upper_)
            this->data_ = *upper_;
    }
    // Decrease the parameter value
    void decrement()
    {
        if (step_)
            this->data_ -= *step_;
        if (this->data_ < *lower_)
            this->data_ = *lower_;
    }
};
