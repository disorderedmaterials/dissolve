/*
	*** Main Tab
	*** src/gui/maintab.h
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

#ifndef DISSOLVE_MAINTAB_H
#define DISSOLVE_MAINTAB_H

#include "gui/widgets/subwindow.h"
#include "module/module.h"
#include "module/reference.h"
#include "base/charstring.h"
#include "templates/list.h"
#include "templates/reflist.h"

// Forward Declarations
class Dissolve;
class DissolveWindow;
class LineParser;
class QLayout;
class QMdiArea;
class QTabWidget;
class QWidget;

// Base 
class MainTab
{
	public:
	// Constructor / Destructor
	MainTab(DissolveWindow* dissolveWindow, Dissolve& dissolve, QTabWidget* parent, const char* title, QWidget* page);
	virtual ~MainTab();


	/*
	 * Data
	 */
	protected:
	// Reference to Dissolve
	Dissolve& dissolve_;
	// Pointer to main window
	DissolveWindow* dissolveWindow_;
	// Tab widget in which this tab is contained
	QTabWidget* tabWidget_;
	// Page widget
	QWidget* page_;
	// Unique title (name) of tab
	CharString title_;

	public:
	// Return tab type
	virtual const char* tabType() const = 0;
	// Return page widget
	QWidget* page() const;
	// Set title of tab
	void setTitle(const char* title);
	// Return title of tab
	const char* title() const;
	// Return whether the title of the tab can be changed
	virtual bool canChangeTitle() const;


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
	// Read widget state through specified LineParser
	virtual bool readState(LineParser& parser) = 0;
};

#endif
