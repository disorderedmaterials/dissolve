// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/models/procedureModel.h"
#include "classes/species.h"
#include "expression/variable.h"
#include "keywords/bool.h"
#include "keywords/enumoptions.h"
#include "keywords/expressionvariablevector.h"
#include "keywords/integer.h"
#include "keywords/node.h"
#include "keywords/nodevalue.h"
#include "keywords/nodevalueenumoptions.h"
#include "keywords/species.h"
#include "keywords/stdstring.h"
#include "keywords/vec3nodevalue.h"
#include "procedure/procedure.h"
#include "templates/algorithms.h"

Q_DECLARE_METATYPE(KeywordBase *);

/*
 * QAbstractItemModel overrides
 */
const quintptr OFFSET = 0x10000;

QString KeywordBaseToString(KeywordBase *k)
{
    if (auto *b = dynamic_cast<BoolKeyword *>(k))
    {
        return b->data() ? "True" : "False";
    }
    if (auto *i = dynamic_cast<IntegerKeyword *>(k))
    {
        return QString::fromStdString(fmt::format("{}", i->data()));
    }
    if (auto *e = dynamic_cast<EnumOptionsBaseKeyword *>(k))
    {
        return "Enum Options Template Problem";
    }
    if (auto *e = dynamic_cast<ExpressionVariableVectorKeyword *>(k))
    {
        std::vector<std::string> names;
        std::transform(e->data().begin(), e->data().end(), std::back_inserter(names),
                       [](auto &x) { return std::string(x->name()); });
        return QString::fromStdString(joinStrings(names));
    }
    if (auto *i = dynamic_cast<IntegerKeyword *>(k))
    {
        return QString::fromStdString(fmt::format("{}", i->data()));
    }
    if (auto *n = dynamic_cast<NodeKeywordBase *>(k))
    {
        return "Node Keyword Template Problem";
    }
    if (auto *n = dynamic_cast<NodeValueEnumOptionsBaseKeyword *>(k))
    {
        return "Node Value Enum Options Template Problem";
    }
    if (auto *n = dynamic_cast<NodeValueKeyword *>(k))
    {
        return QString::fromStdString(n->data().asString());
    }
    if (auto *s = dynamic_cast<SpeciesKeyword *>(k))
    {
        return QString::fromStdString(std::string(s->data()->name()));
    }
    if (auto *s = dynamic_cast<StringKeyword *>(k))
    {
        return QString::fromStdString(s->data());
    }
    if (auto *v = dynamic_cast<Vec3NodeValueKeyword *>(k))
    {
        return QString::fromStdString(
            fmt::format("{},{},{}", v->data().x.asString(), v->data().y.asString(), v->data().z.asString()));
    }

    return k->typeIndex().name();
}

ProcedureModel::ProcedureModel(Procedure &procedure) : procedure_(procedure)
{
    QMetaType::registerConverter<KeywordBase *, QString>(KeywordBaseToString);
}

QModelIndex ProcedureModel::index(int row, int column, const QModelIndex &parent) const
{
    quintptr child;
    if (!parent.isValid())
        child = 0;
    else if (!parent.parent().isValid())
        child = parent.row() + 1;
    else
        child = OFFSET * (parent.row() + 1) + parent.parent().row() + 1;

    return createIndex(row, column, child);
}

QModelIndex ProcedureModel::parent(const QModelIndex &index) const
{
    quintptr root = 0;
    if (index.internalId() == 0)
        return {};
    if (index.internalId() < OFFSET)
        return createIndex(index.internalId() - 1, 0, root);
    return createIndex(index.internalId() / OFFSET - 1, 0, index.internalId() % OFFSET);
}

bool ProcedureModel::hasChildren(const QModelIndex &parent) const
{
    return !parent.internalId() || !parent.parent().internalId();
}

int ProcedureModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

int ProcedureModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    auto nodes = procedure_.nodes();
    if (!parent.isValid())
        return nodes.size();
    if (!parent.parent().isValid())
    {
        auto groups = nodes[parent.row()]->keywords().displayGroups();
        return groups.size();
    }
    auto groups = nodes[parent.parent().row()]->keywords().displayGroups();
    return groups[parent.row()].second.size();
}

QVariant ProcedureModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole)
        return {};
    auto nodes = procedure_.nodes();
    if (!index.parent().isValid())
    {
        switch (index.column())
        {
            case 0:
                return QString::fromStdString(std::string(nodes[index.row()]->name()));
            default:
                return {};
        }
    }
    else if (!index.parent().parent().isValid())
    {
        auto groups = nodes[index.parent().row()]->keywords().displayGroups();

        switch (index.column())
        {
            case 0:
                return QString::fromStdString(std::string(groups[index.row()].first));
            default:
                return {};
        }
    }
    else
    {
        auto groups = nodes[index.parent().parent().row()]->keywords().displayGroups();
        auto keyword = groups[index.parent().row()].second[index.row()];

        switch (index.column())
        {
            case 0:
                return QString::fromStdString(std::string(keyword->name()));
            case 1:
                return QVariant::fromValue(keyword);
            default:
                return {};
        }
    }

    return {};
}

bool ProcedureModel::setData(const QModelIndex &index, const QVariant &value, int role) { return false; }

Qt::ItemFlags ProcedureModel::flags(const QModelIndex &index) const { return Qt::ItemIsSelectable | Qt::ItemIsEnabled; }

QVariant ProcedureModel::headerData(int section, Qt::Orientation orientation, int role) const
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
            default:
                return {};
        }

    return {};
}
