// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QSortFilterProxyModel>

class SortFilterProxy : public QSortFilterProxyModel {
  Q_OBJECT
  // The model to proxy
  Q_PROPERTY(const QAbstractItemModel *model WRITE setModel NOTIFY read)

public:
  SortFilterProxy(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {}
  void setModel(const QAbstractItemModel *model) {setSourceModel(const_cast<QAbstractItemModel *>(model));}

};
