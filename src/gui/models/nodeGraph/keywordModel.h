// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "keywords/store.h"
#include <QAbstractTableModel>

class KeywordModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
    KeywordModel(KeywordStore *source = nullptr);
    KeywordModel &operator=(const KeywordModel &);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    // Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const;

    private:
    KeywordStore *source_;
};

Q_DECLARE_METATYPE(KeywordModel)
