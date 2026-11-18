// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "gui2/models/legacy/atomTypeModel.h"
#include "gui2/models/legacy/commonTermTreeModel.h"
#include "gui2/models/legacy/configurationModel.h"
#include "main/dissolve.h"
#include "nodes/graph.h"
#include "templates/optionalRef.h"
#include <QUrl>
#include <memory>

class DissolveModel : public QObject
{
    Q_OBJECT
    // File to load
    Q_PROPERTY(QUrl file READ fileName WRITE loadInput NOTIFY modelsUpdated)
    // The main graph
    Q_PROPERTY(Graph *graph READ graph NOTIFY modelsUpdated)

    public:
    DissolveModel() = default;
    ~DissolveModel() = default;

    public:
    // Getter for filename
    QUrl fileName() const;
    // Setter for filename
    void loadInput(QUrl filename);
    // Getter for graph
    Graph *graph();

    Q_SIGNALS:
    // The models might've been updated
    void modelsUpdated();

    public Q_SLOTS:
    // Export to a file
    bool save();
    bool saveAs(QUrl filename);

    /*
     * Data
     */
    private:
    // The main Dissolve instance
    Dissolve *dissolve_ = nullptr;

    public:
    // Access dissolve reference
    Dissolve &dissolve();
    // Set reference to Dissolve
    void setDissolve(Dissolve &dissolve);
    // Update models
    void update();
};
