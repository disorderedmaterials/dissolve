// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/modelUpdater.h"
#include <QAbstractItemModel>

void ModelUpdater::connectModelSignals()
{
    connect(this, &columnsAboutToBeInserted, this, &update);
    connect(this, &columnsAboutToBeMoved, this, &update);
    connect(this, &columnsAboutToBeRemoved, this, &update);
    connect(this, &columnsInserted, this, &update);
    connect(this, &columnsMoved, this, &update);
    connect(this, &columnsRemoved, this, &update);
    connect(this, &dataChanged, this, &update);
    connect(this, &headerDataChanged, this, &update);
    connect(this, &layoutAboutToBeChanged, this, &update);
    connect(this, &layoutChanged, this, &update);
    connect(this, &modelAboutToBeReset, this, &update);
    connect(this, &modelReset, this, &update);
    connect(this, &rowsABoutToBeInserted, this, &update);
    connect(this, &rowsABoutToBeMoved, this, &update);
    connect(this, &rowsABoutToBeRemoved, this, &update);
    connect(this, &rowsInserted, this, &update);
    connect(this, &rowsMoved, this, &update);
    connect(this, &rowsRemoved, this, &update);
}

void ModelUpdater::update() { emit modelsUpdated(); }