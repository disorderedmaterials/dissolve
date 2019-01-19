/*
	*** ModuleChart Insertion Block Widget
	*** src/gui/modulechartinsertionblock.h
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

#ifndef DISSOLVE_MODULECHARTINSERTIONBLOCKWIDGET_H
#define DISSOLVE_MODULECHARTINSERTIONBLOCKWIDGET_H

#include "gui/ui_modulechartinsertionblock.h"
#include "gui/modulechartblock.h"
#include "templates/reflist.h"

// Forward Declarations
class Dissolve;
class DissolveWindow;

// ModuleChart Insertion Block Widget
class ModuleChartInsertionBlock : public QWidget, public ModuleChartBlock
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	ModuleChartInsertionBlock(QWidget* parent, DissolveWindow* dissolveWindow);
	~ModuleChartInsertionBlock();
	// Main form declaration
	Ui::ModuleChartInsertionWidget ui;


	/*
	 * QWidget Reimplementations
	 */
	protected:
	// Paint event
	void paintEvent(QPaintEvent* event);


	/*
	 * Block Type
	 */
	public:
	// Return type of this block
	ModuleChartBlock::ModuleChartBlockType blockType();


	/*
	 * Widget Functions
	 */
	public:
	// Return underlying widget
	QWidget* widget();
	// Update controls within widget
	void updateControls();
	// Disable sensitive controls, ready for main code to run
	void disableSensitiveControls();
	// Enable sensitive controls, ready for main code to run
	void enableSensitiveControls();


	/*
	 * Geometry
	 */
	public:
	// Return width of underlying widget
	int widgetWidth() const;
	// Return height of underlying widget
	int widgetHeight() const;
	// Set underlying widget geometry
	void setWidgetGeometry(int left, int top, int width, int height);
};

#endif
