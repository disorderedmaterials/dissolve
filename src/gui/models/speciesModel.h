// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "classes/species.h"
#include "gui/models/speciesAtomModel.h"
#include "gui/models/speciesBondModel.h"
#include <QAbstractTableModel>
#include <QModelIndex>

#include <qtmetamacros.h>
#include <vector>

class SpeciesModel : public QObject
{
    Q_OBJECT
    // The Species being modelled
    Q_PROPERTY(SpeciesAtomModel *atoms READ atoms NOTIFY speciesChanged)
    Q_PROPERTY(SpeciesBondModel *bonds READ bonds NOTIFY speciesChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    private:
    // Source Species data
    std::unique_ptr<Species> species_;
    // Atom Model
    SpeciesAtomModel atoms_;
    // Bond Model
    SpeciesBondModel bonds_;
    // Return object represented by specified model index
    const Species *rawData(const QModelIndex &index) const;

    public:
    enum class SpeciesUserRole
    {
        RawData = Qt::UserRole,
        BondsCount,
        AnglesCount,
        TorsionsCount,
        ImpropersCount
    };
    Q_ENUM(SpeciesUserRole);

    public:
    SpeciesModel();
    // Species name
    QString name();
    // Species name
    void setName(QString name);
    // Atom information
    SpeciesAtomModel *atoms();
    // Bond information
    SpeciesBondModel *bonds();
    // Set vector containing checked items
    void setCheckStateData(std::vector<const Species *> &checkedItemsVector);
    // Refresh model data
    void reset();

    Q_SIGNALS:
    // We've changed the underlying species
    void speciesChanged();
    // The name has changed
    void nameChanged();

    public Q_SLOTS:
    void create(QVariant graphModel);
    void addBond(int i, int j);
};
