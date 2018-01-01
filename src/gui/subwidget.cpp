/*
	*** SubWindow Widget
	*** src/gui/subwidget.cpp
	Copyright T. Youngs 2007-2018

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/subwidget.h"

// Constructor
SubWidget::SubWidget(QWidget* parent) : QWidget(parent)
{
	subWindow_ = NULL;
	refreshing_ = false;
}

SubWidget::~SubWidget()
{
}

/*
 * SubWindow Parent
 */

// Set SubWindow in which this widget is displayed
void SubWidget::setSubWindow(SubWindow* subWindow)
{
	subWindow_ = subWindow;
}

// Return SubWindow in which this widget is displayed
SubWindow* SubWidget::subWindow()
{
	return subWindow_;
}

/*
 * Identification
 */

// Return string specifying widget type
const char* SubWidget::widgetType()
{
	return "NULL";
}
