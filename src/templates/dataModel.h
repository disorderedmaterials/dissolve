// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/flags.h"
#include <fmt/core.h>
#include <functional>
#include <optional>
#include <string>
#include <variant>
#include <vector>

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
    ReadOnly
};

// Index / Column ID, Name / Column Title, Data Type, ReadOnly?
using DataItemProperty = std::tuple<int, std::string, PropertyType, Flags<PropertyFlag>>;

// Helper type for PropertyValue visitor
template <class... Ts> struct DataItemVisitor : Ts...
{
    using Ts::operator()...;
};
// Explicit deduction guide for PropertyValue visitor
template <class... Ts> DataItemVisitor(Ts...) -> DataItemVisitor<Ts...>;

/*
 * DataModelBase - An abstract class intended to allow our custom C++ data classes to be accessed in a consistent
 * and simplified way through Qt's Model-View classes.
 */
class DataModelBase
{
    public:
    DataModelBase(const std::vector<DataItemProperty> &itemProperties) : itemProperties_(itemProperties) {}

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
    // Return name of specified property
    std::string propertyName(int propertyIndex) { return std::get<1>(itemProperties_[propertyIndex]); }
    // Return property type for the specified column
    PropertyType propertyType(int propertyIndex) { return std::get<2>(itemProperties_[propertyIndex]); }
    // Return whether the specified property flag is set
    bool isPropertyFlagSet(int propertyIndex, PropertyFlag flag)
    {
        return std::get<3>(itemProperties_[propertyIndex]).isSet(flag);
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
};

template <class DataItem> class DataTableModel : public DataModelBase
{
    public:
    DataTableModel(std::vector<DataItem> &data, const std::vector<DataItemProperty> &itemProperties)
        : DataModelBase(itemProperties), data_(data)
    {
    }

    private:
    // Target data for the model
    std::vector<DataItem> &data_;

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
    private:
    // Data setter function - unique to DataItem class
    using PropertySetFunction = std::function<bool(DataItem &, int, PropertyValue)>;
    PropertySetFunction setPropertyFunction_;
    // Data getter function - unique to DataItem class
    using PropertyGetFunction = std::function<PropertyValue(const DataItem &, int)>;
    PropertyGetFunction getPropertyFunction_;

    public:
    // Set property access functions
    void setPropertyFunctions(PropertyGetFunction getFunction, PropertySetFunction setFunction)
    {
        getPropertyFunction_ = std::move(getFunction);
        setPropertyFunction_ = std::move(setFunction);
    }
    // Set property
    bool setProperty(int dataIndex, int propertyIndex, const PropertyValue &newValue) final
    {
        // Check index validity
        if (!isIndexValid(dataIndex, propertyIndex))
            return false;

        if (std::get<3>(itemProperties_[propertyIndex]).isSet(PropertyFlag::ReadOnly))
        {
            fmt::print("Refusing to set data '{}' since it is read-only.\n", std::get<1>(itemProperties_[propertyIndex]));
            return false;
        }

        // Set the child at the specified index
        if (!setPropertyFunction_)
            return false;
        else
            return setPropertyFunction_(data_[dataIndex], std::get<0>(itemProperties_[propertyIndex]), newValue);
    }
    // Get property
    PropertyValue getProperty(int dataIndex, int propertyIndex) final
    {
        // Check index validity
        if (!isIndexValid(dataIndex, propertyIndex))
            return {};

        // Set the child at the specified index
        if (!getPropertyFunction_)
            return {};
        else
            return getPropertyFunction_(data_[dataIndex], std::get<0>(itemProperties_[propertyIndex]));
    }

    /*
     * Item Management
     */
    private:
    // Creation function (if required) otherwise default constructor T() will be called
    using CreateItemFunction = std::function<DataItem(std::optional<int>)>;
    CreateItemFunction createItemFunction_;

    public:
    // Set data creation function
    void setDataCreationFunction(CreateItemFunction function) { createItemFunction_ = std::move(function); }

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
};
