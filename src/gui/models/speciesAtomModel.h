// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/speciesatom.h"
#include "main/dissolve.h"
#include <QAbstractTableModel>
#include <QModelIndex>
#include <list>

Q_DECLARE_METATYPE(SpeciesAtom *)

class SpeciesAtomModel : public QAbstractTableModel
{
    Q_OBJECT

    private:
    std::list<SpeciesAtom> &atoms_;
    Dissolve &dissolve_;

    signals:
    void updateIsotopologuesTab();

    public:
    SpeciesAtomModel(std::list<SpeciesAtom> &atoms, Dissolve &dissolve);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    void clear();
};
