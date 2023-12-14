// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/modelUpdater.h"
#include <QAbstractItemModel>
#include <QAbstractListModel>
#include <QMetaObject>
#include <Qt>

void ModelUpdater::setModel(QAbstractItemModel *model) { model = model; }

ModelUpdater::~ModelUpdater() {}

void ModelUpdater::connectModelSignals()
{
    connect(model, &QAbstractItemModel::columnsAboutToBeInserted, this, &ModelUpdater::update);
    connect(model, &QAbstractItemModel::columnsAboutToBeMoved, this, &ModelUpdater::update);
    connect(model, &QAbstractItemModel::columnsAboutToBeRemoved, this, &ModelUpdater::update);
    connect(model, &QAbstractItemModel::columnsInserted, this, &ModelUpdater::update);
    connect(model, &QAbstractItemModel::columnsMoved, this, &ModelUpdater::update);
    connect(model, &QAbstractItemModel::columnsRemoved, this, &ModelUpdater::update);
    connect(model, &QAbstractItemModel::dataChanged, this, &ModelUpdater::update);
    connect(model, &QAbstractItemModel::headerDataChanged, this, &ModelUpdater::update);
    connect(model, &QAbstractItemModel::layoutAboutToBeChanged, this, &ModelUpdater::update);
    connect(model, &QAbstractItemModel::layoutChanged, this, &ModelUpdater::update);
    connect(model, &QAbstractItemModel::modelAboutToBeReset, this, &ModelUpdater::update);
    connect(model, &QAbstractItemModel::modelReset, this, &ModelUpdater::update);
    connect(model, &QAbstractListModel::rowsAboutToBeInserted, this, &ModelUpdater::update);
    connect(model, &QAbstractListModel::rowsAboutToBeMoved, this, &ModelUpdater::update);
    connect(model, &QAbstractListModel::rowsAboutToBeRemoved, this, &ModelUpdater::update);
    connect(model, &QAbstractItemModel::rowsInserted, this, &ModelUpdater::update);
    connect(model, &QAbstractItemModel::rowsMoved, this, &ModelUpdater::update);
    connect(model, &QAbstractItemModel::rowsRemoved, this, &ModelUpdater::update);
}

void ModelUpdater::update()
{
    std::visit([](auto &m){ emit m->modelsUpdated(); }, model);
}