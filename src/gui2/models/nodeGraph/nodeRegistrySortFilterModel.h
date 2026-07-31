// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "gui2/models/nodeGraph/nodeRegistryModel.h"
#include <QObject>
#include <QSortFilterProxyModel>

class NodeRegistrySortFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT

    public:
    Q_PROPERTY(QString searchText READ searchText WRITE setSearchText NOTIFY searchTextChanged)
    NodeRegistrySortFilterModel(QObject *parent = nullptr);

    private:
    QString searchText_;

    QString &searchText();
    void setSearchText(QString &text);

    Q_SIGNALS:
    // Notify if search text has changed
    void searchTextChanged();

    protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &source) const override;
};