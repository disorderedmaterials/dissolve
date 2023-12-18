// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/models/modelUpdater.h"
#include <QAbstractItemModel>
#include <QAbstractListModel>
#include <QMetaObject>
#include <Qt>

void ModelUpdater::setModel(QAbstractItemModel *model) { model_ = model; }

ModelUpdater::~ModelUpdater() {}

void ModelUpdater::connectModelSignals()
{
    connect(model_, &QAbstractItemModel::columnsAboutToBeInserted, this, &ModelUpdater::update);
    connect(model_, &QAbstractItemModel::columnsAboutToBeMoved, this, &ModelUpdater::update);
    connect(model_, &QAbstractItemModel::columnsAboutToBeRemoved, this, &ModelUpdater::update);
    connect(model_, &QAbstractItemModel::columnsInserted, this, &ModelUpdater::update);
    connect(model_, &QAbstractItemModel::columnsMoved, this, &ModelUpdater::update);
    connect(model_, &QAbstractItemModel::columnsRemoved, this, &ModelUpdater::update);
    connect(model_, &QAbstractItemModel::dataChanged, this, &ModelUpdater::update);
    connect(model_, &QAbstractItemModel::headerDataChanged, this, &ModelUpdater::update);
    connect(model_, &QAbstractItemModel::layoutAboutToBeChanged, this, &ModelUpdater::update);
    connect(model_, &QAbstractItemModel::layoutChanged, this, &ModelUpdater::update);
    connect(model_, &QAbstractItemModel::modelAboutToBeReset, this, &ModelUpdater::update);
    connect(model_, &QAbstractItemModel::modelReset, this, &ModelUpdater::update);
    connect(model_, &QAbstractListModel::rowsAboutToBeInserted, this, &ModelUpdater::update);
    connect(model_, &QAbstractListModel::rowsAboutToBeMoved, this, &ModelUpdater::update);
    connect(model_, &QAbstractListModel::rowsAboutToBeRemoved, this, &ModelUpdater::update);
    connect(model_, &QAbstractItemModel::rowsInserted, this, &ModelUpdater::update);
    connect(model_, &QAbstractItemModel::rowsMoved, this, &ModelUpdater::update);
    connect(model_, &QAbstractItemModel::rowsRemoved, this, &ModelUpdater::update);
}

void ModelUpdater::update()
{
    QObject *object = dynamic_cast<QObject *>(model_);
    QMetaObject::invokeMethod(object, "modelsUpdated", Qt::DirectConnection);
}
