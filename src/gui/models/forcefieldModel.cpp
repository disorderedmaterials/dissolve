// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/forcefieldModel.h"

ForcefieldModel::ForcefieldModel(const std::vector<std::shared_ptr<Forcefield>> &forcefields) : forcefields_(forcefields) {}

int ForcefieldModel::rowCount(const QModelIndex &parent) const {
  Q_UNUSED(parent);
  return forcefields_.size(); }

QVariant ForcefieldModel::data(const QModelIndex &index, int role) const {
  if (index.row() > forcefields_.size())
    return {};

  switch (role) {
  case Qt::DisplayRole: // Intentional Fallthrough
  case NameRole:
    return QString::fromStdString(std::string(forcefields_[index.row()]->name()));
  case DescRole:
    return QString::fromStdString(std::string(forcefields_[index.row()]->description()));
  case RawRole:
    return QVariant::fromValue(forcefields_[index.row()].get());
  }
}

QHash<int, QByteArray> ForcefieldModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[DescRole] = "description";
    roles[RawRole] = "raw";
    return roles;
}

// QVariant ForcefieldModel::headerData(int section, Qt::Orientation orientation, int role) const {
//   return {};
// }

// Qt::ItemFlags ForcefieldModel::flags(const QModelIndex &index) const {
//   return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
// }
