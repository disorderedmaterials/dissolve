// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/gizmo.h"
#include "gui/ui_integrator1dgizmo.h"
#include "math/sampleddouble.h"
#include "templates/optionalref.h"

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
    OptionalReferenceWrapper<Data1D> integrationTarget_;
    // Calculated integrals
    SampledDouble integrals_[3];

    private:
    // Calculate integrals from current limits
    void calculateIntegrals();
    // Set data targets in graphs
    void setGraphDataTargets();

    /*
     * Widget Signals / Slots
     */
    private slots:
    void on_TargetSelectButton_clicked(bool checked);

    signals:
    void windowClosed(QString windowTitle);
};
