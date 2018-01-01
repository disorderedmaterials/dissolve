/*
	*** SubWindow Widget
	*** src/gui/subwidget.h
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

#ifndef DUQ_SUBWIDGET_H
#define DUQ_SUBWIDGET_H

#include <QWidget>

// Forward Declarations
class LineParser;
class SubWindow;

// Subwidget (root class for any widget to be displayed in a QMdiSubWindow)
class SubWidget : public QWidget
{
	protected:
	// Whether widget is currently refreshing
	bool refreshing_;

	public:
	// Constructor / Destructor
	SubWidget(QWidget* parent);
	virtual ~SubWidget();
	// Update controls within widget
	virtual void updateControls() = 0;


	/*
	 * SubWindow Parent
	 */
	protected:
	// SubWindow in which this widget is displayed
	SubWindow* subWindow_;

	public:
	// Set SubWindow in which this widget is displayed
	void setSubWindow(SubWindow* subWindow);
	// Return SubWindow in which this widget is displayed
	SubWindow* subWindow();


	/*
	 * Identification
	 */
	public:
	// Return string specifying widget type
	virtual const char* widgetType() = 0;


	/*
	 * State
	 */
	public:
	// Write widget state through specified LineParser
	virtual bool writeState(LineParser& parser) = 0;
	// Read widget state through specified LineParser
	virtual bool readState(LineParser& parser) = 0;


	/*
	 * Reimplementations
	 */
	protected:
	virtual void closeEvent(QCloseEvent* event) = 0;
};

#endif
