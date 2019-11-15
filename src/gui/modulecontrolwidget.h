/*
	*** Module Control Widget
	*** src/gui/modulecontrolwidget.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_MODULECONTROLWIDGET_H
#define DISSOLVE_MODULECONTROLWIDGET_H

#include "gui/ui_modulecontrolwidget.h"
#include "gui/widgets/subwidget.h"
#include "gui/keywordwidgets/base.h"
#include "templates/reflist.h"

// Forward Declarations
class Dissolve;
class DissolveWindow;
class Module;
class ModuleWidget;
class ModuleBlock;

// Module Control Widget
class ModuleControlWidget : public SubWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	private:
	// Associated Module
	Module* module_;
	// Pointer to DissolveWindow
	DissolveWindow* dissolveWindow_;
	// Reference to Dissolve
	Dissolve& dissolve_;

	public:
	// Constructor / Destructor
	ModuleControlWidget(DissolveWindow* dissolveWindow, Module* module, const char* title);
	~ModuleControlWidget();


	/*
	 * UI
	 */
	private:	
	// Main form declaration
	Ui::ModuleControlWidget ui;
	// Module control widget displayed
	ModuleBlock* controlsWidget_;
	// ModuleWidget displayed in this control widget
	ModuleWidget* moduleWidget_;

	public:
	// Initialise controls for the specified Module
	void initialiseControls(Module* module);


	/*
	 * SubWidget Reimplementations / Definitions
	 */
	protected:
	void closeEvent(QCloseEvent* event);

	public:
	// Update controls within widget
	void updateControls();
	// Disable sensitive controls within widget
	void disableSensitiveControls();
	// Enable sensitive controls within widget
	void enableSensitiveControls();
	// Return string specifying widget type
	const char* widgetType();
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser);
	// Read widget state through specified LineParser
	bool readState(LineParser& parser);


	/*
	 * Widget Signals / Slots
	 */
	signals:
	void windowClosed(QString windowTitle);
};

#endif
