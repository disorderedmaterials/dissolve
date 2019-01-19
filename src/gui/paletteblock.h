/*
	*** Palette Block Widget
	*** src/gui/paletteblock.h
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

#ifndef DISSOLVE_PALETTEBLOCKWIDGET_H
#define DISSOLVE_PALETTEBLOCKWIDGET_H

#include "gui/ui_paletteblock.h"
#include "templates/reflist.h"

// Forward Declarations
class Dissolve;
class DissolveWindow;
class Module;
class ModuleReference;

// Palette Block Widget
class PaletteBlock : public QWidget
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
	PaletteBlock(QWidget* parent, DissolveWindow* dissolveWindow, ModuleReference* moduleReference);
	~PaletteBlock();
	// Main form declaration
	Ui::PaletteBlockWidget ui;
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
	private:
	// Whether the widget is currently refreshing
	bool refreshing_;

	public:
	// Update controls within widget
	void updateControls();
	// Disable sensitive controls, ready for main code to run
	void disableSensitiveControls();
	// Enable sensitive controls, ready for main code to run
	void enableSensitiveControls();
};

#endif
