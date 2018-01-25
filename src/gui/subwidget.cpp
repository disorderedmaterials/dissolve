/*
	*** SubWindow Widget
	*** src/gui/subwidget.cpp
	Copyright T. Youngs 2012-2018

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
#include <stdio.h>

// Constructor
SubWidget::SubWidget(QWidget* parent, const char* title) : QWidget(parent), ListItem<SubWidget>()
{
	title_ = title;
	subWindow_ = NULL;
	refreshing_ = false;
}

SubWidget::~SubWidget()
{
	printf("FIX ME - THIS NEEDS TO REMOVE THE WINDOW FROM THE SUBWINDOW PARENT'S LIST!\n");
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

// Set title of widget
void SubWidget::setTitle(const char* title)
{
	title_ = title;
}

// Return title of widget (and associated window title, if in a window)
const char* SubWidget::title()
{
	return title_.get();
}

// Return string specifying widget type
const char* SubWidget::widgetType()
{
	return "NULL";
}
