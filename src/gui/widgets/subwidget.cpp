/*
	*** SubWindow Widget
	*** src/gui/widgets/subwidget.cpp
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

#include "gui/widgets/subwidget.h"
#include <stdio.h>

// Constructor
SubWidget::SubWidget(DissolveWindow* dissolveWindow, const char* title) : QWidget(NULL), ListItem<SubWidget>()
{
	dissolveWindow_ = dissolveWindow;
	title_ = title;
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
