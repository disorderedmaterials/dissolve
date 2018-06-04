/*
	*** ModuleChart Module Block Widget
	*** src/gui/modulechartmoduleblock.h
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

#ifndef DISSOLVE_MODULECHARTMODULEBLOCKWIDGET_H
#define DISSOLVE_MODULECHARTMODULEBLOCKWIDGET_H

#include "gui/ui_modulechartmoduleblock.h"
#include "gui/modulechartblock.h"
#include "templates/reflist.h"

// Forward Declarations
class Dissolve;
class DissolveWindow;
class KeywordWidgetBase;
class Module;
class ModuleReference;

// ModuleChart Module Block Widget
class ModuleChartModuleBlock : public QWidget, public ModuleChartBlock
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	ModuleChartModuleBlock(QWidget* parent, DissolveWindow* dissolveWindow, ModuleReference* moduleReference);
	~ModuleChartModuleBlock();
	// Main form declaration
	Ui::ModuleChartModuleWidget ui;
	// Initialise window
	void initialiseWindow(Module* module);


	/*
	 * Module / Reference Data
	 */
	private:
	// Associated Module
	Module* module_;
	// Reference for associated Module
	ModuleReference* moduleReference_;
	// List of keyword widgets displayed
	RefList<KeywordWidgetBase,bool> keywordWidgets_;

	public:
	// Return reference for associated Module
	ModuleReference* moduleReference();


	/*
	 * QWidget Reimplementations
	 */
	protected:
	// Paint event
	void paintEvent(QPaintEvent* event);


	/*
	 * Widget Functions
	 */
	public:
	// Return underlying widget
	QWidget* widget();
	// Update controls within widget
	void updateControls();
	// Disable sensitive controls, ready for main code to run
	void disableSensitiveControls();
	// Enable sensitive controls, ready for main code to run
	void enableSensitiveControls();

	public slots:
	void on_ToggleKeywordsButton_clicked(bool checked);
	void on_RemoveButton_clicked(bool checked);
	void on_RunButton_clicked(bool checked);
	void on_EnabledButton_clicked(bool checked);

	signals:
	void settingsToggled();
	void removeModule(void* module);
	void moduleRun();


	/*
	 * Geometry
	 */
	public:
	// Return width of underlying widget
	int widgetWidth() const;
	// Return height of underlying widget
	int widgetHeight() const;
	// Set underlying widget geometry
	void setWidgetGeometry(int left, int top, int width, int height);
	// Return right-hand-side widget anchor point
	QPoint globalRightHandWidgetAnchor() const;
	// Return left-hand-side widget anchor point
	QPoint globalLeftHandWidgetAnchor() const;

};

#endif
