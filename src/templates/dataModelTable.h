// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/dataModelBase.h"
#include <map>
#include <optional>
#include <vector>

namespace DataModel
{
// Table Model for Data based on std::vector
template <class DataItemClass, class DataItem> class Table : public Base
{
    public:
    // Modelable properties
    using ModelableProperties = std::vector<typename Modelable<DataItemClass>::ModelableProperty>;
    // Property get function
    using PropertyGetFunction = std::function<PropertyValue(const DataItemClass *)>;
    // Property set function
    using PropertySetFunction = std::function<bool(DataItemClass *, PropertyValue)>;

    Table() : Base()
    {
        // Add properties from the modelable base class
        for (auto &[name, type, flags, getter, setter] : Modelable<DataItemClass>::modelableProperties())
        {
            addProperty(name, type, flags, getter, setter);
        }
    }

    /*
     * Target Data and Vector Compatibility
     */
    private:
    // Target data for the model
    std::vector<DataItem> data_;

    public:
    // Return vector
    const std::vector<DataItem> &data() const { return data_; }
    // Return opening iterator for the data
    typename std::vector<DataItem>::const_iterator begin() const { return data_.begin(); }
    // Return ending iterator for the data
    typename std::vector<DataItem>::const_iterator end() const { return data_.end(); }
    // Return whether the data is empty
    bool empty() const { return data_.empty(); }

    /*
     * Setters / Getters
     */
    private:
    // Map of named properties to data getters
    std::map<std::string, PropertyGetFunction> getters_;
    // Map of named properties to data setters
    std::map<std::string, PropertySetFunction> setters_;

    private:
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

    public:
    // Add or override setter for specified property
    void setSetter(std::string name, PropertySetFunction setter) { setters_[name] = std::move(setter); }

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
    // Call getter
    PropertyValue callGetter(PropertyGetFunction &getter, DataItemClass &item) { return getter(&item); }
    PropertyValue callGetter(PropertyGetFunction &getter, std::shared_ptr<DataItemClass> &item) { return getter(item.get()); }
    PropertyValue callGetter(PropertyGetFunction &getter, std::unique_ptr<DataItemClass> &item) { return getter(item.get()); }
    // Call setter
    bool callSetter(PropertySetFunction &setter, DataItemClass &item, const PropertyValue &newValue)
    {
        return setter(&item, newValue);
    }
    bool callSetter(PropertySetFunction &setter, std::shared_ptr<DataItemClass> &item, const PropertyValue &newValue)
    {
        return setter(item.get(), newValue);
    }
    bool callSetter(PropertySetFunction &setter, std::unique_ptr<DataItemClass> &item, const PropertyValue &newValue)
    {
        return setter(item.get(), newValue);
    }

    public:
    // Get property
    PropertyValue getProperty(int dataIndex, int propertyIndex) final
    {
        // Check index validity
        if (!isIndexValid(dataIndex, propertyIndex))
            return {};

        return callGetter(getters_[itemProperties_[propertyIndex].name()], data_[dataIndex]);
    }
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
            return callSetter(setters_[itemProperties_[propertyIndex].name()], data_[dataIndex], newValue);
    }

    /*
     * Item Management
     */
    private:
    // Item creation function (if required) otherwise a suitable default constructor will be called
    using CreateItemFunction = std::function<DataItem()>;
    CreateItemFunction createItemFunction_;
    // Item removal function (if required)
    using RemoveItemFunction = std::function<void(int)>;
    RemoveItemFunction removeItemFunction_;

    private:
    // Create new item
    void newItem(std::vector<DataItemClass> &vector, std::optional<int> position = {})
    {
        if (position)
            data_.insert(data_.begin() + *position, createItemFunction_ ? createItemFunction_() : DataItem());
        else
            data_.emplace_back(createItemFunction_ ? createItemFunction_() : DataItem());
    }
    void newItem(std::vector<std::shared_ptr<DataItemClass>> &vector, std::optional<int> position = {})
    {
        if (position)
            data_.insert(data_.begin() + *position,
                         createItemFunction_ ? createItemFunction_() : std::make_shared<DataItemClass>());
        else
            data_.emplace_back(createItemFunction_ ? createItemFunction_() : std::make_shared<DataItemClass>());
    }
    void newItem(std::vector<std::unique_ptr<DataItemClass>> &vector, std::optional<int> position = {})
    {
        if (position)
            data_.insert(data_.begin() + *position,
                         createItemFunction_ ? createItemFunction_() : std::make_unique<DataItemClass>());
        else
            data_.emplace_back(createItemFunction_ ? createItemFunction_() : std::make_unique<DataItemClass>());
    }

    public:
    // Set data creation function
    void setCreator(CreateItemFunction function) { createItemFunction_ = std::move(function); }
    // Set data removal function
    void setRemover(RemoveItemFunction function) { removeItemFunction_ = std::move(function); }
    // Create new item(s) starting at specified vector index
    void createItems(int index, int count) final
    {
        emitMutationSignal(Base::MutationSignal::DataCreationStarted, index, index + count - 1);
        for (auto n = 0; n < count; ++n)
            newItem(data_, index + n);
        emitMutationSignal(Base::MutationSignal::DataCreationFinished);
    }
    // Insert a specified item at back of the vector or a specified position
    void insertThis(DataItem &newItem, std::optional<int> position = {})
    {
        if (position)
        {
            emitMutationSignal(Base::MutationSignal::DataCreationStarted, *position, *position);
            data_.insert(data_.begin() + *position, newItem);
            emitMutationSignal(Base::MutationSignal::DataCreationFinished);
        }
        else
        {
            emitMutationSignal(Base::MutationSignal::DataCreationStarted, data_.size(), data_.size());
            data_.emplace_back(newItem);
            emitMutationSignal(Base::MutationSignal::DataCreationFinished);
        }
    }
    // Append new item to the end of the data and return it
    DataItem &appendItem()
    {
        emitMutationSignal(Base::MutationSignal::DataCreationStarted, data_.size(), data_.size());
        newItem(data_);
        emitMutationSignal(Base::MutationSignal::DataCreationFinished);
        return data_.back();
    }
    // Append new item(s) to the end of the data
    void appendItems(int count) final
    {
        emitMutationSignal(Base::MutationSignal::DataCreationStarted, data_.size(), data_.size() + count - 1);
        for (auto n = 0; n < count; ++n)
            newItem(data_);
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
    // Clear all items
    void clear() final
    {
        if (empty())
            return;

        emitMutationSignal(Base::MutationSignal::DataRemovalStarted, 0, data_.size() - 1);
        if (removeItemFunction_)
        {
            for (auto n = 0; n < data_.size(); ++n)
                removeItemFunction_(0);
        }
        else
            data_.clear();
        emitMutationSignal(Base::MutationSignal::DataRemovalFinished);
    }
};
} // namespace DataModel
