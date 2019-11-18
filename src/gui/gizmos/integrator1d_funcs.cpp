/*
	*** Integrator1D Gizmo
	*** src/gui/gizmos/integrator1d_funcs.cpp
	Copyright T. Youngs 2012-2019

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

#include "gui/gizmos/integrator1d.h"
#include "main/dissolve.h"
#include "math/integrator.h"
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>

// Constructor
Integrator1DGizmo::Integrator1DGizmo(DissolveWindow* dissolveWindow) : Gizmo(dissolveWindow)
{
	// Set up user interface
	ui_.setupUi(this);

	// Grab the DataViewer pointer from the 
	View& view = ui_.PlotWidget->view();
	view.setViewType(View::FlatXYView);
	view.axes().setTitle(0, "\\it{r}, \\sym{angstrom}");
	view.axes().setRange(0, 0.0, 10.0);
	view.axes().setTitle(1, "U(r), kJ mol\\sup{-1}");
	view.axes().setRange(1, 0.0, 10.0);

	refreshing_ = false;
}

// Destructor
Integrator1DGizmo::~Integrator1DGizmo()
{
}

/*
 * Core
 */

// Return string specifying Gizmo type
const char* Integrator1DGizmo::type() const
{
	return "Integrator1D";
}

/*
 * UI
 */

// Window close event
void Integrator1DGizmo::closeEvent(QCloseEvent* event)
{
// 	emit(windowClosed());
}

// Update controls within widget
void Integrator1DGizmo::updateControls()
{
	refreshing_ = true;

	// Refresh the graph
	ui_.PlotWidget->postRedisplay();

	refreshing_ = false;
}

// Disable sensitive controls within widget
void Integrator1DGizmo::disableSensitiveControls()
{
}

// Enable sensitive controls within widget
void Integrator1DGizmo::enableSensitiveControls()
{
}

/*
 * State
 */

// Write widget state through specified LineParser
bool Integrator1DGizmo::writeState(LineParser& parser)
{
	// Write DataViewer state
	if (!ui_.PlotWidget->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool Integrator1DGizmo::readState(LineParser& parser)
{
	// Read the DataViewer session info
	if (!ui_.PlotWidget->readSession(parser)) return false;

	return true;
}

/*
 * Widget Slots
 */
