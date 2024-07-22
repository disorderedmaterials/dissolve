// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/dataModelItem.h"

namespace DataModel
{
/*
 * DataModel::Base - An abstract class intended to allow our custom C++ data classes to be accessed in a consistent
 * and simplified way through Qt's Model-View classes, without the need for a specific Qt model for each.
 */
class Base
{
    public:
    Base() {}

    /*
     * Properties
     */
    protected:
    // Descriptions of relevant item properties within a single object in the container
    std::vector<ItemProperty> itemProperties_;

    protected:
    // Add item property
    void addItemProperty(std::string_view name, ItemProperty::PropertyType type,
                         const Flags<ItemProperty::PropertyFlag> &flags = {})
    {
        itemProperties_.emplace_back(name, type, flags);
    }

    public:
    // Return name of specified property
    std::string propertyName(int propertyIndex) { return itemProperties_[propertyIndex].name(); }
    // Return property type for the specified property
    ItemProperty::PropertyType propertyType(int propertyIndex) { return itemProperties_[propertyIndex].type(); }
    // Return all flags set for specified property
    const Flags<ItemProperty::PropertyFlag> &propertyFlags(int propertyIndex) { return itemProperties_[propertyIndex].flags(); }
    // Return whether the specified property flag is set
    bool isPropertyFlagSet(int propertyIndex, ItemProperty::PropertyFlag flag)
    {
        return itemProperties_[propertyIndex].flags().isSet(flag);
    }

    public:
    // Return number of data items (i.e. rows) for the specified index
    virtual int nDataItems() const = 0;
    // Return number of properties (i.e. columns) for the specified index
    virtual int nProperties() const = 0;
    // Set property function
    virtual bool setProperty(int dataIndex, int propertyIndex, const PropertyValue &newValue) = 0;
    // Get property function
    virtual PropertyValue getProperty(int dataIndex, int propertyIndex) const = 0;

    /*
     * Item Management
     */
    public:
    // Create new item(s) starting at specified vector index
    virtual void createItems(int index, int count) = 0;
    // Append new item(s) to the end of the data
    virtual void appendItems(int count) = 0;
    // Remove item(s) starting at specified vector index
    virtual void removeItems(int index, int count) = 0;
    // Clear all items
    virtual void clear() = 0;

    /*
     * Signalling
     */
    public:
    // Mutation Signals
    enum class MutationSignal
    {
        PropertyChanged,
        DataCreationStarted,
        DataCreationFinished,
        DataRemovalStarted,
        DataRemovalFinished
    };

    private:
    // Mutation signal function used to notify an associated Qt model
    using DataMutationSignalFunction = std::function<void(MutationSignal, int, int)>;
    DataMutationSignalFunction mutationSignalFunction_ = {};

    public:
    // Set mutation signal function
    void setMutationSignalFunction(DataMutationSignalFunction mutationSignalFunction)
    {
        mutationSignalFunction_ = std::move(mutationSignalFunction);
    }
    // Emit mutation signal
    void emitMutationSignal(MutationSignal signal, int startIndex = 0, int endIndex = 0)
    {
        if (mutationSignalFunction_)
            mutationSignalFunction_(signal, startIndex, endIndex);
    }
};
} // namespace DataModel
