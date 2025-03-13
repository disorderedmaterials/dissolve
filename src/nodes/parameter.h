// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "templates/flags.h"
#include <string>
#include <typeindex>
#include <vector>

class Node;

template <typename T> class Parameter;

// Base type for all parameter templates to inherit from
class ParameterBase : public Serialisable<>
{
    public:
    ParameterBase(Node *parent, std::string_view name, std::string_view description, std::type_index type);
    // Parameter Flags
    enum ParameterFlags
    {
        Invalidates, /* Indicates that the node's data is invalidated if the parameter is changed */
        Output,      /* Indicates that the parameter is meant to be a source of data and not a sink */
    };

    /*
     * Definition
     */
    protected:
    // Name of the parameter
    std::string_view name_;
    // Description of parameter (used as tooltip in the GUI)
    std::string_view description_;
    // Type of the parameter
    std::type_index type_;
    // Flags for the parameter
    Flags<ParameterBase::ParameterFlags> flags_;
    // The owner the parameter
    Node *parent_;
    // Tell the owner to invalidate
    void invalidate() const;

    public:
    // Return the parameter name
    std::string_view name() const;
    // Return the parameter description
    std::string_view description() const;
    // Return the parameter type
    std::type_index type() const;
    // Return the owner of the parameter
    Node *parent() const;
    // Set flag(s) for the parameter
    void setFlags(const Flags<ParameterBase::ParameterFlags> &flags);
    // Return current flags
    const Flags<ParameterBase::ParameterFlags> &flags() const;

    /*
     * Data
     */
    public:
    // Return whether the contained data represents the default value
    virtual bool isDefault() const = 0;
    // Ensure that parameters are using the latest values
    bool runUpdate() const;

    // Assign the value of another parameter to this one.
    virtual bool assign(ParameterBase *other) = 0;

    // Access the full parameter from the base
    template <typename T> std::shared_ptr<Parameter<T>> upcast()
    {
        if (std::type_index(typeid(T)) != type_)
            return nullptr;
        auto casted = static_cast<Parameter<T> *>(this);
        return casted->shared_from_this();
    }
};

// Primary type for a Parameter to a value of type T
template <typename T> class Parameter : public ParameterBase, public std::enable_shared_from_this<Parameter<T>>
{
    public:
    Parameter(Node *parent, std::string_view name, std::string_view description, T &value)
        : ParameterBase(parent, name, description, std::type_index(typeid(T))), data_(value), default_(value)
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
    virtual void set(const T &value)
    {
        if (data_ != value)
        {
            data_ = value;
            if (flags_.isSet(Invalidates))
                invalidate();
        }
    }
    // Return the parameter value
    T &get() { return data_; }
    const T &get() const { return data_; }
    // Return whether the contained data represents the default value
    bool isDefault() const override { return data_ == default_; }
    // Assign the value of another parameter to this one.
    bool assign(ParameterBase *other) override
    {
        auto upcasted = other->upcast<T>();
        if (!upcasted)
            return false;
        set(upcasted->get());
        return true;
    }
    /*
     * I/O
     */
    public:
    // Express as a serialised value
    SerialisedValue serialise() const override
    {
        SerialisedValue result = {};

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
    BoundedParameter(Node *parent, std::string_view name, std::string_view description, T &value, std::optional<T> lower = {},
                     std::optional<T> upper = {}, std::optional<T> step = {})
        : Parameter<T>(parent, name, description, value), lower_(lower), upper_(upper), step_(step)
    {
    }

    /*
     * Data
     */
    protected:
    // Bounds to apply
    std::optional<T> lower_, upper_;
    // Stepsize for UI controls
    std::optional<T> step_;

    public:
    // Set the parameter value
    void set(const T &value) override
    {
        if (lower_ && value < *lower_)
            this->data_ = *lower_;
        else if (upper_ && value > upper_)
            this->data_ = *upper_;
    }
    // Return lower bound
    std::optional<T> lowerBound() { return lower_; }
    // Return upper bound
    std::optional<T> upperBound() { return upper_; }
    // Return step size
    std::optional<T> stepSize() { return step_; }
};

// Parameters which are bounded and have a text value when at the lower limit to represent the null state
template <typename T> class BoundedOptionalParameter : public BoundedParameter<T>
{
    public:
    BoundedOptionalParameter(Node *parent, std::string_view name, std::string_view description, T &value, T lower,
                             std::string_view textWhenNull, T upper = {}, T step = {})
        : BoundedParameter<T>(parent, name, description, value, lower, upper, step), textWhenNull_{textWhenNull}
    {
    }

    /*
     * Data
     */
    private:
    // Text to display when at lower limit / no value in optional
    std::string textWhenNull_;

    public:
    // Set the parameter value
    void set(const T &value) override
    {
        if (!value)
            this->data_ = {};
        else if (value < *(this->lower_))
            this->data_ = {};
        else if (this->upper_ && value > *(this->upper_))
            this->data_ = *(this->upper_);
    }
    // Return text to display when null
    std::string_view textWhenNull() const { return textWhenNull_; }
};
