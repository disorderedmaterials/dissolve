// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include <QAbstractItemModel>
#include <QObject>

class ModelUpdater : public QBject
{

    Q_OBJECT

    public:
    ModelUpdater() = default;
    ~ModelUpdater();

    void connectModelSignals();

    void setModel(const QAbstractItemModel *);

    private:
    const QAbstractItemModel *model;

    private slots:
    void update();
};