/*
	*** Graph Gizmo
	*** src/gui/gizmos/graph_funcs.cpp
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

#include "gui/gizmos/graph.h"
#include "gui/selectgenericitemdialog.h"
#include "main/dissolve.h"
#include "math/integrator.h"
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>

// Constructor
GraphGizmo::GraphGizmo(Dissolve& dissolve, const char* uniqueName) : Gizmo(dissolve, uniqueName)
{
	// Set up user interface
	ui_.setupUi(this);

	// Grab the DataViewer pointer from the 
	View& view = ui_.DataWidget->view();
	view.setViewType(View::FlatXYView);
	view.axes().setTitle(0, "X");
	view.axes().setRange(0, 0.0, 10.0);
	view.axes().setTitle(1, "Y");
	view.axes().setRange(1, 0.0, 10.0);

	refreshing_ = false;
}

// Destructor
GraphGizmo::~GraphGizmo()
{
}

/*
 * Core
 */

// Return string specifying Gizmo type
const char* GraphGizmo::type() const
{
	return "Graph";
}

/*
 * UI
 */

// Window close event
void GraphGizmo::closeEvent(QCloseEvent* event)
{
	emit(windowClosed(uniqueName_.get()));
}

// Update controls within widget
void GraphGizmo::updateControls()
{
	refreshing_ = true;

	// Refresh the graph
	ui_.DataWidget->postRedisplay();

	refreshing_ = false;
}

// Disable sensitive controls within widget
void GraphGizmo::disableSensitiveControls()
{
}

// Enable sensitive controls within widget
void GraphGizmo::enableSensitiveControls()
{
}

/*
 * State
 */

// Write widget state through specified LineParser
bool GraphGizmo::writeState(LineParser& parser) const
{

	// Write DataViewer state
	if (!ui_.DataWidget->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool GraphGizmo::readState(LineParser& parser)
{
	// Read the DataViewer session info
	if (!ui_.DataWidget->readSession(parser)) return false;

	return true;
}

/*
 * Widget Signals / Slots
 */


// Interaction
void GraphGizmo::on_InteractionViewButton_clicked(bool checked)
{
	ui_.DataWidget->setInteractionMode(DataViewer::DefaultInteraction);
}

// Graph
void GraphGizmo::on_GraphResetButton_clicked(bool checked)
{
	ui_.DataWidget->view().showAllData();
	ui_.DataWidget->view().resetViewMatrix();

	ui_.DataWidget->postRedisplay();
}

void GraphGizmo::on_GraphFollowAllButton_clicked(bool checked)
{
	if (checked)
	{
		ui_.DataWidget->view().setAutoFollowType(View::AllAutoFollow);
		if (ui_.GraphFollowXButton->isChecked()) ui_.GraphFollowXButton->setChecked(false);
	}
	else ui_.DataWidget->view().setAutoFollowType(View::NoAutoFollow);

	ui_.DataWidget->postRedisplay();
}

void GraphGizmo::on_GraphFollowXButton_clicked(bool checked)
{
	if (checked)
	{
		ui_.DataWidget->view().setAutoFollowType(View::XAutoFollow);
		if (ui_.GraphFollowAllButton->isChecked()) ui_.GraphFollowAllButton->setChecked(false);
	}
	else ui_.DataWidget->view().setAutoFollowType(View::NoAutoFollow);

	ui_.DataWidget->postRedisplay();
}

void GraphGizmo::on_GraphFollowXLengthSpin_valueChanged(double value)
{
	ui_.DataWidget->view().setAutoFollowXLength(value);

	ui_.DataWidget->postRedisplay();
}

// View
void GraphGizmo::on_ViewToggleDataButton_clicked(bool checked)
{
	ui_.DataGroup->setVisible(checked);
}

void GraphGizmo::on_ViewAxesVisibleButton_clicked(bool checked)
{
	ui_.DataWidget->setAxesVisible(checked);

	ui_.DataWidget->postRedisplay();
}

void GraphGizmo::on_ViewCopyToClipboardButton_clicked(bool checked)
{
	ui_.DataWidget->copyViewToClipboard(checked);
}
