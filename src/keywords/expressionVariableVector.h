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

// Name / Column Title, Data Type, ReadOnly?
using DataItemProperty = std::tuple<int, std::string, PropertyType, bool>;

struct DataItemValue
{
    PropertyType type;
    union
    {
        int intValue{0};
        double doubleValue;
        std::string stringValue;
    };
};
template <class DataItem> class DataModel
{
    public:
    // Data access functions
    using DataSetFunction = std::function<bool(DataItem&, int, DataItemValue)>;
    using DataGetFunction = std::function<DataItemValue(DataItem&, int)>;
    DataModel(std::vector<DataItem> &data, const std::vector<DataItemProperty> &itemProperties) : data_(data), itemProperties_(itemProperties) {}

    private:
    // Target data for the model
    std::vector<DataItem> &data_;

    /*
     * Extent
     */
    private:
    // Return whether the supplied index is valid
    bool isIndexValid(int childIndex, int columnIndex) const { return (childIndex >= 0 && childIndex < data_.size() && columnIndex >= 0); }
    // Functions for accessing data extents (table style)
    std::function<int(int row, int column)> childCountFunction_{
        [&](const int childIndex, const int columnIndex) { return isIndexValid(childIndex, columnIndex) ? 0 : data_.size(); }};
    std::function<int(int row, int column)> dataItemCountFunction_{
        [&](const int childIndex, const int columnIndex) { return !isIndexValid(childIndex, columnIndex) ? 0 : itemProperties_.size(); }};

    public:
    // Return number of children (rows) for the specified index
    int nChildren(int childIndex, int columnIndex) { return childCountFunction_(childIndex, columnIndex); }
    // Return number of data items (columns) for the specified index
    int nDataItems(int childIndex, int columnIndex) { return dataItemCountFunction_(childIndex, columnIndex); }

    /*
     * Data Access
     */
    private:
    // Descriptions of relevant item properties within a single object in the container
    std::vector<DataItemProperty> itemProperties_;
    // Return whether column index holds / can be set by the given type
    bool isPropertyType(int columnIndex, PropertyType descriptorType) { return std::get<2>(itemProperties_[columnIndex]) == descriptorType; }
    // Set / get functions, unique per class
    DataSetFunction setDataFunction_;
    DataGetFunction getDataFunction_;

    public:
    // Set data functions
    bool setData(int childIndex, int columnIndex, const DataItemValue &newValue) {
        // Check index validity
        if (!isIndexValid(childIndex, columnIndex))
            return false;

        if (!isPropertyType(columnIndex, PropertyType::Integer))
        {
            fmt::print("Refusing to set data '{}' with an int since it is of a different type.\n", std::get<1>(itemProperties_[columnIndex]));
            return false;
        }

        // Set the child at the specified index
        if (!setDataFunction_)
            return false;
        else
            return setDataFunction_(data_[childIndex], std::get<0>(itemProperties_[columnIndex]), newValue);
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
    DataModel<std::shared_ptr<ExpressionVariable>> dataModel_;
    // Parent ProcedureNode
    ProcedureNode *parentNode_;

    public:
    // Has not changed from initial value
    bool isDefault() const override;
    // Return reference to vector of data
    std::vector<std::shared_ptr<ExpressionVariable>> &data();
    const std::vector<std::shared_ptr<ExpressionVariable>> &data() const;
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
