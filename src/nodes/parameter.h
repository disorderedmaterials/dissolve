// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/serialiser.h"
#include "math/function1D.h"
#include "nodes/number.h"
#include "templates/flags.h"
#include <string>
#include <typeindex>
#include <vector>

// Forward Declarations
class Node;
class ParameterBase;
template <typename T> class Parameter;
template <typename T> class PointerParameter;
template <typename T> class OptionalPointerParameter;

// Template functions to determine if a given derives from a specific base
template <class T, template <class...> class U> inline constexpr bool is_instance_of_v = std::false_type{};
template <template <class...> class U, class... Vs> inline constexpr bool is_instance_of_v<U<Vs...>, U> = std::true_type{};

// Parameter Proxy
template <class T> class ParameterProxy
{
    public:
    T data;
};

// Parameter Link Data
struct ParameterLink
{
    // Input parameter side
    std::shared_ptr<ParameterBase> inputParameter;
    // Output parameter side
    std::shared_ptr<ParameterBase> outputParameter;
};

// Base type for all parameter templates to inherit from
class ParameterBase : public Serialisable<>
{
    public:
    ParameterBase(Node *parent, std::string_view name, std::string_view description, std::type_index storedDataType);

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
    // The owner of the parameter
    Node *parent_;
    // Name of the parameter
    std::string name_;
    // Description of parameter (used as tooltip in the GUI)
    std::string description_;
    // Stored data type in the parameter
    std::type_index storedDataType_;
    // Flags for the parameter
    Flags<ParameterBase::ParameterFlags> flags_;

    public:
    // Set node parent
    void setParent(Node *parent);
    // Return the parameter name
    std::string_view name() const;
    // Return the parameter description
    std::string_view description() const;
    // Return the stored data type
    std::type_index storedDataType() const;
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
    virtual bool isDefault() const { return true; };
    // Flag that an update is required in the parent node
    void setParentUpdateRequired() const;
    // Clear data in the parent node
    void clearDataInParent() const;
    // Assign the value of another parameter to this one
    virtual bool assign(ParameterBase *other) = 0;
    // Return whether this parameter accepts the output type of the other
    virtual bool acceptsOutput(ParameterBase *other) = 0;
    // Access the full parameter from the base
    template <typename DataClass> std::shared_ptr<Parameter<DataClass>> upcast()
    {
        if (std::type_index(typeid(DataClass)) != storedDataType_)
            return nullptr;
        auto cast1 = static_cast<Parameter<DataClass> *>(this);
        return cast1->shared_from_this();
    }
    // Get the parameter's value
    template <typename DataClass> DataClass get()
    {
        // Requested DataClass must always match the storedDataType_, regardless of the underlying parameter type
        if (std::type_index(typeid(DataClass)) != storedDataType_)
            throw(std::runtime_error(std::format("ParameterBase::get() called with wrong type ({} vs {}), name = {}\n",
                                                 std::type_index(typeid(DataClass)).name(), storedDataType_.name(), name_)));

        // Upcast to Parameter<T> (common base of all parameter types)
        auto cast = dynamic_cast<Parameter<DataClass> *>(this);
        if (!cast)
            throw(std::runtime_error(std::format("ParameterBase::get() failed to cast, name = {}.\n", name_)));
        return cast->getData();
    }
    // Set the parameter's value
    template <typename DataClass> void set(const DataClass &data)
    {
        // Requested DataClass must always match the storedDataType_, regardless of the underlying parameter type
        if (std::type_index(typeid(DataClass)) != storedDataType_)
            throw(std::runtime_error(std::format("ParameterBase::set() called with wrong type ({} vs {}), name = {}\n",
                                                 std::type_index(typeid(DataClass)).name(), storedDataType_.name(), name_)));

        // Upcast to Parameter<T> (common base of all parameter types)
        auto cast = dynamic_cast<Parameter<DataClass> *>(this);
        if (!cast)
            throw(std::runtime_error(std::format("ParameterBase::set() failed to cast, name = {}.\n", name_)));
        cast->setData(data);
    }
    // Create a parameter link (input - data proxy - output) for the derived class type
    virtual ParameterLink createParameterLink(std::string_view newName, std::string_view newDescription = "") const = 0;

    /*
     * I/O
     */
    public:
    // Express as a serialised value
    virtual SerialisedValue serialise() const override { return {}; }
    // Read from a serialised value
    virtual void deserialise(const SerialisedValue &node) override { return; }
};

namespace ParameterFactory
{
template <typename DataClass>
std::shared_ptr<ParameterBase> create(Node *parent, std::string_view name, std::string_view description, DataClass &value)
{
    return std::make_shared<Parameter<DataClass>>(parent, name, description, value);
}
template <typename DataClass>
std::shared_ptr<ParameterBase> createPointer(Node *parent, std::string_view name, std::string_view description,
                                             DataClass &fromObject)
{
    return std::make_shared<Parameter<DataClass *>>(parent, name, description, fromObject);
}
}; // namespace ParameterFactory

// Primary type for a Parameter to a specific DataClass
template <typename DataClass> class Parameter : public ParameterBase, public std::enable_shared_from_this<Parameter<DataClass>>
{
    public:
    Parameter(Node *parent, std::string_view name, std::string_view description, DataClass &value)
        requires(is_instance_of_v<DataClass, std::vector>)
        : ParameterBase(parent, name, description, std::type_index(typeid(DataClass))), data_(value), default_(value)
    {
    }
    Parameter(Node *parent, std::string_view name, std::string_view description, std::remove_pointer_t<DataClass> &value)
        requires(std::is_pointer_v<DataClass>)
        : ParameterBase(parent, name, description, std::type_index(typeid(DataClass))), data_(localPointer_), default_(nullptr),
          dataGetter_([&]() { return &value; }), dataSetter_([](const DataClass &value) { return false; })
    {
    }
    Parameter(Node *parent, std::string_view name, std::string_view description, DataClass &value,
              std::optional<std::remove_pointer_t<DataClass>> &targetData)
        requires(std::is_pointer_v<DataClass>)
        : ParameterBase(parent, name, description, std::type_index(typeid(DataClass))), data_(localPointer_), default_(nullptr),
          dataGetter_([&]() { return targetData.has_value() ? &targetData.value() : nullptr; }),
          dataSetter_([](const DataClass &value) { return false; })
    {
        data_ = &value;
    }
    Parameter(Node *parent, std::string_view name, std::string_view description, DataClass &value)
        : ParameterBase(parent, name, description, std::type_index(typeid(DataClass))), data_(value), default_(value)
    {
    }
    Parameter(Node *parent, std::string_view name, std::string_view description,
              std::shared_ptr<ParameterProxy<DataClass>> &proxy)
        : ParameterBase(parent, name, description, std::type_index(typeid(DataClass))), data_(proxy->data),
          default_(proxy->data)
    {
        // Store the proxy data smart pointer to preserve the lifetime of the data
        proxyData_ = proxy;
    }
    virtual ~Parameter() = default;

    /*
     * Data
     */
    protected:
    // Reference to target data
    DataClass &data_;
    // Specialised container for local pointer referencing, if relevant
    std::conditional_t<std::is_pointer_v<DataClass>, DataClass, bool> localPointer_;
    // Getter for target data, defaulting so simple return of data_ reference member
    using DataGetter = std::function<DataClass()>;
    DataGetter dataGetter_{[&]() { return data_; }};
    // Setter for target data, defaulting to simple 1-to-1 copy as long as equality fails
    using DataSetter = std::function<bool(const DataClass &value)>;
    DataSetter dataSetter_{[&](const DataClass &value)
                           {
                               if (data_ != value)
                               {
                                   data_ = value;
                                   return true;
                               }
                               else
                                   return false;
                           }};
    // Initial value
    const DataClass default_;
    // Parameter proxy data (if a ParameterLink)
    std::shared_ptr<ParameterProxy<DataClass>> proxyData_;

    private:
    // Perform any updates after a successful setData()
    void updateAfterSet()
    {
        // Changing parameters always flags an update as being required, unless the NoUpdate flag is set
        if (!flags_.isSet(NoUpdate))
            setParentUpdateRequired();

        // Setting some parameters forces any local data to be cleared
        if (flags_.isSet(ClearData))
            clearDataInParent();
    }

    public:
    // Set the parameter value
    bool setData(const DataClass &value)
    {
        if (dataSetter_(value))
        {
            updateAfterSet();
            return true;
        }
        else
            return false;
    }
    // Return the parameter value
    virtual DataClass getData() { return dataGetter_(); }
    // Return whether the contained data represents the default value
    bool isDefault() const override { return data_ == default_; }
    // Assign the value of another parameter to this one.
    bool assign(ParameterBase *other) override
    {
        // If the stored data types are the same then we can just do a straight assignment
        if (storedDataType_ == other->storedDataType())
            return setData(other->get<DataClass>());
        else if constexpr (is_instance_of_v<DataClass, std::vector>)
        {
            // If we represent a std::vector container we can conditionally check for a single data item being passed
            if (std::type_index(typeid(typename DataClass::value_type)) == other->storedDataType())
            {
                data_.push_back(other->get<typename DataClass::value_type>());

                updateAfterSet();
            }
            else
                return false;
        }

        // If we are a pointer type, getting a nullptr is disallowed
        if constexpr (std::is_pointer<DataClass>())
        {
            if (data_ == nullptr)
                return false;
        }

        return true;
    }
    // Return whether this parameter accepts the output type of the other
    bool acceptsOutput(ParameterBase *other) override
    {
        if (storedDataType_ == other->storedDataType())
            return true;
        else if constexpr (is_instance_of_v<DataClass, std::vector>)
        {
            // If we represent a std::vector container we can accept a single data item
            if (std::type_index(typeid(typename DataClass::value_type)) == other->storedDataType())
                return true;
        }

        return false;
    }
    // Create a parameter link (input - data proxy - output) for this parameter type
    ParameterLink createParameterLink(std::string_view newName, std::string_view newDescription) const override
    {
        // Create a parameter holder object with the same type as ours and add it to the proxies_ storage
        auto proxy = std::make_shared<ParameterProxy<DataClass>>();

        // Create an input and an output Parameter linked to the proxy data
        auto inputParameter = std::make_shared<Parameter<DataClass>>(nullptr, newName, newDescription, proxy);
        inputParameter->setFlags(ParameterBase::ParameterFlags::Input);

        // Create a companion input on our Outputs node, again linked to the proxy data
        auto outputParameter = std::make_shared<Parameter<DataClass>>(nullptr, newName, newDescription, proxy);
        outputParameter->setFlags(ParameterBase::ParameterFlags::Output);

        return {inputParameter, outputParameter};
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
        if constexpr (HasEnumOptions<DataClass>)
            result["data"] = getEnumOptions(data_).serialise(data_);
        else if constexpr (std::is_convertible<DataClass, Number>::value)
            result["data"] = data_;
        else if constexpr (std::is_convertible<DataClass, std::string>::value)
            result["data"] = data_;
        else if constexpr (std::is_convertible<DataClass, std::optional<Number>>::value)
        {
            if (data_)
                result["data"] = *data_;
        }
        else if constexpr (serialisablePointer<DataClass>)
            result["data"] = data_->serialise();
        else
            result["data"] = data_;

        return result;
    };
    // Read from a serialised value
    void deserialise(const SerialisedValue &node) override
    {
        if constexpr (std::is_pointer<DataClass>::value)
        {
            data_ = nullptr;
        }
        else if constexpr (is_ptr_vector<DataClass>::value)
            data_.clear();
        else if constexpr (HasEnumOptions<DataClass>)
        {
            DataClass proxy; // Fake T value to get the correct overload
            data_ = getEnumOptions(proxy).deserialise(node);
        }
        else if constexpr (std::is_convertible<DataClass, std::optional<double>>::value)
        {
            if (node.contains("data"))
                data_ = toml::find<double>(node, "data");
            else
                data_ = {};
        }
        else if constexpr (std::is_convertible<DataClass, std::optional<Number>>::value)
        {
            if (node.contains("data"))
                data_ = toml::find<Number>(node, "data");
            else
                data_ = {};
        }
        else
        {
            data_ = toml::find<DataClass>(node, "data");
        }
    }
};

// OptionalPointerParameter, returning a pointer from a target object rather than the object itself
template <typename ClassPtr> class OptionalPointerParameter : public Parameter<ClassPtr>
{
    public:
    OptionalPointerParameter(Node *parent, std::string_view name, std::string_view description,
                             std::optional<std::remove_pointer_t<ClassPtr>> &object)
        : Parameter<ClassPtr>(parent, name, description, pointer_), object_(object), pointer_{nullptr}
    {
    }
    ~OptionalPointerParameter() override = default;

    /*
     * Data
     */
    protected:
    // Reference to optional object
    std::optional<std::remove_pointer_t<ClassPtr>> &object_;
    // Pointer object
    ClassPtr pointer_;

    public:
    // Return the parameter value
    ClassPtr getData() override { return object_.has_value() ? &object_.value() : nullptr; }
};

// Template specialisation for non-defaulted type Function1DWrapper
template <>
class Parameter<Function1DWrapper> : public ParameterBase, public std::enable_shared_from_this<Parameter<Function1DWrapper>>
{
    public:
    Parameter(Node *parent, std::string_view name, std::string_view description, Function1DWrapper &value)
        : ParameterBase(parent, name, description, std::type_index(typeid(Function1DWrapper))), data_(value), default_(value)
    {
    }

    /*
     * Data
     */
    protected:
    // Reference to target data
    Function1DWrapper &data_;
    // Initial value
    const Function1DWrapper default_;

    public:
    // Assign the value of another parameter to this one
    bool assign(ParameterBase *other) override { return false; }
    // Return whether this parameter accepts the output type of the other
    bool acceptsOutput(ParameterBase *other) override { return false; }
    // Return whether the contained data represents the default value
    bool isDefault() const override { return false; }
    // Create a parameter link (input - data proxy - output) for the derived class type
    ParameterLink createParameterLink(std::string_view newName, std::string_view newDescription = "") const override
    {
        throw(std::runtime_error("Can't create a ParameterLink for a Function1DWrapper.\n"));
    }
};
