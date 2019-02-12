/*
	*** Graph Widget
	*** src/gui/viewer/graphwidget.h
	Copyright T. Youngs 2013-2019

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

#ifndef DISSOLVE_GRAPHWIDGET_H
#define DISSOLVE_GRAPHWIDGET_H

#include "gui/viewer/ui_graphwidget.h"

// Forward Declarations
/* none */

// Graph Widget
class GraphWidget : public QWidget
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	GraphWidget(QWidget* parent = 0);
	~GraphWidget();

	private:
	// Main form declaration
	Ui::GraphWidget ui_;
	// Return centre coordinate of main window
	QPoint centrePos();

	public:
	// Return contained DataViewWidget
	DataViewer* dataViewer();


	/*
	 * Signals / Slots
	 */
	public slots:
	// Update coordinate info
	void updateCoordinateInfo();
};

#endif
