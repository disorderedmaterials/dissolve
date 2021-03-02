// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "main/dissolve.h"

#include <QAbstractTableModel>
#include <QModelIndex>

#include <tuple>
#include <vector>

class XmlAtomModel : public QAbstractTableModel
{
    Q_OBJECT

    private:
    std::vector<std::tuple<std::string, std::string, std::string, double, int>> atoms_;
    Dissolve &dissolve_;

    public slots:
    void readFile(const QString &);

    public:
    XmlAtomModel(Dissolve &);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
};
