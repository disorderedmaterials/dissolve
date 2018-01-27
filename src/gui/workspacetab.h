/*
	*** Workspace Tab
	*** src/gui/workspacetab.h
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

#ifndef DUQ_WORKSPACETAB_H
#define DUQ_WORKSPACETAB_H

#include "gui/ui_workspacetab.h"
#include "gui/maintab.h"

// Forward Declarations
/* none */

// Workspace Tab
class WorkspaceTab : public QWidget, public MainTab
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	WorkspaceTab(DUQWindow* duqWindow, DUQ& duq, QTabWidget* parent, const char* title);
	~WorkspaceTab();
	// Main form declaration
	Ui::WorkspaceTab ui;


	/*
	 * Data
	 */
	public:
	// Return whether the title of the tab can be changed
	bool canChangeTitle();


	/*
	 * SubWidget / SubWindow Handling
	 */
	public:
	// Return the tab's SubWindow area, if it has one
	QMdiArea* subWindowArea();
	// Return the tab's SubWidget layout, if it has one
	QLayout* subWidgetLayout();


	/*
	 * Update
	 */
	protected:
	// Update controls in tab
	void updateControls();
	// Disable sensitive controls within tab, ready for main code to run
	void disableSensitiveControls();
	// Enable sensitive controls within tab, ready for main code to run
	void enableSensitiveControls();


	/*
	 * State
	 */
	public:
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser);


	/*
	 * MDI SubWindow Management
	 */
	private:
	// List of all current MDI sub-windows over all tabs
	RefList<SubWindow,bool> allSubWindows_;

	private:
	// Find SubWindow from specified data pointer
	SubWindow* subWindow(void* data);

	public:
	// Remove window for specified data (as pointer) from this widget's MDI area
	bool removeWindowFromMDIArea(void* windowContents);
	// Return window for specified data (as pointer) in this widget's MDI area, if it exists
	QMdiSubWindow* findMDIWindow(void* windowContents);
};

#endif
