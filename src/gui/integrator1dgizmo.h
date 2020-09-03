/*
    *** Integrator1D Gizmo
    *** src/gui/integrator1dgizmo.h
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
#include "gui/ui_integrator1dgizmo.h"
#include "math/sampleddouble.h"

// Forward Declarations
class Dissolve;

// Integrator1D Gizmo
class Integrator1DGizmo : public QWidget, public Gizmo
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    Integrator1DGizmo(Dissolve &dissolve, const QString uniqueName);
    ~Integrator1DGizmo();

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
    Ui::Integrator1DGizmo ui_;

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
     * Data
     */
    private:
    // Data1D that we are integrating
    Data1D *integrationTarget_;
    // Calculated integrals
    SampledDouble integrals_[3];

    private:
    // Calculate integrals from current limits
    void calculateIntegrals();
    // Set data targets in graphs
    void setGraphDataTargets();

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
    void on_TargetSelectButton_clicked(bool checked);

    signals:
    void windowClosed(QString windowTitle);
};
