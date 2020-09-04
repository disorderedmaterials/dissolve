/*
    *** Integrator1D Gizmo
    *** src/gui/integrator1dgizmo_funcs.cpp
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

#include "gui/integrator1dgizmo.h"
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

    integrationTarget_ = nullptr;
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
    double xMin = integrationTarget_ ? integrationTarget_->xAxis().first() : 0.0;
    double xMax = integrationTarget_ ? integrationTarget_->xAxis().last() : 1.0;
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

    Renderable *data = ui_.PlotWidget->createRenderable(Renderable::Data1DRenderable, integrationTarget_->objectTag(),
                                                        integrationTarget_->name());
}

/*
 * State
 */

// Write widget state through specified LineParser
bool Integrator1DGizmo::writeState(LineParser &parser) const
{

    // Write DataViewer state
    if (!ui_.PlotWidget->writeSession(parser))
        return false;

    return true;
}

// Read widget state through specified LineParser
bool Integrator1DGizmo::readState(LineParser &parser)
{
    // Read the DataViewer session info
    if (!ui_.PlotWidget->readSession(parser))
        return false;

    return true;
}

/*
 * Widget Signals / Slots
 */

void Integrator1DGizmo::on_TargetSelectButton_clicked(bool checked)
{
    SelectGenericItemDialog genericItemDialog(this, dissolve_);
    Data1D *item = genericItemDialog.selectGenericItem<Data1D>();
    if (!item)
        return;

    // Set target
    integrationTarget_ = item;

    // Refresh graph data
    setGraphDataTargets();

    // Set limits in controls etc.
    updateControls();
}
