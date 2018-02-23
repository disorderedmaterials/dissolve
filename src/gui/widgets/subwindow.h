/*
	*** SubWindow
	*** src/gui/widgets/subwindow.h
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

#ifndef DUQ_SUBWINDOW_H
#define DUQ_SUBWINDOW_H

#include "templates/listitem.h"

// Forward Declarations
class QMdiSubWindow;
class SubWidget;

class SubWindow : public ListItem<SubWindow>
{
	public:
	// Constructor
	SubWindow(QMdiSubWindow* window, SubWidget* widget, void* data);

	private:
	// QMdiSubWindow containing the SubWidget displaying the data
	QMdiSubWindow* window_;
	// SubWidget displayed in the QMdiSubWindow, displaying the data
	SubWidget* subWidget_;
	// Data which is the display target in the SubWidget
	void* data_;

	public:
	// Return QMdiSubWindow containing the SubWidget displaying the data
	QMdiSubWindow* window();
	// Return SubWidget displayed in the QMdiSubWindow, displaying the data
	SubWidget* subWidget();
	// Set data which is the display target in the SubWidget
	void setData(void* data);
	// Return data which is the display target in the SubWidget
	void* data();
	// Raise the window
	void raise();
};

#endif
