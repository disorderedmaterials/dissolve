/*
	*** Main Tab
	*** src/gui/maintab.h
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

#ifndef DUQ_MAINTAB_H
#define DUQ_MAINTAB_H

#include "gui/subwindow.h"
#include "base/charstring.h"
#include "templates/list.h"
#include "templates/reflist.h"

// Forward Declarations
class DUQ;
class LineParser;
class QMdiArea;
class QTabWidget;
class QWidget;

// Base 
class MainTab : public ListItem<MainTab>
{
	public:
	// Constructor / Destructor
	MainTab(DUQ& duq, QTabWidget* parent, const char* name);
	virtual ~MainTab();


	/*
	 * Data
	 */
	protected:
	// Reference to dUQ
	DUQ& duq_;
	// Tab widget in which this tab is contained
	QTabWidget* tabWidget_;
	// Unique name of tab
	CharString name_;

	public:
	// Return MDI area available in tab (if any)
	virtual QMdiArea* mdiArea() = 0;

	public:
	// Return name of tab
	const char* name() const;


	/*
	 * MDI SubWindow Management
	 */
	private:
	// List of all current MDI sub-windows over all tabs
	RefList<SubWindow,bool> allSubWindows_;
	// List of all current MDI sub-windows in this tab
	List<SubWindow> subWindows_;

	private:
	// Find SubWindow from specified data pointer
	SubWindow* subWindow(void* data);

	public:
	// Add window for widget containing specified data (as pointer)
	QMdiSubWindow* addWindowToMDIArea(SubWidget* widget, void* windowContents, const char* windowTitle);
	// Remove window for specified data (as pointer) from this widget's MDI area
	bool removeWindowFromMDIArea(void* windowContents);
	// Return window for specified data (as pointer) in this widget's MDI area, if it exists
	QMdiSubWindow* findMDIWindow(void* windowContents);
	// Return window with specified title in this widget's MDI area, if it exists
	QMdiSubWindow* findMDIWindow(const char* title);


	/*
	 * Update
	 */
	protected:
	// Update controls in page
	virtual void updatePage() = 0;
	// Disable sensitive controls within page, ready for main code to run
	virtual void disableSensitiveControlsInPage() = 0;
	// Enable sensitive controls within page, ready for main code to run
	virtual void enableSensitiveControlsInPage() = 0;

	public:
	// Update all controls in the tab
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
};

#endif
