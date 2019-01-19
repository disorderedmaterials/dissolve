/*
	*** Palette Block Widget - Functions
	*** src/gui/paletteblock_funcs.cpp
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

#include "gui/paletteblock.h"
#include "gui/gui.h"
#include "gui/keywordwidgets.h"
#include "main/dissolve.h"
#include "classes/configuration.h"
#include <QPainter>

// Constructor
PaletteBlock::PaletteBlock(QWidget* parent, DissolveWindow* dissolveWindow, ModuleReference* modRef) : QWidget(parent), dissolveWindow_(dissolveWindow), dissolve_(dissolveWindow->dissolve())
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

PaletteBlock::~PaletteBlock()
{
}

// Initialise window
void PaletteBlock::initialiseWindow(Module* module)
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
ModuleReference* PaletteBlock::moduleReference()
{
	return moduleReference_;
}

/*
 * QWidget Reimplementations
 */

// Paint event
void PaletteBlock::paintEvent(QPaintEvent* event)
{
	if (!module_) return;

	QPainter painter(this);

	const int* colour = module_->colour();
	QColor blockColour(colour[0], colour[1], colour[2], 50);

	QLinearGradient linearGrad(QPointF(0, 0), QPointF(100, 50));
	linearGrad.setColorAt(0, blockColour);
	linearGrad.setColorAt(1, QColor(0,0,0,0));
	painter.fillRect(QRect(0, 0, width(), height()), linearGrad);
}

/*
 * Widget Functions
 */

// Update controls within widget
void PaletteBlock::updateControls()
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
void PaletteBlock::disableSensitiveControls()
{
}

// Enable sensitive controls, ready for main code to run
void PaletteBlock::enableSensitiveControls()
{
}
