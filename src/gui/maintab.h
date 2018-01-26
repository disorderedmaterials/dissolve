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
#include "module/module.h"
#include "base/charstring.h"
#include "templates/list.h"
#include "templates/reflist.h"

// Forward Declarations
class DUQ;
class DUQWindow;
class LineParser;
class QLayout;
class QMdiArea;
class QTabWidget;
class QWidget;

// Base 
class MainTab : public ListItem<MainTab>
{
	public:
	// Constructor / Destructor
	MainTab(DUQWindow* duqWindow, DUQ& duq, QTabWidget* parent, const char* title, QWidget* page);
	virtual ~MainTab();


	/*
	 * Data
	 */
	protected:
	// Reference to dUQ
	DUQ& duq_;
	// Pointer to main window
	DUQWindow* duqWindow_;
	// Tab widget in which this tab is contained
	QTabWidget* tabWidget_;
	// Unique titla (name) of tab
	CharString title_;

	public:
	// Return title of tab
	const char* title() const;


	/*
	 * SubWidget / SubWindow Handling
	 */
	public:
	// Return whether the tab has a SubWindow area
	virtual bool hasSubWindowArea() = 0;
	// Add SubWindow for widget containing specified data (as pointer)
	virtual QMdiSubWindow* addSubWindow(SubWidget* widget, void* windowContents) = 0;
	// Find and return named SubWidget
	virtual SubWidget* findSubWidget(const char* widgetTitle) = 0;


	/*
	 * Update
	 */
	public:
	// Update controls in tab
	virtual void updateControls() = 0;
	// Disable sensitive controls within the tab, ready for main code to run
	virtual void disableSensitiveControls() = 0;
	// Enable sensitive controls within the tab, ready for main code to run
	virtual void enableSensitiveControls() = 0;


	/*
	 * State
	 */
	public:
	// Write widget state through specified LineParser
	virtual bool writeState(LineParser& parser) = 0;


	/*
	 * Helper Functions
	 */
	public:
	// Add module widgets to specified layout
	void addModuleWidgets(const RefList<Module,bool>& modules, List<SubWidget>& widgets, QLayout* layout);
};

#endif
