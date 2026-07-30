// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "parameterModel.h"
#include "gui2/models/nodeGraph/enumOptionsModel.h"
#include "gui2/models/nodeGraph/enumRegistry.h"
#include "nodes/number.h"
#include "nodes/registry.h"
#include <qvariant.h>

enum Roles
{
    NAME = 0,
    DESCRIPTION,
    TYPE,
    DATA,
    MODEL,
};

ParameterModel::ParameterModel(Node::NodeParameterMap &values) : values_(values) {}

// Return number of parameters (required by QAbstractListModel)
int ParameterModel::rowCount(const QModelIndex &parent) const { return values_.size(); }

// Access edge by QModelIndex.  The correct role can be found in the roleNames function.
QVariant ParameterModel::data(const QModelIndex &index, int role) const
{
    auto it = std::next(values_.begin(), index.row());
    switch (role - Qt::UserRole)
    {
        case NAME:
            return QString::fromStdString(it->first);
        case DESCRIPTION:
            return QString::fromStdString(std::string(it->second->description()));
        case DATA:
            if (it->second->storedDataType() == typeid(Number))
                return QVariant::fromValue(it->second->get<Number>().asInteger());
            if (it->second->storedDataType() == typeid(bool))
                return QVariant::fromValue(it->second->get<bool>());
            if (it->second->storedDataType() == typeid(std::optional<Number>))
            {
                auto value = it->second->get<std::optional<Number>>();
                return value ? QVariant::fromValue(value->asInteger()) : QVariant();
            }
            if (it->second->storedDataType() == typeid(std::string))
                return QString::fromStdString(it->second->get<std::string>());
            if (EnumRegistry::hasEnumOption(it->second->storedDataType()))
                return QVariant::fromValue(it->second->getAsInt());
            return QString::fromStdString("Unrepresentable");
        case TYPE:
            if (it->second->storedDataType() == typeid(Number))
                return "number";
            if (it->second->storedDataType() == typeid(std::optional<Number>))
                return "optional number";
            if (it->second->storedDataType() == typeid(bool))
                return "bool";
            if (it->second->storedDataType() == typeid(std::string))
                return "string";
            if (EnumRegistry::hasEnumOption(it->second->storedDataType()))
                return "enum";
            if (it->second->storedDataType() == typeid(std::shared_ptr<Species>))
                return "species";

            return "unknown";
        case MODEL:
            if (EnumRegistry::hasEnumOption(it->second->storedDataType()))
                return QVariant::fromValue(EnumRegistry::options(it->second->storedDataType()).get());
            return {};

        default:
            return {};
    }
}

// Update parameter info
bool ParameterModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    auto it = std::next(values_.begin(), index.row())->second;
    if (it->storedDataType() == typeid(bool))
        it->set<bool>(value.toBool());
    if (it->storedDataType() == typeid(Number))
    {
        if (it->get<Number>().isInteger())
            it->set<Number>(value.toInt());
        else
            it->set<Number>(value.toFloat());
    }
    if (it->storedDataType() == typeid(std::optional<Number>))
    {
        if (value.toString().isNull())
            it->set<std::optional<Number>>({});
        else
        {
            auto original = it->get<std::optional<Number>>();
            if (original.has_value() && original->isInteger())
                it->set<std::optional<Number>>(value.toString().toInt());
            else
                it->set<std::optional<Number>>(value.toString().toInt());
        }
    }
    if (EnumRegistry::hasEnumOption(it->storedDataType()))
        it->setFromInt(value.toInt());
    if (it->storedDataType() == typeid(std::shared_ptr<Species>))
        it->set<std::shared_ptr<Species>>(qvariant_cast<std::shared_ptr<Species>>(value));
    return true;
}

// Return the mapping between role index and QML value name.  This is required by QAbstractListModel
QHash<int, QByteArray> ParameterModel::roleNames() const
{
    QHash<int, QByteArray> result;
    result[Qt::UserRole + (int)NAME] = "name";
    result[Qt::UserRole + (int)DESCRIPTION] = "description";
    result[Qt::UserRole + (int)DATA] = "param";
    result[Qt::UserRole + (int)TYPE] = "type";
    result[Qt::UserRole + (int)MODEL] = "innerModel";
    return result;
}
