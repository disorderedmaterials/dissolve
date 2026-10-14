// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "parameterModel.h"
#include "gui2/models/enumOptionsModel.h"
#include "gui2/models/enumRegistry.h"
#include "math/rangedVector3.h"
#include "math/vector3.h"
#include "nodes/graph.h"
#include "nodes/inputs.h"
#include "nodes/number.h"
#include "nodes/outputs.h"
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

// Returns the reference to the parameters from the model
Node::NodeParameterMap &ParameterModel::values() { return values_; }
const Node::NodeParameterMap &ParameterModel::values() const { return values_; }

// Reset parameter model
void ParameterModel::resetParameters()
{
    beginResetModel();
    endResetModel();
}

// Rename a parameter
void ParameterModel::rename(const std::string &currentName, const std::string &newName)
{
    // TODO: This could do with being wrapped in a begin/end reset model/insert rows
    // *if* we defer the onItemAdded callback in the GraphDelegate, probably using a "renameInProgress" flag
    // therefore preventing the call to GraphModel::replaceTargetEndPoints while the endPoints are not in their final state.
    auto row = int(std::distance(values_.begin(), values_.find(currentName)));
    auto element = values_.extract(currentName);

    Graph::NodeParameterMap newMap = values_;
    newMap.erase(element.key());
    newMap.insert({newName, element.mapped()});
    row = int(std::distance(newMap.begin(), newMap.find(newName)));

    values_ = newMap;
}

/*
 * QAbstractListModel overrides
 */

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
            {
                const auto n = it->second->get<Number>();
                return QVariant::fromValue(n.isInteger() ? n.asInteger() : n.asDouble());
            }
            if (it->second->storedDataType() == typeid(bool))
                return QVariant::fromValue(it->second->get<bool>());
            if (it->second->storedDataType() == typeid(std::optional<Number>))
            {
                auto value = it->second->get<std::optional<Number>>();
                return value ? QVariant::fromValue(value->asInteger()) : QVariant();
            }
            if (it->second->storedDataType() == typeid(std::string))
                return QString::fromStdString(it->second->get<std::string>());
            if (it->second->storedDataType() == typeid(std::filesystem::path))
                return QString::fromStdString(it->second->get<std::filesystem::path>().string());
            if (it->second->storedDataType() == typeid(Vector3))
            {
                QVariantList vector;
                auto item = it->second->get<Vector3>();
                vector.push_back(item.x);
                vector.push_back(item.y);
                vector.push_back(item.z);
                return vector;
            }
            if (it->second->storedDataType() == typeid(RangedVector3))
            {
                QVariantList rangedVector;
                QVariantList min;
                QVariantList value;
                QVariantList max;
                auto item = it->second->get<RangedVector3>();
                if (item.hasLowerBound())
                {
                    min.push_back(item.min()->x);
                    min.push_back(item.min()->y);
                    min.push_back(item.min()->z);
                }
                if (item.hasUpperBound())
                {
                    max.push_back(item.max()->x);
                    max.push_back(item.max()->y);
                    max.push_back(item.max()->z);
                }
                value.push_back(item.value().x);
                value.push_back(item.value().y);
                value.push_back(item.value().z);
                rangedVector.push_back(min);
                rangedVector.push_back(value);
                rangedVector.push_back(max);
                return rangedVector;
            }
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
            if (it->second->storedDataType() == typeid(std::filesystem::path))
                return "file path";
            if (it->second->storedDataType() == typeid(Vector3))
                return "vector3";
            if (it->second->storedDataType() == typeid(RangedVector3))
                return "ranged vector3";
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
    if (it->storedDataType() == typeid(std::filesystem::path))
    {
        auto path = value.toString().toStdString();
        it->set<std::filesystem::path>(std::filesystem::path(path));
    }
    if (it->storedDataType() == typeid(Vector3))
    {
        auto vector = qvariant_cast<QVariantList>(value);
        auto x = qvariant_cast<double>(vector[0]);
        auto y = qvariant_cast<double>(vector[1]);
        auto z = qvariant_cast<double>(vector[2]);
        it->set<Vector3>({x, y, z});
    }
    if (it->storedDataType() == typeid(RangedVector3))
    {
        auto rangedVector = qvariant_cast<QVariantList>(value);
        auto min = qvariant_cast<QVariantList>(rangedVector[0]);
        auto value = qvariant_cast<QVariantList>(rangedVector[1]);
        auto max = qvariant_cast<QVariantList>(rangedVector[1]);
        auto x = qvariant_cast<double>(value[0]);
        auto y = qvariant_cast<double>(value[1]);
        auto z = qvariant_cast<double>(value[2]);

        if (min.empty() && max.empty())
            it->set<RangedVector3>(Vector3{x, y, z});
        else
        {
            auto minX = qvariant_cast<double>(min[0]);
            auto minY = qvariant_cast<double>(min[1]);
            auto minZ = qvariant_cast<double>(min[2]);
            auto maxX = qvariant_cast<double>(max[0]);
            auto maxY = qvariant_cast<double>(max[1]);
            auto maxZ = qvariant_cast<double>(max[2]);
            auto optMin = std::optional<Vector3>();
            optMin.emplace(minX, minY, minZ);
            auto optMax = std::optional<Vector3>();
            optMax.emplace(maxX, maxY, maxZ);
            it->set<RangedVector3>(RangedVector3{Vector3{x, y, z}, optMin, optMax});
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
