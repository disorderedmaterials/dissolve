// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include <QAbstractItemModel>

class ModelUpdater
{
    public:
    ModelUpdater(const QAbstractItemModel *);
    ~ModelUpdater();

    void connectModelSignals();

    private:
    const QAbstractItemModel *parent_{nullptr};

    private slots:
    void update();
};