/*
    *** Graph Gizmo
    *** src/gui/graphgizmo.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    bool sendData(std::string_view dataType, std::string_view objectTag, std::string_view name);

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
     * State
     */
    public:
    // Write widget state through specified LineParser
    bool writeState(LineParser &parser) const;
    // Read widget state through specified LineParser
    bool readState(LineParser &parser);

    /*
     * Widget Signals / Slots
     */
    private slots:

    signals:
    void windowClosed(QString windowTitle);
};
