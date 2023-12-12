// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include <QAbstractItemModel>
#include <QObject>

class ModelUpdater : public QObject
{

    Q_OBJECT

    public:
    ModelUpdater() = default;
    ~ModelUpdater();

    void connectModelSignals();

    void setModel(QAbstractItemModel *);

    private:
    QAbstractItemModel *model;

    private slots:
    void update();
};