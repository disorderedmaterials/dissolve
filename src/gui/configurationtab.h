/*
	*** Configuration Tab
	*** src/gui/configurationtab.h
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

#ifndef DUQ_CONFIGURATIONTAB_H
#define DUQ_CONFIGURATIONTAB_H

#include "gui/ui_configurationtab.h"
#include "gui/maintab.h"
#include "gui/subwidget.h"

// Forward Declarations
class Configuration;

// Configuration Tab
class ConfigurationTab : public QWidget, public MainTab
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	ConfigurationTab(DUQWindow* duqWindow, DUQ& duq, QTabWidget* parent, const char* title, Configuration* cfg);
	~ConfigurationTab();
	// Main form declaration
	Ui::ConfigurationTab ui;


	/*
	 * Target
	 */
	private:
	Configuration* configuration_;


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
};

#endif
