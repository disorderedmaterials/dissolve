// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/flags.h"
#include <fmt/core.h>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

// Index / Column ID, Name / Column Title, Data Type, ReadOnly?
class DataItemProperty
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
    DataItemProperty(std::string_view name, PropertyType type, Flags<PropertyFlag> flags = {})
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

// Helper type for PropertyValue visitor
template <class... Ts> struct DataItemVisitor : Ts...
{
    using Ts::operator()...;
};
// Explicit deduction guide for PropertyValue visitor
template <class... Ts> DataItemVisitor(Ts...) -> DataItemVisitor<Ts...>;

/*
 * DataModelBase - An abstract class intended to allow our custom C++ data classes to be accessed in a consistent
 * and simplified way through Qt's Model-View classes, without the need for a specific Qt model for each.
 */
class DataModelBase
{
    public:
    DataModelBase() {}

    /*
     * Property Values
     */
    public:
    // Property value variant
    using PropertyValue = std::variant<int, double, std::string_view, std::string>;
    // Return value as string
    static std::string asString(const PropertyValue &value)
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

    /*
     * Properties
     */
    protected:
    // Descriptions of relevant item properties within a single object in the container
    std::vector<DataItemProperty> itemProperties_;

    public:
    // Add item property
    void addItemProperty(std::string_view name, DataItemProperty::PropertyType type,
                         Flags<DataItemProperty::PropertyFlag> flags = {})
    {
        itemProperties_.emplace_back(name, type, flags);
    }

    // Return name of specified property
    std::string propertyName(int propertyIndex) { return itemProperties_[propertyIndex].name(); }
    // Return property type for the specified property
    DataItemProperty::PropertyType propertyType(int propertyIndex) { return itemProperties_[propertyIndex].type(); }
    // Return all flags set for specified property
    const Flags<DataItemProperty::PropertyFlag> &propertyFlags(int propertyIndex)
    {
        return itemProperties_[propertyIndex].flags();
    }
    // Return whether the specified property flag is set
    bool isPropertyFlagSet(int propertyIndex, DataItemProperty::PropertyFlag flag)
    {
        return itemProperties_[propertyIndex].flags().isSet(flag);
    }

    public:
    // Return number of data items (i.e. rows) for the specified index
    virtual int nDataItems() = 0;
    // Return number of properties (i.e. columns) for the specified index
    virtual int nProperties() = 0;
    // Set property function
    virtual bool setProperty(int dataIndex, int propertyIndex, const PropertyValue &newValue) = 0;
    // Get property function
    virtual PropertyValue getProperty(int dataIndex, int propertyIndex) = 0;

    /*
     * Item Management
     */
    public:
    enum class MutationSignal
    {
        PropertyChanged,
        DataCreationStarted,
        DataCreationFinished,
        DataRemovalStarted,
        DataRemovalFinished
    };
    using DataMutationSignalFunction = std::function<void(MutationSignal, int, int)>;
    DataMutationSignalFunction mutationSignalFunction_ = {};
    void setMutationSignalFunction(DataMutationSignalFunction mutationSignalFunction)
    {
        mutationSignalFunction_ = std::move(mutationSignalFunction);
    }
    void emitMutationSignal(MutationSignal signal, int startIndex = 0, int endIndex = 0)
    {
        if (mutationSignalFunction_)
            mutationSignalFunction_(signal, startIndex, endIndex);
    }
    // Create new item(s) starting at specified vector index
    virtual void createItems(int index, int count) = 0;
    // Append new item(s) to the end of the data
    virtual void appendItems(int count) = 0;
    // Remove item(s) starting at specified vector index
    virtual void removeItems(int index, int count) = 0;
};

template <class DataItem> class DataTableModel : public DataModelBase
{
    public:
    // Property get function
    using PropertyGetFunction = std::function<PropertyValue(const DataItem &)>;
    // Property set function
    using PropertySetFunction = std::function<bool(DataItem &, PropertyValue)>;

    DataTableModel(std::vector<DataItem> &data) : DataModelBase(), data_(data) {}

    /*
     * Target Data and Functions
     */
    private:
    // Target data for the model
    std::vector<DataItem> &data_;
    // Map of named properties to data getters
    std::map<std::string, PropertyGetFunction> getters_;
    // Map of named properties to data setters
    std::map<std::string, PropertySetFunction> setters_;

    public:
    // Add item property for use in the model
    void addProperty(const std::string &name, DataItemProperty::PropertyType type, Flags<DataItemProperty::PropertyFlag> flags,
                     PropertyGetFunction getter, PropertySetFunction setter = {})
    {
        // Add the property base info - the order will be reflected in the table model
        addItemProperty(name, type, flags);

        // Store functions
        getters_[name] = std::move(getter);
        if (setter)
            setters_[name] = std::move(setter);
    }

    /*
     * Extent
     */
    private:
    // Return whether the supplied index is valid
    bool isIndexValid(int dataIndex, int propertyIndex) const
    {
        return dataIndex >= 0 && dataIndex < data_.size() && propertyIndex >= 0 && propertyIndex < itemProperties_.size();
    }

    public:
    // Return number of data items (rows) in the table
    int nDataItems() final { return data_.size(); }
    // Return number of properties per data item (i.e. columns) in the table
    int nProperties() final { return itemProperties_.size(); }

    /*
     * Data Access
     */
    public:
    // Set property
    bool setProperty(int dataIndex, int propertyIndex, const PropertyValue &newValue) final
    {
        // Check index validity
        if (!isIndexValid(dataIndex, propertyIndex))
            return false;

        if (itemProperties_[propertyIndex].flags().isSet(DataItemProperty::PropertyFlag::ReadOnly))
        {
            fmt::print("Refusing to set data '{}' since it is read-only.\n", itemProperties_[propertyIndex].name());
            return false;
        }

        // Set the child at the specified index
        if (setters_.find(itemProperties_[propertyIndex].name()) == setters_.end())
            return false;
        else
            return setters_[itemProperties_[propertyIndex].name()](data_[dataIndex], newValue);
    }
    // Get property
    PropertyValue getProperty(int dataIndex, int propertyIndex) final
    {
        // Check index validity
        if (!isIndexValid(dataIndex, propertyIndex))
            return {};

        return getters_[itemProperties_[propertyIndex].name()](data_[dataIndex]);
    }

    /*
     * Item Management
     */
    private:
    // Item creation function (if required) otherwise default constructor T() will be called
    using CreateItemFunction = std::function<DataItem(std::optional<int>)>;
    CreateItemFunction createItemFunction_;
    // Item removal function (if required)
    using RemoveItemFunction = std::function<void(int)>;
    RemoveItemFunction removeItemFunction_;

    public:
    // Set data creation function
    void setDataCreationFunction(CreateItemFunction function) { createItemFunction_ = std::move(function); }
    // Set data removal function
    void setDataRemovalFunction(RemoveItemFunction function) { removeItemFunction_ = std::move(function); }

    public:
    // Create new item(s) starting at specified vector index
    void createItems(int index, int count) final
    {
        emitMutationSignal(DataModelBase::MutationSignal::DataCreationStarted, index, index + count - 1);
        for (auto n = 0; n < count; ++n)
        {
            if (createItemFunction_)
                createItemFunction_(index + n);
            else
                data_.insert(data_.begin() + index, DataItem());
        }
        emitMutationSignal(DataModelBase::MutationSignal::DataCreationFinished);
    }
    // Append new item(s) to the end of the data
    void appendItems(int count) final
    {
        emitMutationSignal(DataModelBase::MutationSignal::DataCreationStarted, data_.size(), data_.size() + count - 1);
        for (auto n = 0; n < count; ++n)
        {
            if (createItemFunction_)
                createItemFunction_({});
            else
                data_.emplace_back(DataItem());
        }
        emitMutationSignal(DataModelBase::MutationSignal::DataCreationFinished);
    }
    // Remove item(s) starting at specified vector index
    void removeItems(int index, int count) final
    {
        emitMutationSignal(DataModelBase::MutationSignal::DataRemovalStarted, index, index + count - 1);
        if (removeItemFunction_)
        {
            for (auto n = 0; n < count; ++n)
                removeItemFunction_(index);
        }
        else
            data_.erase(data_.begin() + index, data_.begin() + index + count);
        emitMutationSignal(DataModelBase::MutationSignal::DataRemovalFinished);
    }
};
