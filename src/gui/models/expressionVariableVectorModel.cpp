// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/expressionVariableVectorModel.h"
#include "generator/node.h"

// Set source variable data
void ExpressionVariableVectorModel::setData(std::vector<std::shared_ptr<ExpressionVariable>> &variables,
                                            const ProcedureNode *parentNode)
{
    beginResetModel();
    variables_ = variables;
    parentNode_ = parentNode;
    endResetModel();
}

int ExpressionVariableVectorModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return variables_ ? variables_->get().size() : 0;
}
int ExpressionVariableVectorModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return 3;
}

Qt::ItemFlags ExpressionVariableVectorModel::flags(const QModelIndex &index) const
{
    if (index.column() == 1)
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    else
        return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable;
}

QVariant ExpressionVariableVectorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        switch (section)
        {
            case 0:
                return "Name";
            case 1:
                return "Type";
            case 2:
                return "Value";
            default:
                return {};
        }

    return {};
}

// Bond model
QVariant ExpressionVariableVectorModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !variables_)
        return {};

    auto &vars = variables_->get();

    if (index.row() >= vars.size() || index.row() < 0)
        return {};

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return {};

    auto &var = vars[index.row()];

    switch (index.column())
    {
        // Name
        case 0:
            return QString::fromStdString(std::string(var->baseName()));
        case 1:
            return var->value().type() == ExpressionValue::ValueType::Integer ? "Int" : "Real";
        case 2:
            return QString::fromStdString(var->value().asString());
        default:
            return {};
    }

    return {};
}

bool ExpressionVariableVectorModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole || !variables_ || index.column() == 1)
        return false;

    auto &var = variables_->get()[index.row()];

    if (index.column() == 0)
    {
        // Name - must check for existing var in scope with the same name
        auto p = parentNode_->getParameterInScope(value.toString().toStdString());
        if (p && p != var)
            return false;
        var->setBaseName(value.toString().toStdString());
    }
    else if (index.column() == 2)
    {
        // Value - need to check type (int vs double)
        auto varValue = value.toString().toStdString();
        bool isFloatingPoint = false;
        if (DissolveSys::isNumber(varValue, isFloatingPoint))
        {
            if (isFloatingPoint)
                var->setValue(value.toDouble());
            else
                var->setValue(value.toInt());
        }
        else
            return Messenger::error("Value '{}' provided for variable '{}' doesn't appear to be a number.\n", varValue,
                                    var->baseName());
    }

    Q_EMIT dataChanged(index, index);
    return true;
}
