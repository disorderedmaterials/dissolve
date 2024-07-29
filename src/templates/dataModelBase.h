// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "templates/dataModelItem.h"
#include <map>

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
    void addItemProperty(const ItemProperty &property) { itemProperties_.emplace_back(property); }

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
    int nProperties() const { return itemProperties_.size(); }
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
        DataMutationStarted,
        DataMutationFinished,
        DataCreationStarted,
        DataCreationFinished,
        DataRemovalStarted,
        DataRemovalFinished
    };

    private:
    // Mutation signal function used to notify an associated Qt model
    using DataMutationSignalFunction = std::function<void(MutationSignal, int, int)>;
    std::map<void *, DataMutationSignalFunction> mutationSignalFunctions_;

    public:
    // Add mutation signal function
    void addMutationSignalFunction(void *object, const DataMutationSignalFunction &mutationSignalFunction)
    {
        mutationSignalFunctions_[object] = mutationSignalFunction;
    }
    // Remove mutation signal function for specified object
    void removeMutationSignalFunction(void *object)
    {
        auto it = mutationSignalFunctions_.find(object);
        if (it != mutationSignalFunctions_.end())
            mutationSignalFunctions_.erase(it);
    }
    // Emit mutation signal
    void emitMutationSignal(MutationSignal signal, int startIndex = 0, int endIndex = 0)
    {
        for (const auto &[object, func] : mutationSignalFunctions_)
            func(signal, startIndex, endIndex);
    }
};
} // namespace DataModel
