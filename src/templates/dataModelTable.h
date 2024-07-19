// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/dataModelBase.h"

namespace DataModel
{
// Table Model for Data
template <class DataItem> class Table : public Base
{
    public:
    // Property get function
    using PropertyGetFunction = std::function<PropertyValue(const DataItem &)>;
    // Property set function
    using PropertySetFunction = std::function<bool(DataItem &, PropertyValue)>;

    Table(std::vector<DataItem> &data) : Base(), data_(data) {}

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
    void addProperty(const std::string &name, ItemProperty::PropertyType type, Flags<ItemProperty::PropertyFlag> flags,
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

        if (itemProperties_[propertyIndex].flags().isSet(ItemProperty::PropertyFlag::ReadOnly))
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
        emitMutationSignal(Base::MutationSignal::DataCreationStarted, index, index + count - 1);
        for (auto n = 0; n < count; ++n)
        {
            if (createItemFunction_)
                createItemFunction_(index + n);
            else
                data_.insert(data_.begin() + index, DataItem());
        }
        emitMutationSignal(Base::MutationSignal::DataCreationFinished);
    }
    // Append new item(s) to the end of the data
    void appendItems(int count) final
    {
        emitMutationSignal(Base::MutationSignal::DataCreationStarted, data_.size(), data_.size() + count - 1);
        for (auto n = 0; n < count; ++n)
        {
            if (createItemFunction_)
                createItemFunction_({});
            else
                data_.emplace_back(DataItem());
        }
        emitMutationSignal(Base::MutationSignal::DataCreationFinished);
    }
    // Remove item(s) starting at specified vector index
    void removeItems(int index, int count) final
    {
        emitMutationSignal(Base::MutationSignal::DataRemovalStarted, index, index + count - 1);
        if (removeItemFunction_)
        {
            for (auto n = 0; n < count; ++n)
                removeItemFunction_(index);
        }
        else
            data_.erase(data_.begin() + index, data_.begin() + index + count);
        emitMutationSignal(Base::MutationSignal::DataRemovalFinished);
    }
};
} // namespace DataModel
