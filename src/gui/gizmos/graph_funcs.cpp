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

	// Grab the data viewer target and set our view
	dataViewer_ = ui_.DataView->dataViewer();
	dataViewer_->view().setAutoFollowType(View::AllAutoFollow);
	dataViewer_->groupManager().setGroupColouring("Default", RenderableGroup::AutomaticIndividualColouring);

	// Enable our DataViewer as a target
	dataViewer_->enableAsRenderableDestination();
	dataViewer_->setDestinationName(uniqueName);

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
	dataViewer_->postRedisplay();

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
	if (!dataViewer_->writeSession(parser)) return false;

	return true;
}

// Read widget state through specified LineParser
bool GraphGizmo::readState(LineParser& parser)
{
	// Read the DataViewer session info
	if (!dataViewer_->readSession(parser)) return false;

	return true;
}

/*
 * Widget Signals / Slots
 */
