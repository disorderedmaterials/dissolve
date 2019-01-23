/*
	*** ModuleChart Module Block Widget
	*** src/gui/modulechartmoduleblock.h
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

// ModuleChart Module Block Widget
class ModuleChartModuleBlock : public QWidget, public ModuleChartBlock
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	ModuleChartModuleBlock(QWidget* parent, DissolveWindow* dissolveWindow, Module* module);
	~ModuleChartModuleBlock();
	// Main form declaration
	Ui::ModuleChartModuleWidget ui;


	/*
	 * Module / Reference Data
	 */
	private:
	// Associated Module
	Module* module_;
	// List of keyword widgets displayed
	RefList<KeywordWidgetBase,bool> keywordWidgets_;

	public:
	// Return associated Module
	Module* module();


	/*
	 * QWidget Reimplementations
	 */
	protected:
	// Paint event
	void paintEvent(QPaintEvent* event);


	/*
	 * Block Type
	 */
	public:
	// Return type of this block
	ModuleChartBlock::ModuleChartBlockType blockType();


	/*
	 * Widget Functions
	 */
	public:
	// Return underlying widget
	QWidget* widget();
	// Update controls within widget
	void updateControls();
	// Return suitable QPixmap for supplied Module
	static QPixmap modulePixmap(const Module* module);
	// Return suitable QPixmap for supplied Module type
	static QPixmap modulePixmap(QString moduleType);
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
};

#endif
