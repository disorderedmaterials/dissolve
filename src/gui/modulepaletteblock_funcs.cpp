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
#include <QFile>
#include <QPainter>

// Constructor
ModulePaletteBlock::ModulePaletteBlock(QWidget* parent, DissolveWindow* dissolveWindow, ModuleReference* modRef) : QWidget(parent), dissolveWindow_(dissolveWindow), dissolve_(dissolveWindow->dissolve())
{
	// Set up user interface
	ui.setupUi(this);

	setAutoFillBackground(true);

	moduleReference_ = modRef;

	module_ = moduleReference_ ? moduleReference_->module() : NULL;

	initialiseWindow(module_);
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
		ui.TextLabel->setText(module->type());

		// Construct the name of the icon for this module in our resource file
		CharString iconName(":/modules/icons/modules_%s.svg", DissolveSys::lowerCase(module->type()));
		if (QFile::exists(iconName.get())) ui.IconLabel->setPixmap(QPixmap(iconName.get()));
		else ui.IconLabel->setPixmap(QPixmap(":/modules/icons/modules_generic.svg"));
	}
	else
	{
		ui.TextLabel->setText("???");
		ui.IconLabel->setPixmap(QPixmap(":/modules/icons/modules_generic.svg"));
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
