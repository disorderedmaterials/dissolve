/*
	*** Module Insertion Block Widget
	*** src/gui/charts/moduleinsertionblock.h
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

#ifndef DISSOLVE_CHARTS_MODULEINSERTIONBLOCK_H
#define DISSOLVE_CHARTS_MODULEINSERTIONBLOCK_H

#include "gui/charts/ui_moduleinsertionblock.h"
#include "gui/modulechartblock.h"
#include "templates/reflist.h"

// Forward Declarations
class Dissolve;
class DissolveWindow;

// Module Insertion Block Widget
class ModuleInsertionBlock : public QWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	ModuleInsertionBlock(QWidget* parent);
	~ModuleInsertionBlock();


	/*
	 * UI
	 */
	private:
	// Main form declaration
	Ui::ModuleInsertionWidget ui_;


	/*
	 * QWidget Reimplementations
	 */
	protected:
	// Paint event
	void paintEvent(QPaintEvent* event);


	/*
	 * Widget Functions
	 */
	public:
	// Return underlying widget
	QWidget* widget();
	// Update controls within widget
	void updateControls();
	// Disable sensitive controls
	void disableSensitiveControls();
	// Enable sensitive controls
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


	/*
	 * Icons
	 */
	public:
	// Set icons to reflect insertion of specified Module
	void setDisplayModuleInsertion(QString moduleType);
	// Set icons to reflect moving existing Module to new position
	void setDisplayModuleMove();
};

#endif
