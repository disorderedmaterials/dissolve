// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/integrator1dgizmo.h"
#include "gui/render/renderabledata1d.h"
#include "gui/selectgenericitemdialog.h"
#include "main/dissolve.h"
#include "math/integrator.h"
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>

Integrator1DGizmo::Integrator1DGizmo(Dissolve &dissolve, const QString uniqueName) : Gizmo(dissolve, uniqueName)
{
    // Set up user interface
    ui_.setupUi(this);

    // Grab the DataViewer pointer from the
    auto &view = ui_.PlotWidget->view();
    view.setViewType(View::FlatXYView);
    view.axes().setTitle(0, "X");
    view.axes().setRange(0, 0.0, 10.0);
    view.axes().setTitle(1, "Y");
    view.axes().setRange(1, 0.0, 10.0);

    integrationTarget_ = std::nullopt;
    integrals_[0] = 0.0;
    integrals_[1] = 0.0;
    integrals_[2] = 0.0;

    refreshing_ = false;
}

Integrator1DGizmo::~Integrator1DGizmo() {}

/*
 * Core
 */

// Return string specifying Gizmo type
const QString Integrator1DGizmo::type() const { return "Integrator1D"; }

/*
 * UI
 */

// Window close event
void Integrator1DGizmo::closeEvent(QCloseEvent *event) { emit(windowClosed(uniqueName_)); }

// Update controls within widget
void Integrator1DGizmo::updateControls()
{
    refreshing_ = true;

    // Refresh the graph
    ui_.PlotWidget->postRedisplay();

    // Get limits from data
    auto xMin = integrationTarget_ ? integrationTarget_->get().xAxis().front() : 0.0;
    auto xMax = integrationTarget_ ? integrationTarget_->get().xAxis().back() : 1.0;
    ui_.Region1MinSpin->setRange(xMin, xMax);
    ui_.Region1MaxSpin->setRange(xMin, xMax);

    refreshing_ = false;
}

// Disable sensitive controls within widget
void Integrator1DGizmo::disableSensitiveControls() {}

// Enable sensitive controls within widget
void Integrator1DGizmo::enableSensitiveControls() {}

/*
 * Data
 */

// Calculate integrals from current limits
void Integrator1DGizmo::calculateIntegrals()
{
    // Check for a valid Data target
    // Region 1
    // 	if (ui_.Region1Group->isChecked())
    // 	{
    // 		double integral = Integrator::trapezoid(
    // 	}
}

// Set data targets in graphs
void Integrator1DGizmo::setGraphDataTargets()
{
    // Remove any current data
    ui_.PlotWidget->clearRenderables();

    if (!integrationTarget_)
        return;

    ui_.PlotWidget->createRenderable<RenderableData1D, Data1D>(integrationTarget_->get(), integrationTarget_->get().tag());
}

/*
 * Widget Signals / Slots
 */

void Integrator1DGizmo::on_TargetSelectButton_clicked(bool checked)
{
    SelectGenericItemDialog genericItemDialog(this, dissolve_);
    auto itemName = genericItemDialog.selectGenericItem<Data1D>();
    if (itemName.isEmpty())
        return;

    // Set target
    integrationTarget_ = dissolve_.processingModuleData().retrieve<Data1D>(qPrintable(itemName));

    // Refresh graph data
    setGraphDataTargets();

    // Set limits in controls etc.
    updateControls();
}
