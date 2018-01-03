/*
	*** Module Control Widget
	*** src/gui/modulecontrolwidget.h
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

#ifndef DUQ_MODULECONTROLWIDGET_H
#define DUQ_MODULECONTROLWIDGET_H

#include "gui/ui_modulecontrolwidget.h"
#include "gui/subwidget.h"
#include "gui/keywordwidgets/base.h"
#include "templates/reflist.h"

// Forward Declarations
class DUQ;
class Module;
class ModuleWidget;

// Module Control Widget
class ModuleControlWidget : public SubWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	private:
	// Associated Module
	Module* module_;
	// Reference to dUQ
	DUQ& duq_;

	public:
	// Constructor / Destructor
	ModuleControlWidget(QWidget* parent, Module* module, DUQ& dUQ);
	~ModuleControlWidget();
	// Main form declaration
	Ui::ModuleControlWidget ui;
	// Initialise window
	void initialiseWindow(Module* module);
	// Initialise controls
	void initialiseControls(Module* module);


	/*
	 * SubWidget Reimplementations / Definitions
	 */
	protected:
	void closeEvent(QCloseEvent* event);

	public:
	// Update controls within widget
	void updateControls();
	// Disable sensitive controls within widget, ready for main code to run
	void disableSensitiveControls();
	// Enable sensitive controls within widget, ready for main code to run
	void enableSensitiveControls();
	// Return string specifying widget type
	const char* widgetType();
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser);
	// Read widget state through specified LineParser
	bool readState(LineParser& parser);


	/*
	 * Functions
	 */
	private:
	// ModuleWidget displayed in this control widget
	ModuleWidget* moduleWidget_;
	// List of keyword widgets displayed
	RefList<KeywordWidgetBase,bool> keywordWidgets_;


	/*
	 * Widget Signals / Slots
	 */
	signals:
	void windowClosed(void*);

	public slots:
	void on_EnabledCheck_clicked(bool checked);
	void on_FrequencySpin_valueChanged(int value);
};

#endif
