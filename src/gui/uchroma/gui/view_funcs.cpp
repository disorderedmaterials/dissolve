/*
	*** View Window
	*** src/gui/uchroma/gui/view_funcs.cpp
	Copyright T. Youngs 2013-2017

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/uchroma/gui/view.h"
#include "gui/uchroma/uchromabase.h"
#include "templates/reflist.h"

/*
 * Window Functions
 */

// Constructor
ViewWindow::ViewWindow(UChromaBase& uChromaBase, QWidget* parent) : uChromaBase_(uChromaBase), QWidget(parent)
{
	ui.setupUi(this);

	QWidget::setWindowFlags(Qt::Tool);

	refreshing_ = false;
}

// Destructor
ViewWindow::~ViewWindow()
{
}

// Window close event
void ViewWindow::closeEvent(QCloseEvent *event)
{
	emit(windowClosed(false));
}

/*
 * Convenience Functions
 */

/*
 * Slots
 */

void ViewWindow::on_ViewBoundingBoxNoneRadio_clicked(bool checked)
{
	if (refreshing_) return;

	// Check for valid ViewPane
	ViewPane* viewPane = uChromaBase_.currentViewPane();
	if (!ViewPane::objectValid(viewPane, "view pane in ViewWindow::on_ViewBoundingBoxNoneRadio_clicked")) return;

	viewPane->setBoundingBox(ViewPane::NoBox);

	// Update display
	uChromaBase_.setAsModified();
	uChromaBase_.updateDisplay();
}

void ViewWindow::on_ViewBoundingBoxPlaneRadio_clicked(bool checked)
{
	if (refreshing_) return;

	// Check for valid ViewPane
	ViewPane* viewPane = uChromaBase_.currentViewPane();
	if (!ViewPane::objectValid(viewPane, "view pane in ViewWindow::on_ViewBoundingBoxPlaneRadio_clicked")) return;

	viewPane->setBoundingBox(ViewPane::PlaneBox);

	// Update display
	uChromaBase_.setAsModified();
	uChromaBase_.updateDisplay();
}

void ViewWindow::on_ViewBoundingBoxCubeRadio_clicked(bool checked)
{
	if (refreshing_) return;

	// Check for valid ViewPane
	ViewPane* viewPane = uChromaBase_.currentViewPane();
	if (!ViewPane::objectValid(viewPane, "view pane in ViewWindow::on_ViewBoundingBoxCubeRadio_clicked")) return;

	viewPane->setBoundingBox(ViewPane::CubeBox);

	// Update display
	uChromaBase_.setAsModified();
	uChromaBase_.updateDisplay();
}

void ViewWindow::on_ViewBoundingBoxPlaneYSpin_valueChanged(double value)
{
	if (refreshing_) return;

	// Check for valid ViewPane
	ViewPane* viewPane = uChromaBase_.currentViewPane();
	if (!ViewPane::objectValid(viewPane, "view pane in ViewWindow::on_ViewBoundingBoxPlaneYSpin_valueChanged")) return;

	viewPane->setBoundingBoxPlaneY(value);

	// Update display
	uChromaBase_.setAsModified();
	uChromaBase_.updateDisplay();
}

void ViewWindow::on_ViewBoundingBoxPlaneYSetMinimumButton_clicked(bool checked)
{
	if (refreshing_) return;

	// Check for valid ViewPane
	ViewPane* viewPane = uChromaBase_.currentViewPane();
	if (!ViewPane::objectValid(viewPane, "view pane in ViewWindow::on_ViewBoundingBoxPlaneYSetMinimumButton_clicked")) return;

	ui.ViewBoundingBoxPlaneYSpin->setValue(viewPane->axes().min(1));
}

void ViewWindow::on_ViewBoundingBoxPlaneYSetZeroButton_clicked(bool checked)
{
	ui.ViewBoundingBoxPlaneYSpin->setValue(0.0);
}

void ViewWindow::on_ViewBoundingBoxPlaneYSetMaximumButton_clicked(bool checked)
{
	if (refreshing_) return;

	// Check for valid ViewPane
	ViewPane* viewPane = uChromaBase_.currentViewPane();
	if (!ViewPane::objectValid(viewPane, "view pane in ViewWindow::on_ViewBoundingBoxPlaneYSetMaximumButton_clicked")) return;

	ui.ViewBoundingBoxPlaneYSpin->setValue(viewPane->axes().max(1));
}

// -- General Options
void ViewWindow::on_ViewFlatLabelsCheck_clicked(bool checked)
{
	if (refreshing_) return;

	// Check for valid ViewPane
	ViewPane* viewPane = uChromaBase_.currentViewPane();
	if (!ViewPane::objectValid(viewPane, "view pane in ViewWindow::on_ViewBoundingBoxPlaneYSetMaximumButton_clicked")) return;

	viewPane->setFlatLabelsIn3D(checked);

	// Update display
	uChromaBase_.setAsModified();
	uChromaBase_.updateDisplay();
}

void ViewWindow::on_ViewLabelPointSizeSpin_valueChanged(double value)
{
	if (refreshing_) return;

	// Check for valid ViewPane
	ViewPane* viewPane = uChromaBase_.currentViewPane();
	if (!ViewPane::objectValid(viewPane, "view pane in ViewWindow::on_ViewLabelPointSizeSpin_valueChanged")) return;
	
	viewPane->setLabelPointSize(value);

	// Update display
	uChromaBase_.setAsModified();
	uChromaBase_.updateDisplay();
}

void ViewWindow::on_ViewTitlePointSizeSpin_valueChanged(double value)
{
	if (refreshing_) return;

	// Check for valid ViewPane
	ViewPane* viewPane = uChromaBase_.currentViewPane();
	if (!ViewPane::objectValid(viewPane, "view pane in ViewWindow::on_ViewTitlePointSizeSpin_valueChanged")) return;
	
	viewPane->setTitlePointSize(value);

	// Update display
	uChromaBase_.setAsModified();
	uChromaBase_.updateDisplay();
}

/*
 * Update
 */

// Update controls and show window
void ViewWindow::updateAndShow()
{
	updateControls();
	show();
	move(uChromaBase_.centrePos() - QPoint(width()/2, height()/2));
}

// Update controls
void ViewWindow::updateControls(bool force)
{
	// If the window isn't visible, do nothing...
	if ((!isVisible()) && (!force) ) return;

	// Check for valid ViewPane
	ViewPane* viewPane = uChromaBase_.currentViewPane();
	if (!ViewPane::objectValid(viewPane, "view pane in ViewWindow::updateControls")) return;

	refreshing_ = true;

	// Bounding box
	if (viewPane->boundingBox() == ViewPane::NoBox) ui.ViewBoundingBoxNoneRadio->setChecked(true);
	else if (viewPane->boundingBox() == ViewPane::PlaneBox) ui.ViewBoundingBoxPlaneRadio->setChecked(true);
	else if (viewPane->boundingBox() == ViewPane::CubeBox) ui.ViewBoundingBoxCubeRadio->setChecked(true);
	ui.ViewBoundingBoxPlaneYSpin->setValue(viewPane->boundingBoxPlaneY());

	// Label Options
	ui.ViewLabelPointSizeSpin->setValue(viewPane->labelPointSize());
	ui.ViewTitlePointSizeSpin->setValue(viewPane->titlePointSize());
	ui.ViewFlatLabelsCheck->setChecked(viewPane->flatLabelsIn3D());

	refreshing_ = false;
}
