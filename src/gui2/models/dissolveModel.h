// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "nodes/dissolve.h"
#include <QUrl>
#include <QObject>

class DissolveModel : public QObject
{
    Q_OBJECT
    // File to load
    Q_PROPERTY(QUrl file READ fileName WRITE loadInput NOTIFY modelsUpdated)
    // The main graph
    Q_PROPERTY(DissolveGraph *graph READ dissolveGraph NOTIFY modelsUpdated)

    Q_SIGNALS:
    // The models might've been updated
    void modelsUpdated();

    public:
    DissolveModel() = default;
    ~DissolveModel() = default;

    private:
    // The DissolveGraph root node
    DissolveGraph *dissolveGraph_ = nullptr;

    public:
    // Return the current filename
    QUrl fileName() const;
    // Load the specified file
    void loadInput(QUrl filename);
    // Getter for root graph
    DissolveGraph *dissolveGraph();
    // Set root graph
    void setDissolveGraph(DissolveGraph *dissolveGraph);
    // Update models
    void update();

    public Q_SLOTS:
    // Export to a file
    bool save();
    bool saveAs(QUrl filename);
};
