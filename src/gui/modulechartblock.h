/*
	*** Module Chart Block
	*** src/gui/modulechartblock.h
	Copyright T. Youngs 2012-2018

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

#ifndef DISSOLVE_MODULECHARTBLOCK_H
#define DISSOLVE_MODULECHARTBLOCK_H

#include "templates/listitem.h"
#include <QPoint>

// Forward Declarations
class Dissolve;
class DissolveWindow;
class QWidget;

// Module Chart Block - Base class for any widget displayed in a ModuleChart
class ModuleChartBlock : public ListItem<ModuleChartBlock>
{
	protected:
	// Pointer to DissolveWindow
	DissolveWindow* dissolveWindow_;
	// Reference to Dissolve
	Dissolve& dissolve_;

	public:
	// Constructor / Destructor
	ModuleChartBlock(DissolveWindow* dissolveWindow, Dissolve& dissolve);
	~ModuleChartBlock();


	/*
	 * Widget Functions
	 */
	protected:
	// Whether the derived-class widget is currently refreshing
	bool refreshing_;

	public:
	// Return underlying widget
	virtual QWidget* widget() = 0;
	// Update controls within widget
	virtual void updateControls() = 0;
	// Disable sensitive controls, ready for main code to run
	virtual void disableSensitiveControls() = 0;
	// Enable sensitive controls, ready for main code to run
	virtual void enableSensitiveControls() = 0;


	/*
	 * Geometry
	 */
	public:
	// Return width of underlying widget
	virtual int widgetWidth() const = 0;
	// Return height of underlying widget
	virtual int widgetHeight() const = 0;
	// Set underlying widget geometry
	virtual void setWidgetGeometry(int left, int top, int width, int height) = 0;
	// Return right-hand-side underlying widget anchor point
	virtual QPoint globalRightHandWidgetAnchor() const = 0;
	// Return left-hand-side underlying widget anchor point
	virtual QPoint globalLeftHandWidgetAnchor() const = 0;
};

#endif
