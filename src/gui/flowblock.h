/*
	*** Flow Block Widget
	*** src/gui/flowblock.h
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

#ifndef DISSOLVE_FLOWBLOCKWIDGET_H
#define DISSOLVE_FLOWBLOCKWIDGET_H

#include "gui/ui_flowblock.h"
#include "templates/reflist.h"

// Forward Declarations
class Dissolve;
class DissolveWindow;
class KeywordWidgetBase;
class Module;
class ModuleReference;

// Flow Block Widget
class FlowBlock : public QWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	private:
	// Pointer to DissolveWindow
	DissolveWindow* dissolveWindow_;
	// Reference to Dissolve
	Dissolve& dissolve_;

	public:
	// Constructor / Destructor
	FlowBlock(QWidget* parent, DissolveWindow* dissolveWindow, ModuleReference* moduleReference);
	~FlowBlock();
	// Main form declaration
	Ui::FlowBlockWidget ui;
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
	 * Widget Functions
	 */
	private:
	// Whether the widget is currently refreshing
	bool refreshing_;

	protected:
	void closeEvent(QCloseEvent* event);

	public:
	// Update controls within widget
	void updateControls();
	// Disable sensitive controls, ready for main code to run
	void disableSensitiveControls();
	// Enable sensitive controls, ready for main code to run
	void enableSensitiveControls();
	// Return right-hand-side flow anchor point
	QPoint globalRightHandFlowAnchor() const;
	// Return left-hand-side flow anchor point
	QPoint globalLeftHandFlowAnchor() const;

	public slots:
	void on_ToggleKeywordsButton_clicked(bool checked);
	void on_RemoveButton_clicked(bool checked);
	void on_RunButton_clicked(bool checked);
	void on_EnabledButton_clicked(bool checked);

	signals:
	void settingsToggled();
	void removeModule(void* module);
	void moduleRun();
};

#endif
