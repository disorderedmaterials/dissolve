// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/models/sortFilterProxy.h"

SortFilterProxy::SortFilterProxy(QObject *parent) : QSortFilterProxyModel(parent) {}

void SortFilterProxy::setModel(const QAbstractItemModel *model) { setSourceModel(const_cast<QAbstractItemModel *>(model)); }
