// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/serialiser.h"
#include "nodes/number.h"
#include "templates/flags.h"
#include <string>
#include <typeindex>
#include <vector>

// Forward Declarations
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
        Required,  /* Indicates that this (input) has no default value and must have a link */
        NoUpdate,  /* Indicates that changing the parameters value does not warrant a node update */
        ClearData, /* Indicates that any local data should be cleared if the parameter is changed */
        Input,     /* Indicates that the parameter is meant to be a sink for data and not a source */
        Output,    /* Indicates that the parameter is meant to be a source of data and not a sink */
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
    // The owner of the parameter
    Node *parent_;

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
    // Flag that an update is required in the parent node
    void setParentUpdateRequired() const;
    // Clear data in the parent node
    void clearDataInParent() const;
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

            // Changing parameters always flags an update as being required, unless the NoUpdate flag is set
            if (!flags_.isSet(NoUpdate))
                setParentUpdateRequired();

            // Setting some parameters forces any local data to be cleared
            if (flags_.isSet(ClearData))
                clearDataInParent();
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

    // Helper templates for handling serialisation

    template <typename V> struct is_ptr_vector : std::false_type
    {
    };

    template <serialisablePointer E> struct is_ptr_vector<std::vector<E>> : std::true_type
    {
    };

    /*
     * I/O
     */
    public:
    // Express as a serialised value
    SerialisedValue serialise() const override
    {
        SerialisedValue result = {};

        // Serialise non-pointer values
        if constexpr (HasEnumOptions<T>)
            result["data"] = getEnumOptions(data_).serialise(data_);
        else if constexpr (std::is_convertible<T, Number>::value)
            result["data"] = data_;
        else if constexpr (std::is_convertible<T, std::string>::value)
            result["data"] = data_;
        else if constexpr (std::is_convertible<T, std::optional<Number>>::value)
        {
            if (data_)
                result["data"] = *data_;
        }
        else if constexpr (serialisablePointer<T>)
            result["data"] = data_->serialise();
        else
            result["data"] = data_;

        return result;
    };
    // Read from a serialised value
    void deserialise(const SerialisedValue &node) override
    {
        if constexpr (std::is_pointer<T>::value)
        {
            data_ = nullptr;
        }
        else if constexpr (is_ptr_vector<T>::value)
            data_.clear();
        else if constexpr (HasEnumOptions<T>)
        {
            T proxy; // Fake T value to get the corret overload
            data_ = getEnumOptions(proxy).deserialise(node);
        }
        else if constexpr (std::is_convertible<T, std::optional<double>>::value)
        {
            if (node.contains("data"))
                data_ = toml::find<double>(node, "data");
            else
                data_ = {};
        }
        else if constexpr (std::is_convertible<T, std::optional<Number>>::value)
        {
            if (node.contains("data"))
                data_ = toml::find<Number>(node, "data");
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
