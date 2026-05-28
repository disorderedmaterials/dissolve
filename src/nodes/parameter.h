// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/context.h"
#include "base/enumOptions.h"
#include "base/serialiser.h"
#include "classes/coreData.h"
#include "math/data1D.h"
#include "nodes/number.h"
#include "templates/algorithms.h"
#include "templates/flags.h"
#include <stdexcept>
#include <string>
#include <typeindex>
#include <vector>

// Forward Declarations
class Node;
class ParameterBase;
template <typename T> class Parameter;
template <typename T> class SerialisableParameter;

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
    ParameterBase(Node *parent, std::string_view name, std::string_view description, std::type_index storedDataType,
                  std::type_index contextDataType);
    virtual ~ParameterBase() = default;

    // Parameter Flags
    enum ParameterFlags
    {
        Required,  /* Indicates that this (input) has no default value and must have a link */
        NoUpdate,  /* Indicates that changing the parameters value does not warrant a node update */
        ClearData, /* Indicates that any local data should be cleared if the parameter is changed */
        Input,     /* Indicates that the parameter is meant to be a sink for data and not a source */
        Output,    /* Indicates that the parameter is meant to be a source of data and not a sink */
    };
    // Allowed Edge Count
    enum AllowedEdgeCount
    {
        Zero,     /* No edges are allowed */
        One,      /* Exactly one edge is allowed */
        AnyNumber /* Any number of edges is allowed */
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
    // Stored data type of the context
    std::type_index contextDataType_;
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
    // Remove flag for the parameter
    void removeFlag(ParameterBase::ParameterFlags flag);
    // Return current flags
    const Flags<ParameterBase::ParameterFlags> &flags() const;
    // Return the number of allowed input edges
    virtual AllowedEdgeCount nAllowedInputEdges() const = 0;

    /*
     * Data
     */
    public:
    // Return whether the contained data is an instance of std::vector
    virtual bool isVector() const { return false; }
    // Flag that an update is required in the parent node
    void setParentUpdateRequired() const;
    // Clear data in the parent node
    void clearDataInParent() const;
    // Mark edges for re-pull in parent node
    void markIncomingEdgesForPull() const;
    // Assign the value of another parameter to this one
    virtual bool assign(ParameterBase *other) = 0;
    // Return whether this parameter accepts the output type of the other
    virtual bool acceptsOutput(ParameterBase *other) const = 0;
    // The type's representation as a raw int (only valid for int and enum)
    virtual int getAsInt() const { return -1; }
    // Set type's representation as a raw int (only valid for int and enum)
    virtual void setFromInt(int value) { return; }

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
    // Get the parameter's context
    template <typename DataClass> Context<DataClass>::type context()
    {
        // Requested DataClass must always match the storedDataType_, regardless of the underlying parameter type
        if (std::type_index(typeid(typename Context<DataClass>::type)) != contextDataType_)
            throw(std::runtime_error(std::format("ParameterBase::context() called with wrong type ({} vs {}), name = {}\n",
                                                 std::type_index(typeid(typename Context<DataClass>::type)).name(),
                                                 contextDataType_.name(), name_)));

        // Upcast to Parameter<T> (common base of all parameter types)
        auto cast = dynamic_cast<Parameter<DataClass> *>(this);
        if (!cast)
            throw(std::runtime_error(std::format("ParameterBase::context() failed to cast, name = {}.\n", name_)));
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
    // Invalidate the vector data (instances of std::vector only)
    virtual void invalidateVector() {}
    // Create a parameter link (input - data proxy - output) for the derived class type
    virtual ParameterLink createParameterLink(std::string_view newName, std::string_view newDescription = "") const = 0;

    /*
     * Serialisation
     */
    public:
    // Express as a serialised value
    virtual void serialise(std::string tag, SerialisedValue &target) const override {}
    // Read from a serialised value
    virtual void deserialise(const SerialisedValue &node) override { return; }
};

namespace ParameterFactory
{
template <typename DataClass>
std::shared_ptr<ParameterBase> create(Node *parent, std::string_view name, std::string_view description, DataClass &value,
                                      typename Context<DataClass>::type context = {})
{
    return std::make_shared<Parameter<DataClass>>(parent, name, description, value, context);
}
template <typename DataClass>
std::shared_ptr<ParameterBase> createPointer(Node *parent, std::string_view name, std::string_view description,
                                             DataClass &fromObject, typename Context<DataClass>::type context = {})
{
    return std::make_shared<Parameter<DataClass *>>(parent, name, description, fromObject, context);
}
template <typename DataClass>
std::shared_ptr<ParameterBase> createPointer(Node *parent, std::string_view name, std::string_view description,
                                             std::optional<DataClass> &fromOptional,
                                             typename Context<DataClass>::type context = {})
{
    return std::make_shared<Parameter<DataClass *>>(parent, name, description, fromOptional, context);
}
template <typename DataClass>
std::shared_ptr<ParameterBase> createSerialisable(Node *parent, std::string_view name, std::string_view description,
                                                  DataClass &value)
{
    return std::make_shared<SerialisableParameter<DataClass>>(parent, name, description, value);
}
}; // namespace ParameterFactory

// Primary type for a Parameter to a specific DataClass
template <typename DataClass> class Parameter : public ParameterBase, public std::enable_shared_from_this<Parameter<DataClass>>
{
    public:
    Parameter(Node *parent, std::string_view name, std::string_view description, DataClass &value,
              typename Context<DataClass>::type context)
        requires(is_instance_of_v<DataClass, std::vector>)
        : ParameterBase(parent, name, description, std::type_index(typeid(DataClass)),
                        std::type_index(typeid(typename Context<DataClass>::type))),
          data_(value), default_(value)
    {
    }
    Parameter(Node *parent, std::string_view name, std::string_view description, std::remove_pointer_t<DataClass> &value,
              typename Context<DataClass>::type context)
        requires(std::is_pointer_v<DataClass>)
        : ParameterBase(parent, name, description, std::type_index(typeid(DataClass)),
                        std::type_index(typeid(typename Context<DataClass>::type))),
          data_(localPointer_), default_(nullptr), dataGetter_([&]() { return &value; }),
          dataSetter_([](const DataClass &value) { return false; }), context_(context)
    {
    }
    Parameter(Node *parent, std::string_view name, std::string_view description,
              std::optional<std::remove_pointer_t<DataClass>> &targetData, typename Context<DataClass>::type context)
        requires(std::is_pointer_v<DataClass>)
        : ParameterBase(parent, name, description, std::type_index(typeid(DataClass)),
                        std::type_index(typeid(typename Context<DataClass>::type))),
          data_(localPointer_), default_(nullptr),
          dataGetter_([&]() { return targetData.has_value() ? &targetData.value() : nullptr; }),
          dataSetter_([](const DataClass &value) { return false; }), context_(context)
    {
    }
    Parameter(Node *parent, std::string_view name, std::string_view description, DataClass &value,
              typename Context<DataClass>::type context)
        : ParameterBase(parent, name, description, std::type_index(typeid(DataClass)),
                        std::type_index(typeid(typename Context<DataClass>::type))),
          data_(value), default_(value), context_(context)
    {
    }
    Parameter(Node *parent, std::string_view name, std::string_view description,
              std::shared_ptr<ParameterProxy<DataClass>> &proxy)
        : ParameterBase(parent, name, description, std::type_index(typeid(DataClass)),
                        std::type_index(typeid(typename Context<DataClass>::type))),
          data_(proxy->data), default_(proxy->data), context_{}
    {
        // Store the proxy data smart pointer to preserve the lifetime of the data
        proxyData_ = proxy;
    }
    virtual ~Parameter() = default;

    /*
     * Definition
     */
    public:
    // Return the number of allowed input edges
    AllowedEdgeCount nAllowedInputEdges() const override
    {
        if (flags_.isSet(ParameterFlags::Output))
            return AllowedEdgeCount::Zero;
        else if constexpr (is_instance_of_v<DataClass, std::vector>)
            return AllowedEdgeCount::AnyNumber;
        else
            return AllowedEdgeCount::One;
    }
    // The type's representation as a raw int (only valid for int and enum)
    int getAsInt() const override
    {
        if constexpr (std::is_integral_v<DataClass>)
            return data_;
        if constexpr (std::is_enum_v<DataClass>)
            return static_cast<int>(data_);

        Messenger::exception("Tried to extract int value from non integral type {}", storedDataType_.name());
    }
    // Set type's representation as a raw int (only valid for int and enum)
    void setFromInt(int value) override
    {
        if constexpr (std::is_integral_v<DataClass>)
            data_ = value;
        else if constexpr (std::is_enum_v<DataClass>)
            data_ = static_cast<DataClass>(value);
        else
            Messenger::exception("Tried to extract int value from non integral type {}", storedDataType_.name());
        return;
    }

    /*
     * Data
     */
    protected:
    // Reference to target data
    DataClass &data_;
    // Reference to target context
    Context<DataClass>::type context_;
    // Specialised container for local pointer referencing, if relevant
    std::conditional_t<std::is_pointer_v<DataClass>, DataClass, bool> localPointer_;
    // Getter for target data, defaulting so simple return of data_ reference member
    using DataGetter = std::function<DataClass()>;
    DataGetter dataGetter_{[&]() { return data_; }};
    // Setter for target data, defaulting to simple 1-to-1 copy as long as equality fails
    using DataSetter = std::function<void(const DataClass &value)>;
    DataSetter dataSetter_{[&](const DataClass &value) { data_ = value; }};
    // Initial value
    const DataClass default_;
    // Parameter proxy data (if a ParameterLink)
    std::shared_ptr<ParameterProxy<DataClass>> proxyData_;

    private:
    // Perform any updates after a successful setData()
    void updateAfterSet() const
    {
        // Changing parameters always flags an update as being required, unless the NoUpdate flag is set
        if (!flags_.isSet(NoUpdate))
            setParentUpdateRequired();

        // Setting some parameters forces any local data to be cleared
        if (flags_.isSet(ClearData))
            clearDataInParent();
    }

    public:
    // Return whether the contained data is an instance of std::vector
    bool isVector() const override
    {
        if constexpr (is_instance_of_v<DataClass, std::vector>)
            return true;
        return false;
    }
    // Assign the value of another parameter to this one.
    bool assign(ParameterBase *other) override
    {
        if constexpr (std::is_pointer<DataClass>())
        {
            // If we are a pointer type, getting a nullptr is disallowed
            setData(other->get<DataClass>());

            return data_ != nullptr;
        }
        else if constexpr (is_instance_of_v<DataClass, std::vector>)
        {
            // If we represent a std::vector container we can conditionally check for a single data item being passed

            // Vector to vector
            if (storedDataType_ == other->storedDataType())
            {
                setData(other->get<DataClass>());

                return true;
            }

            // Single value to vector
            if (std::type_index(typeid(typename DataClass::value_type)) == other->storedDataType())
            {
                data_.push_back(other->get<typename DataClass::value_type>());

                updateAfterSet();

                return true;
            }
        }
        else if constexpr (is_instance_of_v<DataClass, std::optional>)
        {
            // Optional arguments can be set from the base class (i.e. with no std::optional container) as well as std::optional

            // Optional to optional
            if (storedDataType_ == other->storedDataType())
            {
                setData(other->get<DataClass>());

                return true;
            }

            // Base type into std::optional
            if (std::type_index(typeid(typename DataClass::value_type)) == other->storedDataType())
            {
                setData(other->get<typename DataClass::value_type>());

                return true;
            }
        }
        else if (typeid(std::optional<DataClass>) == other->storedDataType())
        {
            // Data types can be set from a std::optional containing the same type

            auto otherData = other->get<std::optional<DataClass>>();
            if (otherData.has_value())
            {
                setData(*otherData);
                return true;
            }
        }

        // General case - if the stored data types are the same then we can just do a straight assignment
        if (storedDataType_ == other->storedDataType())
        {
            setData(other->get<DataClass>());
            return true;
        }

        return false;
    }
    // Return whether this parameter accepts the output type of the other
    bool acceptsOutput(ParameterBase *other) const override
    {
        if (storedDataType_ == other->storedDataType())
            return true;

        // Normal data types can be set from optional values
        if (typeid(std::optional<DataClass>) == other->storedDataType())
            return true;
        else if constexpr (is_instance_of_v<DataClass, std::vector>)
        {
            // Vectors can accept a single value of the contained type
            if (std::type_index(typeid(typename DataClass::value_type)) == other->storedDataType())
                return true;
        }
        else if constexpr (is_instance_of_v<DataClass, std::optional>)
        {
            // Optionals can accept non-optional data
            if (std::type_index(typeid(typename DataClass::value_type)) == other->storedDataType())
                return true;
        }

        return false;
    }
    // Invalidate the vector data (instances of std::vector only)
    void invalidateVector() override
    {
        if constexpr (is_instance_of_v<DataClass, std::vector>)
        {
            // Mark all incoming edges to us as needing a re-pull
            markIncomingEdgesForPull();

            // Empty the vector
            data_.clear();
        }
        else
            throw(std::runtime_error(std::format("Parameter<{}>::invalidateVector() - Tried to invalidate a non-vector type.",
                                                 storedDataType_.name())));
    }
    // Set the parameter value
    void setData(const DataClass &value)
    {
        dataSetter_(value);
        updateAfterSet();
    }
    // Return the parameter value
    virtual DataClass getData() { return dataGetter_(); }
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
};

// Primary type for a Parameter to a specific DataClass
template <typename DataClass> class SerialisableParameter : public Parameter<DataClass>
{
    public:
    SerialisableParameter(Node *parent, std::string_view name, std::string_view description, DataClass &value,
                          typename Context<DataClass>::type context = {})
        : Parameter<DataClass>(parent, name, description, value, context)
    {
    }
    // Helper templates for handling serialisation

    template <typename V> struct is_ptr_vector : std::false_type
    {
    };

    template <serialisablePointer E> struct is_ptr_vector<std::vector<E>> : std::true_type
    {
    };

    /*
     * Serialisation
     */
    public:
    // Express as a serialised value
    void serialise(std::string tag, SerialisedValue &target) const override
    {
        SerialisedValue result = {};

        // Serialise non-pointer values
        if constexpr (HasEnumOptions<DataClass>)
            result["data"] = getEnumOptions(Parameter<DataClass>::data_).serialise(Parameter<DataClass>::data_);
        else if constexpr (std::is_convertible<DataClass, Number>::value)
            result["data"] = Parameter<DataClass>::data_;
        else if constexpr (std::is_convertible<DataClass, std::string>::value)
            result["data"] = Parameter<DataClass>::data_;
        else if constexpr (std::is_convertible<DataClass, std::optional<Number>>::value)
        {
            if (Parameter<DataClass>::data_)
                result["data"] = *Parameter<DataClass>::data_;
        }
        else if constexpr (serialisablePointer<DataClass>)
            Parameter<DataClass>::data_->serialise("data", result);
        else
            result["data"] = Parameter<DataClass>::data_;

        target[tag] = result;
    };
    // Read from a serialised value
    void deserialise(const SerialisedValue &node) override
    {
        if constexpr (std::is_pointer<DataClass>::value)
        {
            Parameter<DataClass>::data_ = nullptr;
        }
        else if constexpr (is_ptr_vector<DataClass>::value)
            Parameter<DataClass>::data_.clear();
        else if constexpr (HasEnumOptions<DataClass>)
        {
            DataClass proxy; // Fake T value to get the correct overload
            Parameter<DataClass>::data_ = getEnumOptions(proxy).deserialise(node);
        }
        else if constexpr (std::is_convertible<DataClass, std::optional<double>>::value)
        {
            if (node.contains("data"))
                Parameter<DataClass>::data_ = toml::find<double>(node, "data");
            else
                Parameter<DataClass>::data_ = {};
        }
        else if constexpr (std::is_convertible<DataClass, std::optional<Number>>::value)
        {
            if (node.contains("data"))
                Parameter<DataClass>::data_ = toml::find<Number>(node, "data");
            else
                Parameter<DataClass>::data_ = {};
        }
        else if constexpr (std::is_convertible<DataClass, std::optional<Data1D>>::value)
        {
            if (node.contains("data"))
                Parameter<DataClass>::data_ = toml::find<Data1D>(node, "data");
            else
                Parameter<DataClass>::data_ = {};
        }
        else if constexpr (serialisablePointer<DataClass>)
        {
            CoreData coreData; // Temporary patch until we fix up the deserialisation
            Parameter<DataClass>::data_->deserialise(node.at("data"), coreData);
        }
        else
        {
            Parameter<DataClass>::data_ = toml::find<DataClass>(node, "data");
        }
    }
};
