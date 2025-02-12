// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "keywordModel.h"
#include "base/units.h"
#include "classes/species.h"
#include "expression/variable.h"
#include "generator/addBase.h"
#include "generator/coordinateSets.h"
#include "keywords/bool.h"
#include "keywords/enumOptions.h"
#include "keywords/expressionVariableVector.h"
#include "keywords/node.h"
#include "keywords/nodeValue.h"
#include "keywords/nodeValueEnumOptions.h"
#include "keywords/species.h"
#include "keywords/vec3NodeValue.h"

KeywordModel::KeywordModel(KeywordStore *source) : source_(source) {}

KeywordModel &KeywordModel::operator=(const KeywordModel &other)
{
    source_ = other.source_;
    return *this;
}

int KeywordModel::rowCount(const QModelIndex &parent) const
{
    if (!source_)
        return 0;
    return source_->nVisibleKeywords();
}

int KeywordModel::columnCount(const QModelIndex &parent) const
{
    if (!source_)
        return 0;
    return 2;
}

QVariant KeywordModel::data(const QModelIndex &index, int role) const
{
    const int row = index.row();

    auto [keyword, type] = source_->at(row);
    auto typeIndex = keyword->typeIndex();

    // Return delegate type
    if (role == Qt::UserRole)
    {
        if (index.column() == 0)
            return "header";
        if (typeIndex == std::type_index(typeid(BoolKeyword *)))
            return "bool";
        if (typeIndex == std::type_index(typeid(NodeValueKeyword *)))
            return "string";
        return "default";
    }

    if (index.column() == 0)
        switch (role)
        {
            case Qt::DisplayRole:
                return QString::fromStdString(std::string(keyword->name()));
            case Qt::ToolTipRole:
                return QString::fromStdString(std::string(keyword->description()));
            default:
                return "Unknown Role";
        }

    switch (role)
    {
        case Qt::CheckStateRole:
            if (typeIndex == std::type_index(typeid(BoolKeyword *)))
            {
                auto boolKeyword = static_cast<BoolKeyword *>(keyword);
                return QVariant(boolKeyword->data());
            }
            return QVariant(false);
        case Qt::DisplayRole:
        {
            if (typeIndex == std::type_index(typeid(BoolKeyword *)))
            {
                auto boolKeyword = static_cast<BoolKeyword *>(keyword);
                return boolKeyword->data();
            }
            else if (typeIndex == std::type_index(typeid(SpeciesKeyword *)))
            {
                auto speciesKeyword = static_cast<SpeciesKeyword *>(keyword);
                return QString::fromStdString(std::string(speciesKeyword->data()->name()));
            }
            else if (typeIndex == std::type_index(typeid(NodeValueKeyword *)))
            {
                auto node = static_cast<NodeValueKeyword *>(keyword);
                return QString::fromStdString(node->data().asString());
            }
            else if (typeIndex == std::type_index(typeid(EnumOptionsBaseKeyword *)))
            {
                auto value = dynamic_cast<EnumOptionsKeyword<AddGeneratorNodeBase::PositioningType> *>(keyword);
                // FIXME: I need a way of accessing the inner type of the EnumOptionsKeys
                if (!value)
                    return "Bad cast";
                return QString::fromStdString(AddGeneratorNodeBase::positioningTypes().keyword(value->data()));
            }
            else if (typeIndex == std::type_index(typeid(ExpressionVariableVectorKeyword *)))
            {
                auto data = static_cast<ExpressionVariableVectorKeyword *>(keyword)->data();
                return QString::fromStdString(std::format("{} = {}", data[0]->name(), data[0]->value().asString()));
            }
            else if (typeIndex == std::type_index(typeid(NodeValueEnumOptionsBaseKeyword *)))
            {
                auto node = dynamic_cast<NodeValueEnumOptionsKeyword<Units::DensityUnits> *>(keyword);
                if (!node)
                    return "Bad cast";
                auto magnitude = node->value().asString();
                auto units = Units::densityUnits().keywordByIndex(node->enumerationIndex());
                return QString::fromStdString(std::format("{} ({})", magnitude, units));
            }
            else if (typeIndex == std::type_index(typeid(NodeKeywordBase *)))
            {
                auto node = dynamic_cast<NodeKeyword<CoordinateSetsGeneratorNode> *>(keyword);
                if (!node)
                    return "Bad cast";
                if (!node->data())
                    return "Null pointer";
                return QString::fromStdString(std::string(node->data()->name()));
            }
            else if (typeIndex == std::type_index(typeid(Vec3NodeValueKeyword *)))
            {
                auto vec3 = static_cast<Vec3NodeValueKeyword *>(keyword)->data();
                auto x = vec3.x.asString();
                auto y = vec3.y.asString();
                auto z = vec3.z.asString();
                return QString::fromStdString(std::format("{}, {}, {}", x, y, z));
            }
            else
                return QString::fromStdString(typeIndex.name());
        }
        case Qt::ToolTipRole:
            return {};
        default:
            return "Unknown Role";
    }
    // return QString::fromStdString(std::string(keyword.first->name()));
}

bool KeywordModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole)
    {
        const int row = index.row();

        auto [keyword, type] = source_->at(row);
        auto typeIndex = keyword->typeIndex();

        if (typeIndex == std::type_index(typeid(BoolKeyword *)))
        {
            auto boolKeyword = dynamic_cast<BoolKeyword *>(keyword);
            if (!boolKeyword)
                return false;
            boolKeyword->setData(value.toBool());
            Q_EMIT dataChanged(index, index);
            return true;
        }
    }
    if (role == Qt::DisplayRole)
    {
        const int row = index.row();

        auto [keyword, type] = source_->at(row);
        auto typeIndex = keyword->typeIndex();

        if (typeIndex == std::type_index(typeid(NodeValueKeyword *)))
        {
            auto node = dynamic_cast<NodeValueKeyword *>(keyword);
            if (!node)
                return false;
            if (value.canConvert<int>())
                node->setData(NodeValue(value.toInt()));
            else if (value.canConvert<double>())
                node->setData(NodeValue(value.toDouble()));
            else
                node->setData(NodeValue(value.toString().toStdString()));
            Q_EMIT dataChanged(index, index);
            return true;
        }
    }
    return false;
}

// Qt::ItemFlags KeywordModel::flags(const QModelIndex &index) const override;

QVariant KeywordModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (section)
    {
        case 0:
            return "Name";
        case 1:
            return "Value";
        default:
            return "ERROR";
    }
}

QHash<int, QByteArray> KeywordModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[Qt::DisplayRole] = "display";
    roles[Qt::ToolTipRole] = "tooltip";
    roles[Qt::CheckStateRole] = "checkedState";
    roles[Qt::UserRole] = "type";
    return roles;
}

Qt::ItemFlags KeywordModel::flags(const QModelIndex &index) const
{
    const int row = index.row();
    if (index.column() == 0)
        return Qt::NoItemFlags;

    return Qt::ItemIsUserCheckable | Qt::ItemIsEditable;
}
