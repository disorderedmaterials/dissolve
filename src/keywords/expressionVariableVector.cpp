// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "keywords/expressionVariableVector.h"
#include "base/lineParser.h"
#include "classes/coreData.h"
#include "expression/variable.h"
#include "procedure/nodes/node.h"
#include <memory>

enum ExpressionVariableProperties
{
    Name,
    Type,
    Value
};
std::vector<DataItemProperty> expressionVariableProperties = {
    {ExpressionVariableProperties::Name, "Name", PropertyType::String, {}},
    {ExpressionVariableProperties::Type, "Type", PropertyType::String, {PropertyFlag::ReadOnly}},
    {ExpressionVariableProperties::Value, "Value", PropertyType::String, {}}};

ExpressionVariableVectorKeyword::ExpressionVariableVectorKeyword(std::vector<std::shared_ptr<ExpressionVariable>> &data,
                                                                 ProcedureNode *parentNode)
    : KeywordBase(typeid(this)), data_(data), parentNode_(parentNode), dataModel_(data_, expressionVariableProperties)
{
    dataModel_.setPropertyFunctions(
        [&](const std::shared_ptr<ExpressionVariable> &var, int propertyIndex)
        {
            switch (propertyIndex)
            {
                case (ExpressionVariableProperties::Name):
                    return DataItemValue(var->baseName());
                case (ExpressionVariableProperties::Type):
                    return DataItemValue(std::string(var->value().type() == ExpressionValue::ValueType::Integer ? "Int" : "Real"));
                case (ExpressionVariableProperties::Value):
                    return DataItemValue(var->value().asString());
                default:
                    return DataItemValue();
            }
        },
        [&](std::shared_ptr<ExpressionVariable> &var, int propertyIndex, const DataItemValue &newValue)
        {
            switch (propertyIndex)
            {
                case (ExpressionVariableProperties::Name):
                {
                    // Must check for existing var in scope with the same name
                    auto p = parentNode_->getParameter(newValue.stringValue());
                    if (p && p != var)
                        return false;
                    var->setBaseName(newValue.stringValue());
                }
                break;
                case (ExpressionVariableProperties::Value):
                {
                    // Value - need to check type (int vs double)
                    bool isFloatingPoint = false;
                    if (DissolveSys::isNumber(newValue.stringValue(), isFloatingPoint))
                    {
                        if (isFloatingPoint)
                            var->setValue(std::stod(newValue.stringValue()));
                        else
                            var->setValue(std::stoi(newValue.stringValue()));
                    }
                    else
                        return Messenger::error("Value '{}' provided for variable '{}' doesn't appear to be a number.\n",
                                                newValue.stringValue(), var->baseName());
                }
                break;
                default:
                    return false;
            }
            return true;
        });
}

/*
 * Data
 */

// Return reference to vector of data
std::vector<std::shared_ptr<ExpressionVariable>> &ExpressionVariableVectorKeyword::data() { return data_; }
const std::vector<std::shared_ptr<ExpressionVariable>> &ExpressionVariableVectorKeyword::data() const { return data_; }

// Return data model
DataTableModel<std::shared_ptr<ExpressionVariable>> &ExpressionVariableVectorKeyword::dataModel() { return dataModel_; }

// Return parent ProcedureNode
ProcedureNode *ExpressionVariableVectorKeyword::parentNode() { return parentNode_; }
const ProcedureNode *ExpressionVariableVectorKeyword::parentNode() const { return parentNode_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int ExpressionVariableVectorKeyword::minArguments() const { return 2; }

// Return maximum number of arguments accepted
std::optional<int> ExpressionVariableVectorKeyword::maxArguments() const { return 2; }

// Deserialise from supplied LineParser, starting at given argument offset
bool ExpressionVariableVectorKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (!parentNode_)
        return Messenger::error("Parent ProcedureNode not set, so can't read ExpressionVariableVector data.\n");

    // First argument is the name of the parameter to create - does it already exist in the node's current scope?
    if (parentNode_->getParameter(parser.argsv(startArg)))
        return Messenger::error("A parameter with the name '{}' is already in scope, and cannot be redefined.\n",
                                parser.argsv(startArg));

    // Create a new one
    auto newVar = parentNode_->addParameter(parser.argsv(startArg));

    // Set the value
    bool isFloatingPoint = false;
    if (DissolveSys::isNumber(parser.argsv(startArg + 1), isFloatingPoint))
    {
        if (isFloatingPoint)
            newVar->setValue(parser.argd(startArg + 1));
        else
            newVar->setValue(parser.argi(startArg + 1));
    }
    else
        return Messenger::error("Value '{}' provided for variable '{}' doesn't appear to be a number.\n",
                                parser.argsv(startArg + 1), parser.argsv(startArg));

    return true;
}

// Serialise data to specified LineParser
bool ExpressionVariableVectorKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    for (const auto &node : data_)
    {
        if (!parser.writeLineF("{}{}  {}  {}\n", prefix, keywordName, node->baseName(), node->value().asString()))
            return false;
    }

    return true;
}

// Has not changed from initial value
bool ExpressionVariableVectorKeyword::isDefault() const { return data_.empty(); }

// Express as a serialisable value
SerialisedValue ExpressionVariableVectorKeyword::serialise() const
{
    SerialisedValue result;
    for (auto &i : data_)
        result[std::string(i->baseName())] = i->value();
    return result;
}

// Read values from a serialisable value
void ExpressionVariableVectorKeyword::deserialise(const SerialisedValue &node, const CoreData &coreData)
{
    data_.clear();
    toMap(node,
          [this](const auto &key, const auto &value) { parentNode_->addParameter(key, toml::get<ExpressionValue>(value)); });
}

/*
 * Object Management
 */
