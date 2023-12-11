// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/modelUpdater.h"
#include <QAbstractItemModel>

ModelUpdater::ModelUpdater(const QAbstractItemModel *parent) : parent_(parent) {}

void ModelUpdater::connectModelSignals()
{
    connect(parent_, &columnsAboutToBeInserted, this, &update);
    connect(parent_, &columnsAboutToBeMoved, this, &update);
    connect(parent_, &columnsAboutToBeRemoved, this, &update);
    connect(parent_, &columnsInserted, this, &update);
    connect(parent_, &columnsMoved, this, &update);
    connect(parent_, &columnsRemoved, this, &update);
    connect(parent_, &dataChanged, this, &update);
    connect(parent_, &headerDataChanged, this, &update);
    connect(parent_, &layoutAboutToBeChanged, this, &update);
    connect(parent_, &layoutChanged, this, &update);
    connect(parent_, &modelAboutToBeReset, this, &update);
    connect(parent_, &modelReset, this, &update);
    connect(parent_, &rowsABoutToBeInserted, this, &update);
    connect(parent_, &rowsABoutToBeMoved, this, &update);
    connect(parent_, &rowsABoutToBeRemoved, this, &update);
    connect(parent_, &rowsInserted, this, &update);
    connect(parent_, &rowsMoved, this, &update);
    connect(parent_, &rowsRemoved, this, &update);
}

void ModelUpdater::update() { emit parent_->modelsUpdated(); }