// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "gui/models/nodeGraph/graphModel.h"
#include "gui/models/speciesAngleModel.h"
#include "gui/models/speciesAtomModel.h"
#include "gui/models/speciesBondModel.h"
#include "gui/models/speciesImproperModel.h"
#include "gui/models/speciesTorsionModel.h"
#include "nodes/species.h"
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
    Q_PROPERTY(SpeciesAngleModel *angles READ angles NOTIFY speciesChanged)
    Q_PROPERTY(SpeciesTorsionModel *torsions READ torsions NOTIFY speciesChanged)
    Q_PROPERTY(SpeciesImproperModel *impropers READ impropers NOTIFY speciesChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(GraphModel *graphModel READ graphModel WRITE assignModel NOTIFY speciesChanged);

    private:
    // Source Species data
    std::unique_ptr<SpeciesNode> node_;
    // Parent Graph
    GraphModel *graphModel_;
    // Atom Model
    SpeciesAtomModel atoms_;
    // Bond Model
    SpeciesBondModel bonds_;
    // Angle Model
    SpeciesAngleModel angles_;
    // Torsion Model
    SpeciesTorsionModel torsions_;
    // Improper Model
    SpeciesImproperModel impropers_;
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
    // Angle information
    SpeciesAngleModel *angles();
    // Torsion information
    SpeciesTorsionModel *torsions();
    // Improper information
    SpeciesImproperModel *impropers();
    // Set vector containing checked items
    void setCheckStateData(std::vector<const Species *> &checkedItemsVector);
    // Refresh model data
    void reset();
    GraphModel *graphModel();
    // Set the graph onto which the species will be created
    void assignModel(GraphModel *graphModel);

    Q_SIGNALS:
    // We've changed the underlying species
    void speciesChanged();
    // The name has changed
    void nameChanged();

    public Q_SLOTS:
    // Finalise the node
    void create();
    void addBond(int i, int j);
    void addAngle(int i, int j, int k);
    void addTorsion(int i, int j, int k, int l);
    void addImproper(int i, int j, int k, int l);
};
