// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include <QAbstractTableModel>
#include <QModelIndex>

class Species;

class SpeciesAtomModel : public QAbstractTableModel
{
    Q_OBJECT

    private:
    std::shared_ptr<Species> species_;

    Q_SIGNALS:
    void atomTypeChanged();

    public Q_SLOTS:
    void addAtom(QString element, double x, double y, double z, double charge);

    public:
    SpeciesAtomModel();

    void setSpecies(std::shared_ptr<Species> species);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    void clear();
};
