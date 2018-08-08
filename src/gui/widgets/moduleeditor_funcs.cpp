/*
	*** Module Editor Functions
	*** src/gui/widgets/moduleeditor_funcs.cpp
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

#include "gui/widgets/moduleeditor.h"
#include "gui/gui.h"
#include "gui/modulechart.hui"
#include "gui/modulepalette.hui"
#include "main/dissolve.h"
#include "base/lineparser.h"

// Constructor / Destructor
ModuleEditor::ModuleEditor(QWidget* parent) : QWidget(parent)
{
	ui.setupUi(this);

	chartWidget_ = NULL;
	paletteWidget_ = NULL;

	refreshing_ = false;
}

ModuleEditor::~ModuleEditor()
{
}

/*
 * Setup
 */

// Setup up the ModuleEditor for the specified Module list
bool ModuleEditor::setUp(DissolveWindow* dissolveWindow, ModuleList& moduleList)
{
	// Create a ModuleChart widget and set its source list
	chartWidget_ = new ModuleChart(dissolveWindow, moduleList);
	chartWidget_->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Minimum);
	ui.ModuleScrollArea->setWidget(chartWidget_);

	// Create a ModulePalette widget
	paletteWidget_ = new ModulePalette(dissolveWindow);
	ui.PaletteScrollArea->setWidget(paletteWidget_);

	// Hide palette group initially
// 	ui.PaletteGroup->setVisible(false);

	updateControls();
}

/*
 * Update
 */

// Update controls in tab
void ModuleEditor::updateControls()
{
	if (!chartWidget_) return;

	refreshing_ = true;

	chartWidget_->updateControls();

	refreshing_ = false;
}

// Disable sensitive controls within tab, ready for main code to run
void ModuleEditor::disableSensitiveControls()
{
	chartWidget_->disableSensitiveControls();
}

// Enable sensitive controls within tab, ready for main code to run
void ModuleEditor::enableSensitiveControls()
{
	chartWidget_->enableSensitiveControls();
}

/*
 * Widget Functions
 */

/*
 * State
 */

// Write widget state through specified LineParser
bool ModuleEditor::writeState(LineParser& parser)
{
	if ((!chartWidget_) || (!chartWidget_->writeState(parser))) return false;

	return true;
}

// Read widget state through specified LineParser
bool ModuleEditor::readState(LineParser& parser)
{
	if ((!chartWidget_) || (!chartWidget_->readState(parser))) return false;

	return true;
}
