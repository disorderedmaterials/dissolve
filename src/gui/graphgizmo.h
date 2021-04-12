// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/gizmo.h"
#include "gui/ui_graphgizmo.h"
#include "math/sampleddouble.h"

// Forward Declarations
class Dissolve;

// Graph Gizmo
class GraphGizmo : public QWidget, public Gizmo
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    GraphGizmo(Dissolve &dissolve, const QString uniqueName);
    ~GraphGizmo();

    /*
     * Core
     */
    public:
    // Return string specifying Gizmo type
    const QString type() const;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::GraphGizmo ui_;

    protected:
    // Window close event
    void closeEvent(QCloseEvent *event);

    public:
    // Update controls within widget
    void updateControls();
    // Disable sensitive controls within widget
    void disableSensitiveControls();
    // Enable sensitive controls within widget
    void enableSensitiveControls();

    /*
     * Data Handling
     */
    public:
    // Return whether this Gizmo accepts data of the specified type
    bool acceptsData(std::string_view dataType);
    // Send data (referenced by its object tag) to the Gizmo
    bool sendData(std::string_view dataType, std::string_view tag, std::string_view name);

    /*
     * Data
     */
    private:
    // Pointer to our DataViewer
    DataViewer *dataViewer_;

    public:
    // Return pointer to the contained DataViewer
    DataViewer *dataViewer() const;

    /*
     * Widget Signals / Slots
     */
    private slots:

    signals:
    void windowClosed(QString windowTitle);
};
