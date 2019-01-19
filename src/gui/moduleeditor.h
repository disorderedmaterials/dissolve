/*
	*** Module Editor
	*** src/gui/moduleeditor.h
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

#ifndef DISSOLVE_MODULEEDITOR_H
#define DISSOLVE_MODULEEDITOR_H

#include "gui/ui_moduleeditor.h"

// Forward Declarations
class DissolveWindow;
class LineParser;
class ModuleChart;
class ModuleLayer;
class ModulePalette;

// ModuleEditor
class ModuleEditor : public QWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	ModuleEditor(QWidget* parent = NULL);
	~ModuleEditor();
	// Main form declaration
	Ui::ModuleEditor ui;


	/*
	 * Setup
	 */
	public:
	// Set up the ModuleEditor for the specified ModuleLayer
	bool setUp(DissolveWindow* dissolveWindow, ModuleLayer* moduleLayer);


	/*
	 * Update
	 */
	private:
	// Whether the widget is currently refreshing
	bool refreshing_;

	public:
	// Update controls
	void updateControls();
	// Disable sensitive controls within widget, ready for main code to run
	void disableSensitiveControls();
	// Enable sensitive controls within widget, ready for main code to run
	void enableSensitiveControls();


	/*
	 * Widget Functions
	 */
	private:
	// Chart widget being displayed
	ModuleChart* chartWidget_;
	// Palette widget
	ModulePalette* paletteWidget_;


	/*
	 * State
	 */
	public:
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser);
	// Read widget state through specified LineParser
	bool readState(LineParser& parser);
};

#endif
