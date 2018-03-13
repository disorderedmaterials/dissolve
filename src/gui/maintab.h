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

#include "gui/widgets/subwindow.h"
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
	// Page widget
	QWidget* page_;
	// Unique title (name) of tab
	CharString title_;

	public:
	// Set title of tab
	void setTitle(const char* title);
	// Return title of tab
	const char* title() const;
	// Return whether the title of the tab can be changed
	virtual bool canChangeTitle();


	/*
	 * SubWidget / SubWindow Handling
	 */
	protected:
	// List of all current MDI SubWindows in this tab, if it has an MDI area
	List<SubWindow> subWindows_;
	// List of module SubWidgets, if there is no MDI area
	List<SubWidget> subWidgets_;

	public:
	// Return the tab's SubWindow area, if it has one
	virtual QMdiArea* subWindowArea() = 0;
	// Return the tab's SubWidget layout, if it has one
	virtual QLayout* subWidgetLayout() = 0;
	// Find SubWindow by title
	SubWindow* findSubWindow(const char* title);
	// Find SubWindow by SubWidget
	SubWindow* findSubWindow(SubWidget* subWidget);
	// Find SubWindow by data content
	SubWindow* findSubWindow(void* windowData);
	// Find and return named SubWidget
	SubWidget* findSubWidget(const char* widgetTitle);
	// Add SubWindow for widget containing specified data (as pointer)
	SubWindow* addSubWindow(SubWidget* widget, void* windowContents);
	// Remove SubWindow specified
	void removeSubWindow(SubWindow* window);


	/*
	 * Update
	 */
	protected:
	// Whether the tab is currently refreshing
	bool refreshing_;

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
	void addModuleWidgets(const RefList<Module,bool>& modules, List<SubWidget>& widgets, QLayout* layout, bool allowShiftAndRemove);
};

#endif
