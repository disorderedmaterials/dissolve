// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/modelUpdater.h"
#include <QAbstractItemModel>

ModelUpdater::setModel(const QAbstractItemModel *model) { model = model; }

void ModelUpdater::connectModelSignals()
{
    connect(model, &columnsAboutToBeInserted, this, &update);
    connect(model, &columnsAboutToBeMoved, this, &update);
    connect(model, &columnsAboutToBeRemoved, this, &update);
    connect(model, &columnsInserted, this, &update);
    connect(model, &columnsMoved, this, &update);
    connect(model, &columnsRemoved, this, &update);
    connect(model, &dataChanged, this, &update);
    connect(model, &headerDataChanged, this, &update);
    connect(model, &layoutAboutToBeChanged, this, &update);
    connect(model, &layoutChanged, this, &update);
    connect(model, &modelAboutToBeReset, this, &update);
    connect(model, &modelReset, this, &update);
    connect(model, &rowsABoutToBeInserted, this, &update);
    connect(model, &rowsABoutToBeMoved, this, &update);
    connect(model, &rowsABoutToBeRemoved, this, &update);
    connect(model, &rowsInserted, this, &update);
    connect(model, &rowsMoved, this, &update);
    connect(model, &rowsRemoved, this, &update);
}

void ModelUpdater::update() { emit model->modelsUpdated(); }