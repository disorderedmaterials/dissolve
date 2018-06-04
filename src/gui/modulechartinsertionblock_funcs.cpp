/*
	*** ModuleChart Insertion Block Widget - Functions
	*** src/gui/modulechartinsertionblock_funcs.cpp
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

#include "gui/modulechartinsertionblock.h"
#include "gui/gui.h"
#include "gui/keywordwidgets.h"
#include "main/dissolve.h"
#include <QPainter>

// Constructor
ModuleChartInsertionBlock::ModuleChartInsertionBlock(QWidget* parent, DissolveWindow* dissolveWindow) : QWidget(parent), ModuleChartBlock(dissolveWindow, dissolveWindow->dissolve())
{
	// Set up user interface
	ui.setupUi(this);

	updateControls();
}

ModuleChartInsertionBlock::~ModuleChartInsertionBlock()
{
}

/*
 * Widget Functions
 */

// Return underlying widget
QWidget* ModuleChartInsertionBlock::widget()
{
	return this;
}

// Update controls within widget
void ModuleChartInsertionBlock::updateControls()
{
}

// Disable sensitive controls, ready for main code to run
void ModuleChartInsertionBlock::disableSensitiveControls()
{
}

// Enable sensitive controls, ready for main code to run
void ModuleChartInsertionBlock::enableSensitiveControls()
{
}

/*
 * Geometry
 */

// Return width of underlying widget
int ModuleChartInsertionBlock::widgetWidth() const
{
	return minimumSize().width();
}

// Return height of underlying widget
int ModuleChartInsertionBlock::widgetHeight() const
{
	return minimumSize().height();
}

// Set underlying widget geometry
void ModuleChartInsertionBlock::setWidgetGeometry(int left, int top, int width, int height)
{
	setGeometry(left, top, width, height);
}

// Return right-hand-side flow anchor point
QPoint ModuleChartInsertionBlock::globalRightHandWidgetAnchor() const
{
	QPoint X = mapToGlobal(rect().topRight() + QPoint(1, 0));
	QPoint Y = mapToGlobal(ui.MainFrame->rect().topRight()) + QPoint(0, ui.MainFrame->height()/2);
	return QPoint(X.x(), Y.y());
}

// Return left-hand-side flow anchor point
QPoint ModuleChartInsertionBlock::globalLeftHandWidgetAnchor() const
{
	QPoint X = mapToGlobal(rect().topLeft() + QPoint(-1, 0));
	QPoint Y = mapToGlobal(ui.MainFrame->rect().topLeft()) + QPoint(0, ui.MainFrame->height()/2);
	return QPoint(X.x(), Y.y());
}
