/*
	*** SubWindow
	*** src/gui/subwindow.cpp
	Copyright T. Youngs 2012-2018

	This file is part of DUQ.

	DUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	DUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with DUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/subwindow.h"
#include "gui/subwidget.h"

// Constructor
SubWindow::SubWindow(QMdiSubWindow* window, SubWidget* widget, void* data)
{
	window_ = window;
	subWidget_ = widget;
	data_ = data;

	if (subWidget_) subWidget_->setSubWindow(this);
}

// Return QMdiSubWindow containing the SubWidget displaying the data
QMdiSubWindow* SubWindow::window()
{
	return window_;
}

// Return SubWidget displayed in the QMdiSubWindow, displaying the data
SubWidget* SubWindow::subWidget()
{
	return subWidget_;
}

// Set data which is the display target in the SubWidget
void SubWindow::setData(void* data)
{
	data_ = data;
}

// Return data which is the display target in the SubWidget
void* SubWindow::data()
{
	return data_;
}

// Raise the window
void SubWindow::raise()
{
	if (window_) window_->raise();
}
