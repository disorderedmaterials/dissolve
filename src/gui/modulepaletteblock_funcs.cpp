/*
	*** Module Palette Block Widget - Functions
	*** src/gui/modulepaletteblock_funcs.cpp
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

#include "gui/modulepaletteblock.h"
#include "gui/gui.h"
#include "gui/keywordwidgets.h"
#include "main/dissolve.h"
#include "classes/configuration.h"
#include <QPainter>

// Constructor
ModulePaletteBlock::ModulePaletteBlock(QWidget* parent, DissolveWindow* dissolveWindow, ModuleReference* modRef) : QWidget(parent), dissolveWindow_(dissolveWindow), dissolve_(dissolveWindow->dissolve())
{
	// Set up user interface
	ui.setupUi(this);

	setAutoFillBackground(true);

	moduleReference_ = modRef;

	module_ = moduleReference_ ? moduleReference_->module() : NULL;

	refreshing_ = false;

	initialiseWindow(module_);

	updateControls();
}

ModulePaletteBlock::~ModulePaletteBlock()
{
}

// Initialise window
void ModulePaletteBlock::initialiseWindow(Module* module)
{
	// Set information panel contents
	if (module)
	{
		CharString topText("%s (%s)", module->type(), module->uniqueName());
		ui.TopLabel->setText(topText.get());
		CharString bottomText("Runs @ %s", module->frequencyDetails(dissolve_.iteration()));
		ui.BottomLabel->setText(bottomText.get());
	}
	else
	{
		ui.TopLabel->setText("? (?) @ ?");
		ui.BottomLabel->setText("Runs @ ");
	}
}

/*
 * Module / Reference Data
 */

// Return reference for associated Module
ModuleReference* ModulePaletteBlock::moduleReference()
{
	return moduleReference_;
}

/*
 * Widget Functions
 */

// Update controls within widget
void ModulePaletteBlock::updateControls()
{
	if (!module_) return;

	// Make sure tooltip on HeaderFrame is up-to-date
	CharString toolTip("Targets: ");
	RefListIterator<Configuration,bool> configIterator(module_->targetConfigurations());
	while (Configuration* cfg = configIterator.iterate())
	{
		if (configIterator.isFirst()) toolTip.strcatf("%s", cfg->name());
		else toolTip.strcatf(", %s", cfg->name());
	}
	ui.HeaderFrame->setToolTip(toolTip.get());
}

// Disable sensitive controls, ready for main code to run
void ModulePaletteBlock::disableSensitiveControls()
{
}

// Enable sensitive controls, ready for main code to run
void ModulePaletteBlock::enableSensitiveControls()
{
}
