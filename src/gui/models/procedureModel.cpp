// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "procedure/procedure.h"
#include "gui/models/procedureModel.h"


/*
 * QAbstractItemModel overrides
 */
const quintptr OFFSET = 0x10000;

ProcedureModel::ProcedureModel(Procedure &procedure) : procedure_(procedure) {}

QModelIndex ProcedureModel::index(int row, int column, const QModelIndex& parent) const {
    quintptr child;
    if (!parent.isValid())
      child = 0;
    else if (!parent.parent().isValid())
        child = parent.row() + 1;
    else
      child = OFFSET * (parent.row() + 1) + parent.parent().row() + 1;

    return createIndex(row, column, child);
}

QModelIndex ProcedureModel::parent(const QModelIndex& index) const {
  quintptr root = 0;
  if (index.internalId() == 0)
        return {};
  if (index.internalId() < OFFSET)
    return createIndex(index.internalId() - 1, 0, root);
  return createIndex(index.internalId() / OFFSET - 1, 0, index.internalId() % OFFSET);
}

bool ProcedureModel::hasChildren(const QModelIndex &parent) const {
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
      return 1;
    return 1;
    return nodes[parent.parent().row()]->keywords().keywords().size();
}

QVariant ProcedureModel::data(const QModelIndex &index, int role) const
{
  if (role != Qt::DisplayRole) return {};
  if (!index.parent().isValid())
    return  "Node";
  else if (!index.parent().parent().isValid())
    return "Group";
  else return "Param";

    auto nodes = procedure_.nodes();

    if (!index.parent().isValid()) {
    if (role == Qt::DisplayRole)
      switch (index.column()) {
      case 0:
	return QString::fromStdString(std::string(nodes[index.row()]->name()));
      case 1:
	return QString::fromStdString("Type");
      default:
	return {};
      }
    else if (role == Qt::UserRole)
      return QVariant::fromValue(nodes[index.row()]);
    }

    if (!index.parent().parent().isValid()) {
      if (role == Qt::DisplayRole)
	return "Quux";
      return {};
    }

    Messenger::print("{}", index.parent().parent().row());
    if (index.parent().parent().row() < 0) return {};

    auto keywords = nodes[index.parent().parent().row()]->keywords().keywords();
    if (role == Qt::DisplayRole)
      switch (index.column()) {
      case 0:
	return QString::fromStdString(std::string((std::next(keywords.begin(), index.row())->first)));
      case 1:
	return QString::fromStdString("Type");
      default:
	return {};
      }

    return {};
}

bool ProcedureModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

Qt::ItemFlags ProcedureModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

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
