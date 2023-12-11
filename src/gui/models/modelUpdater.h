// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include <QAbstractItemModel>

class ModelUpdater
{
    Q_OBJECT

    public:
    ModelUpdater(const QAbstractItemModel);
    ~ModelUpdater();

    void connectModelSignals();

    signals:
    void modelsUpdated();

    private slots:
    void update();
};