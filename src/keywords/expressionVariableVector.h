// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "expression/node.h"
#include "keywords/base.h"

// Forward Declarations
class ExpressionVariable;
class ProcedureNode;

// Data property types
enum class PropertyType
{
    Invalid,
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

class DataItemValue
{
    public:
    DataItemValue() : type_(PropertyType::Invalid) {}
    DataItemValue(int i) : type_(PropertyType::Integer), intValue_(i) {}
    DataItemValue(double d) : type_(PropertyType::Double), doubleValue_(d) {}
    DataItemValue(std::string_view sv) : type_(PropertyType::String), stringValue_(sv) {}
    DataItemValue(std::string s) : type_(PropertyType::String), stringValue_(std::move(s)) {}

    private:
    PropertyType type_;
    int intValue_{0};
    double doubleValue_{0.0};
    std::string stringValue_;

    public:
    // Return value type
    PropertyType type() const { return type_; }
    // Return integer value
    int intValue() const { return intValue_; }
    // Return double value
    double doubleValue() const { return doubleValue_; }
    // Return string value
    std::string stringValue() const { return stringValue_; }
};

class DataModelBase
{
    public:
    DataModelBase(const std::vector<DataItemProperty> &itemProperties) : itemProperties_(itemProperties) {}

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
    // Return number of children (rows) for the specified index
    virtual int nChildren(int dataIndex, int propertyIndex) = 0;
    // Return number of properties (i.e. columns) for the specified index
    virtual int nProperties(int dataIndex, int propertyIndex) = 0;
    // Set property function
    virtual bool setProperty(int dataIndex, int propertyIndex, const DataItemValue &newValue) = 0;
    // Get property function
    virtual DataItemValue getProperty(int dataIndex, int propertyIndex) = 0;
};

template <class DataItem> class DataTableModel : public DataModelBase
{
    public:
    // Data access functions
    using PropertySetFunction = std::function<bool(DataItem &, int, DataItemValue)>;
    using PropertyGetFunction = std::function<DataItemValue(const DataItem &, int)>;
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
    // Functions for accessing data extents (table style)
    std::function<int(int row, int column)> childCountFunction_{[&](const int dataIndex, const int propertyIndex) {
        return isIndexValid(dataIndex, propertyIndex) ? 0 : data_.size();
    }};
    std::function<int(int row, int column)> propertyCountFunction_{[&](const int dataIndex, const int propertyIndex)
                                                                   { return itemProperties_.size(); }};

    public:
    // Return number of children (rows) for the specified index
    int nChildren(int dataIndex, int propertyIndex) final { return childCountFunction_(dataIndex, propertyIndex); }
    // Return number of properties per child (i.e. columns) for the specified index
    int nProperties(int dataIndex, int propertyIndex) final { return propertyCountFunction_(dataIndex, propertyIndex); }

    /*
     * Data Access
     */
    private:
    // Set / get functions, unique per class
    PropertySetFunction setPropertyFunction_;
    PropertyGetFunction getPropertyFunction_;

    public:
    // Set property access functions
    void setPropertyFunctions(PropertyGetFunction getFunction, PropertySetFunction setFunction)
    {
        getPropertyFunction_ = std::move(getFunction);
        setPropertyFunction_ = std::move(setFunction);
    }
    // Set property
    bool setProperty(int dataIndex, int propertyIndex, const DataItemValue &newValue) final
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
    DataItemValue getProperty(int dataIndex, int propertyIndex) final
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
};

// Keyword managing vector of ExpressionVariable
class ExpressionVariableVectorKeyword : public KeywordBase
{
    public:
    ExpressionVariableVectorKeyword(std::vector<std::shared_ptr<ExpressionVariable>> &data, ProcedureNode *parentNode);
    ~ExpressionVariableVectorKeyword() override = default;

    /*
     * Data
     */
    private:
    // Reference to vector of data
    std::vector<std::shared_ptr<ExpressionVariable>> &data_;
    DataTableModel<std::shared_ptr<ExpressionVariable>> dataModel_;
    // Parent ProcedureNode
    ProcedureNode *parentNode_;

    public:
    // Has not changed from initial value
    bool isDefault() const override;
    // Return reference to vector of data
    std::vector<std::shared_ptr<ExpressionVariable>> &data();
    const std::vector<std::shared_ptr<ExpressionVariable>> &data() const;
    // Return data model
    DataTableModel<std::shared_ptr<ExpressionVariable>> &dataModel();
    // Return parent ProcedureNode
    ProcedureNode *parentNode();
    const ProcedureNode *parentNode() const;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const override;
    // Return maximum number of arguments accepted
    std::optional<int> maxArguments() const override;
    // Deserialise from supplied LineParser, starting at given argument offset
    bool deserialise(LineParser &parser, int startArg, const CoreData &coreData) override;
    // Serialise data to specified LineParser
    bool serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const override;
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node, const CoreData &coreData) override;
};
