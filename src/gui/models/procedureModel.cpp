// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/models/procedureModel.h"
#include "classes/species.h"
#include "expression/variable.h"
#include "keywords/bool.h"
#include "keywords/expressionvariablevector.h"
#include "keywords/integer.h"
#include "keywords/node.h"
#include "keywords/nodevalue.h"
#include "keywords/nodevalueenumoptions.h"
#include "keywords/species.h"
#include "keywords/stdstring.h"
#include "keywords/vec3double.h"
#include "keywords/vec3nodevalue.h"
#include "procedure/procedure.h"
#include <functional>
#include <map>

Q_DECLARE_METATYPE(KeywordBase *);

static const std::map<std::type_index, std::function<QString(KeywordBase *)>> readers = {
    {typeid(BoolKeyword *),
     [](KeywordBase *k)
     {
         BoolKeyword *b = static_cast<BoolKeyword *>(k);
         return b->data() ? "True" : "False";
     }},
    {typeid(StringKeyword *),
     [](KeywordBase *k)
     {
         StringKeyword *s = static_cast<StringKeyword *>(k);
         return QString::fromStdString(s->data());
     }},
    {typeid(NodeValueKeyword *),
     [](KeywordBase *k)
     {
         NodeValueKeyword *n = static_cast<NodeValueKeyword *>(k);
         return QString::fromStdString(n->data().asString());
     }},
    {typeid(SpeciesKeyword *),
     [](KeywordBase *k)
     {
         SpeciesKeyword *s = static_cast<SpeciesKeyword *>(k);
         return QString::fromStdString(std::string(s->data()->name()));
     }},
    {typeid(ExpressionVariableVectorKeyword *),
     [](KeywordBase *k)
     {
         ExpressionVariableVectorKeyword *e = static_cast<ExpressionVariableVectorKeyword *>(k);
         return QString::fromStdString(joinStrings(
             e->data(), ", ", [](const auto &v) { return fmt::format("{}={}", v->name(), v->value().asString()); }));
     }},
    {typeid(Vec3DoubleKeyword *),
     [](KeywordBase *k)
     {
         Vec3DoubleKeyword *v = static_cast<Vec3DoubleKeyword *>(k);
         return QString::fromStdString(fmt::format("{},{},{}", v->data().x, v->data().y, v->data().z));
     }},
    {typeid(Vec3NodeValueKeyword *),
     [](KeywordBase *k)
     {
         Vec3NodeValueKeyword *v = static_cast<Vec3NodeValueKeyword *>(k);
         return QString::fromStdString(
             fmt::format("{},{},{}", v->data().x.asString(), v->data().y.asString(), v->data().z.asString()));
     }},
    {typeid(IntegerKeyword *), [](KeywordBase *k)
     {
         IntegerKeyword *i = static_cast<IntegerKeyword *>(k);
         return QString::number(i->data());
     }}};

/*
 * QAbstractItemModel overrides
 */
const quintptr OFFSET = 0x10000;

QString KeywordBaseToString(KeywordBase *k)
{
    auto action = readers.find(k->typeIndex());
    if (action != readers.end())
        return action->second(k);
    // if (NodeKeyword *n = dynamic_cast<NodeKeyword *>(k))
    //   return n->data()->name()
    // if (NodeValueEnumOptionsKeyword *n = dynamic_cast<NodeValueEnumOptionsKeyword *>(k))
    //   return n->value().asString();
    return QString::fromStdString(fmt::format("UKT: {}", k->typeIndex().name()));
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
                // auto type = node->type();
                return QString::fromStdString(nodes[index.row()]->nodeTypes().keyword(nodes[index.row()]->type()));
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
