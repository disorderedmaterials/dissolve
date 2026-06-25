// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/context.h"
#include "base/enumOptions.h"
#include "base/serialiser.h"
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
class ParameterBase : public Serialisable
{
    public:
    ParameterBase(Node *parent, std::string_view name, std::string_view description, std::type_index storedDataType);
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
    // Perform any updates after a successful setData()
    void updateAfterSet() const;
    // Return whether this datatype accepts the specified one
    virtual bool acceptsDataFromSource(ParameterBase *source) = 0;
    // Return whether this datatype provides the specified one
    virtual bool providesDataType(std::type_index id) = 0;
    // Assign the value of another parameter to this one
    virtual bool assignDataFromSource(ParameterBase *source) = 0;
    // Assign the value of this parameter to the target one
    virtual bool assignDataTo(ParameterBase *destination) = 0;
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
template <typename DataClass>
std::shared_ptr<ParameterBase> createPointer(Node *parent, std::string_view name, std::string_view description,
                                             std::optional<DataClass> &fromOptional)
{
    return std::make_shared<Parameter<DataClass *>>(parent, name, description, fromOptional);
}
template <typename DataClass>
std::shared_ptr<ParameterBase> createSerialisable(Node *parent, std::string_view name, std::string_view description,
                                                  DataClass &value)
{
    return std::make_shared<SerialisableParameter<DataClass>>(parent, name, description, value);
}
}; // namespace ParameterFactory

// Parameter Data based on std::variant
template <class... Ts> class VariantParameterData
{
    public:
    // Variant data
    std::variant<std::monostate, Ts...> data;

    // Return whether the type_index provided matches one of the supplied variant's allowed types
    bool isAlternative(std::type_index id) const { return ((id == typeid(Ts)) || ...); };
    // Emplace the data of the source parameter into the variant
    bool emplaceFrom(ParameterBase *source)
    {
        return ((source->storedDataType() == typeid(Ts) ? (data = source->get<Ts>()), true : false) || ...);
    }
    // Emplace the variant contents into the destination parameter
    bool emplaceInto(ParameterBase *destination) const
    {
        // Double-fold - check first for straight assignment of an alternative, then for std::optional of the same.
        return ((destination->storedDataType() == typeid(Ts) ? destination->set<Ts>(std::get<Ts>(data)), true : false) ||
                ...) ||
               ((destination->storedDataType() == typeid(std::optional<Ts>)
                 ? destination->set<std::optional<Ts>>(std::get<Ts>(data)),
                 true : false) ||
                ...);
    }
    // Return whether the variant is empty
    bool isEmpty() const { return data.index() == 0; }
    // Check for null pointer in variant, ignoring other types
    bool hasNullPointer()
    {
        return std::visit(
            [](auto &&arg) -> bool
            {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_pointer_v<T>)
                {
                    return arg == nullptr;
                }
                return false;
            },
            data);
    }
};

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
    Parameter(Node *parent, std::string_view name, std::string_view description,
              std::optional<std::remove_pointer_t<DataClass>> &targetData)
        requires(std::is_pointer_v<DataClass>)
        : ParameterBase(parent, name, description, std::type_index(typeid(DataClass))), data_(localPointer_), default_(nullptr),
          dataGetter_([&]() { return targetData.has_value() ? &targetData.value() : nullptr; }),
          dataSetter_([](const DataClass &value) { return false; })
    {
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

        Messenger::exception("Tried to extract int value from non integral type {}", storedDataType_.name());
    }

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
    using DataSetter = std::function<void(const DataClass &value)>;
    DataSetter dataSetter_{[&](const DataClass &value) { data_ = value; }};
    // Initial value
    const DataClass default_;
    // Parameter proxy data (if a ParameterLink)
    std::shared_ptr<ParameterProxy<DataClass>> proxyData_;

    public:
    // Return whether the contained data is an instance of std::vector
    bool isVector() const override
    {
        if constexpr (is_instance_of_v<DataClass, std::vector>)
            return true;
        return false;
    }
    // Return whether this datatype accepts data from the specified source
    bool acceptsDataFromSource(ParameterBase *source) override
    {
        if (source->providesDataType(storedDataType_))
            return true;

        // Optionals and vectors also accept single value_type
        if constexpr (is_instance_of_v<DataClass, std::optional> || is_instance_of_v<DataClass, std::vector>)
            return source->providesDataType(typeid(typename DataClass::value_type));

        // Variants accept multiple data types
        if constexpr (is_instance_of_v<DataClass, VariantParameterData>)
            return data_.isAlternative(source->storedDataType());

        return false;
    }
    // Return whether this datatype provides the specified one
    bool providesDataType(std::type_index id) override
    {
        if (id == storedDataType_)
            return true;

        // Optionals can provide the plain value_type
        if constexpr (is_instance_of_v<DataClass, std::optional>)
            return id == typeid(typename DataClass::value_type);

        // A Variant might contain the right data as one of its alternatives
        if constexpr (is_instance_of_v<DataClass, VariantParameterData>)
            return data_.isAlternative(id);

        return false;
    }
    // Assign the value of another parameter to this one
    bool assignDataFromSource(ParameterBase *source) override
    {
        if constexpr (is_instance_of_v<DataClass, std::vector>)
        {
            // If we represent a std::vector container we can conditionally check for a single data item being passed

            // Vector to vector
            if (storedDataType_ == source->storedDataType())
            {
                setData(source->get<DataClass>());

                return true;
            }

            // Single value to vector
            if (std::type_index(typeid(typename DataClass::value_type)) == source->storedDataType())
            {
                data_.push_back(source->get<typename DataClass::value_type>());

                updateAfterSet();

                return true;
            }
        }
        else if constexpr (is_instance_of_v<DataClass, std::optional>)
        {
            // Optional arguments can be set from the base class (i.e. with no std::optional container) as well as std::optional
            // or a variant containing the correct data type

            // Optional to optional
            if (storedDataType_ == source->storedDataType())
            {
                setData(source->get<DataClass>());

                return true;
            }

            // Base type into std::optional
            if (std::type_index(typeid(typename DataClass::value_type)) == source->storedDataType())
            {
                setData(source->get<typename DataClass::value_type>());

                return true;
            }

            // Variant
            if (source->providesDataType(typeid(typename DataClass::value_type)))
                return source->assignDataTo(this);
        }
        else if (typeid(std::optional<DataClass>) == source->storedDataType())
        {
            // Data types can be set from a std::optional containing the same type
            auto otherData = source->get<std::optional<DataClass>>();
            if (otherData.has_value())
            {
                setData(*otherData);
                return true;
            }
        }
        else if constexpr (is_instance_of_v<DataClass, VariantParameterData>)
        {
            // Variant to variant
            if (storedDataType_ == source->storedDataType())
            {
                setData(source->get<DataClass>());

                // If the variant now contains a pointer type we need to check for nullptr
                if (data_.hasNullPointer())
                    return false;

                return true;
            }

            // Variants can be set from any matching type
            if (data_.emplaceFrom(source))
            {
                // If the variant now contains a pointer type we need to check for nullptr
                if (data_.hasNullPointer())
                    return false;

                updateAfterSet();
                return true;
            }
        }
        else if (storedDataType_ == source->storedDataType())
        {
            // General case - if the stored data types are the same then we can just do a straight assignment
            setData(source->get<DataClass>());

            // If we are a pointer type, getting a nullptr is disallowed
            if constexpr (std::is_pointer<DataClass>())
            {
                return data_ != nullptr;
            }

            return true;
        }
        else
        {
            // Try setting from the source parameter so we have it's full DataClass information - this is necessary for types
            // like std::variant where we need to know the stored alternative.
            if (source->assignDataTo(this))
            {
                updateAfterSet();
                return true;
            }
        }

        return false;
    }
    // Assign the value of this parameter to the target one
    bool assignDataTo(ParameterBase *destination) override
    {
        if constexpr (is_instance_of_v<DataClass, VariantParameterData>)
        {
            // Check that we actually contain a valid value
            if (data_.isEmpty())
                return false;

            // The possibility for a match between the parameters has already been checkwd by the Edge, so here we must just
            // try to emplace the variant's data into the destination parameter.
            if (data_.emplaceInto(destination))
            {
                destination->updateAfterSet();

                return true;
            }
        }
        else if (storedDataType_ == destination->storedDataType())
        {
            // General case - if the stored data types are the same then we can just do a straight assignment
            destination->set<DataClass>(data_);

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
        : Parameter<DataClass>(parent, name, description, value)
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
