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

/*
 * QAbstractItemModel overrides
 */
std::vector<std::pair<std::string, KeywordBase *>> ProcedureModel::getKeywords_() const
{
    std::vector<std::pair<std::string, KeywordBase *>> keywords;
    auto nodes = procedure_.nodes();
    for (auto n : nodes)
        for (auto g : n->keywords().displayGroups())
            for (auto k : g.second)
                keywords.emplace_back(std::pair(n->nodeTypes().keyword(n->type()), k));
    return keywords;
}

ProcedureModel::ProcedureModel(Procedure &procedure) : procedure_(procedure) {}

int ProcedureModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    // FIXME: We shouldn't just be counting the nodes but all of the
    // keywords
    return getKeywords_().size();
}

QVariant ProcedureModel::data(const QModelIndex &index, int role) const
{
    auto node = getKeywords_()[index.row()];
    switch (role)
    {
        case Qt::DisplayRole:
            return QString::fromStdString(fmt::format("{}: {}", node.first, node.second->name()));
        case Qt::UserRole:
            return QVariant::fromValue(node.second);
        default:
            return {};
    }
}

QVariant ProcedureModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return {};

    if (orientation == Qt::Horizontal)
        return "Name";

    return {};
}
