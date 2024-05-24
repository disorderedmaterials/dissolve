// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QAbstractTableModel>
#include <QModelIndex>
#include <vector>
#include "classes/speciesAtom.h"
#include "classes/atomType.h"

namespace dissolve {
  template <typename T>
  int tableColumns();

  template <typename T>
  QVariant tableHeaders(int column, Qt::Orientation orientation, int role);

  QVariant asTableRow(SpeciesAtom &item, int column, int role);
}

template <typename T>
class ReadVectorModel : public QAbstractTableModel
{
private:
  std::vector<T> &items_;

public:
    ReadVectorModel(std::vector<T> &items) : items_(items) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
      Q_UNUSED(parent);
      return items_.size();
    }

  int columnCount(const QModelIndex &parent = QModelIndex()) const override {
      Q_UNUSED(parent);
      return dissolve::tableColumns<T>();
  }

  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
    auto &item = items_[index.row()];
    return dissolve::asTableRow(item, index.column(), role);
  }

  QVariant headerData(int column, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
    return dissolve::tableHeaders<T>(column, orientation, role);
  }
};
