/*
	*** Module Control Widget
	*** src/gui/modulecontrolwidget.h
	Copyright T. Youngs 2007-2017

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
#include "templates/list.h"

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
	// Update controls within widget
	void updateControls();


	/*
	 * Reimplementations
	 */
	protected:
	void closeEvent(QCloseEvent* event);


	/*
	 * Functions
	 */
	private:
	// ModuleWidget displayed in this control widget
	ModuleWidget* moduleWidget_;
	// List of keyword widgets displayed
	List<KeywordWidgetBase> keywordWidgets_;

	private:
	// Populate list of Module options
	void setUpOptions();
	// Update Module keyword widgets from stored values
	void updateKeywordWidgets();


	/*
	 * Widget Signals / Slots
	 */
	signals:
	void windowClosed(void*);
};

#endif
