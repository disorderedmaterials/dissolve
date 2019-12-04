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
// #include "gui/keywordwidgets/base.h"
#include "templates/variantpointer.h"

// Forward Declarations
class Dissolve;
class DissolveWindow;
class Module;
class ModuleWidget;
class ModuleBlock;

// Module Control Widget
class ModuleControlWidget : public QWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	ModuleControlWidget(QWidget* parent);
	~ModuleControlWidget();

	private:
	// Whether the widget is currently refreshing
	bool refreshing_;


	/*
	 * Setup
	 */
	public:
	// Set up links to main window
	void setUp(DissolveWindow* dissolveWindow);


	/*
	 * Module Target
	 */
	private:
	// Pointer to Dissolve
	Dissolve* dissolve_;
	// Associated Module
	Module* module_;

	private slots:
	// Run the set-up stage of the associated Module
	void setUpModule();

	public:
	// Set target Module to display
	void setModule(Module* module, Dissolve* dissolve);

	signals:
	void updateModuleWidget(int flags);


	/*
	 * Update
	 */
	public:
	// Update controls within widget
	void updateControls();
	// Disable sensitive controls
	void disableSensitiveControls();
	// Enable sensitive controls
	void enableSensitiveControls();


	/*
	 * UI
	 */
	private:	
	// Main form declaration
	Ui::ModuleControlWidget ui_;

	private slots:
	// Keyword data for Module has been modified
	void keywordDataModified();

	public slots:
	void on_NameEdit_editingFinished();
	void on_NameEdit_returnPressed();
	void on_EnabledButton_clicked(bool checked);
	void on_FrequencySpin_valueChanged(int value);
	void on_ConfigurationTargetList_itemChanged(QListWidgetItem* item);

	signals:
	// Notify that the Module's data has been modified in some way
	void dataModified();
};

#endif
