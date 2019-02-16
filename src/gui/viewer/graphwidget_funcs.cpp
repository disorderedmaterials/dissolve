/*
	*** Graph Widget - Functions 
	*** src/gui/viewer/graphwidget_funcs.cpp
	Copyright T. Youngs 2013-2019

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

#include "gui/viewer/graphwidget.h"
#include "gui/viewer/render/view.h"

// Constructor
GraphWidget::GraphWidget(QWidget* parent) : QWidget(parent)
{
	// Call the main creation function
	ui_.setupUi(this);
}

// Destructor
GraphWidget::~GraphWidget()
{
}

// Return contained DataViewWidget
DataViewer* GraphWidget::dataViewer()
{
	return ui_.DataView;
}

/*
 * Signals / Slots
 */

// Update coordinate info
void GraphWidget::updateCoordinateInfo()
{
	View& view = ui_.DataView->view();
	Vec3<double> rLocal = ui_.DataView->rCurrentLocal();
	QString text;
	switch (view.viewType())
	{
		case (View::FlatXYView):
			text.sprintf("x = %e, y = %e", rLocal.x, rLocal.y);
			break;
		case (View::FlatXZView):
			text.sprintf("x = %e, z = %e", rLocal.x, rLocal.z);
			break;
		case (View::FlatZYView):
			text.sprintf("z = %e, y = %e", rLocal.z, rLocal.y);
			break;
		default:
			text.sprintf("x = %e, y = %e, z = %e", rLocal.x, rLocal.y, rLocal.z);
			break;
	}

	ui_.CoordinateLabel->setText(text);
}

