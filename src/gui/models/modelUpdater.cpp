// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/modelUpdater.h"
#include <QAbstractItemModel>

void ModelUpdater::setModel(const QAbstractItemModel *model) { model = model; }

void ModelUpdater::connectModelSignals()
{
    connect(model, &QAbstractItemModel::columnsAboutToBeInserted, this, &update);
    connect(model, &QAbstractItemModel::columnsAboutToBeMoved, this, &update);
    connect(model, &QAbstractItemModel::columnsAboutToBeRemoved, this, &update);
    connect(model, &QAbstractItemModel::columnsInserted, this, &update);
    connect(model, &QAbstractItemModel::columnsMoved, this, &update);
    connect(model, &QAbstractItemModel::columnsRemoved, this, &update);
    connect(model, &QAbstractItemModel::dataChanged, this, &update);
    connect(model, &QAbstractItemModel::headerDataChanged, this, &update);
    connect(model, &QAbstractItemModel::layoutAboutToBeChanged, this, &update);
    connect(model, &QAbstractItemModel::layoutChanged, this, &update);
    connect(model, &QAbstractItemModel::modelAboutToBeReset, this, &update);
    connect(model, &QAbstractItemModel::modelReset, this, &update);
    connect(model, &QAbstractItemModel::rowsABoutToBeInserted, this, &update);
    connect(model, &QAbstractItemModel::rowsABoutToBeMoved, this, &update);
    connect(model, &QAbstractItemModel::rowsABoutToBeRemoved, this, &update);
    connect(model, &QAbstractItemModel::rowsInserted, this, &update);
    connect(model, &QAbstractItemModel::rowsMoved, this, &update);
    connect(model, &QAbstractItemModel::rowsRemoved, this, &update);
}

void ModelUpdater::update() { emit model->modelsUpdated(); }