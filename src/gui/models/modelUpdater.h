// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <QAbstractItemModel>
#include <QObject>
#include <variant>

class ModelUpdater : public QObject
{

    Q_OBJECT

    public:
    ModelUpdater() = default;
    ~ModelUpdater();

    void connectModelSignals();

    void setModel(QAbstractItemModel *);

    private:
    QAbstractItemModel *model_;

    private slots:
    void update();
};