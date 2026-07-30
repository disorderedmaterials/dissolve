// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "data/ff/ff.h"
#include "gui2/models/nodeGraph/graphModel.h"
#include <QObject>
#include <QStringList>

class SimpleForcefieldModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY changed());
    Q_PROPERTY(QStringList library READ library);
    Q_PROPERTY(GraphModel *graphModel READ graphModel WRITE assignModel);

    private:
    // Chosen forcefield
    std::shared_ptr<Forcefield> ff_;
    // Parent Graph
    GraphModel *graphModel_;

    public:
    // Forcefield name
    QString name();
    // Select forcefield
    void setName(QString name);
    // List of known forcefields
    QStringList library();
    // The graph onto which the species will be created
    GraphModel *graphModel();
    // Set the graph onto which the species will be created
    void assignModel(GraphModel *graphModel);

    Q_SIGNALS:
    void changed();

    public Q_SLOTS:
    // Finalise the node
    void create(int x, int y);
};
