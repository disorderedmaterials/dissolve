/*
	*** Module Control Widget - Functions
	*** src/gui/modulecontrolwidget_funcs.cpp
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

#include "gui/modulecontrolwidget.h"
#include "gui/gui.h"
#include "gui/charts/moduleblock.h"
#include "gui/modulewidget.h"
#include "module/module.h"
#include "main/dissolve.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include <QGridLayout>
#include <QLabel>

// Constructor
ModuleControlWidget::ModuleControlWidget(DissolveWindow* dissolveWindow, Module* module, const char* title) : module_(module), dissolveWindow_(dissolveWindow), dissolve_(dissolveWindow->dissolve())
{
	// Set up user interface
	ui.setupUi(this);

	controlsWidget_ = NULL;
	moduleWidget_ = NULL;

	initialiseControls(module_);
}

ModuleControlWidget::~ModuleControlWidget()
{
}

// Initialise controls for the specified Module
void ModuleControlWidget::initialiseControls(Module* module)
{
	if (!module) return;

	// Check if we have already created a widget...
	if (controlsWidget_)
	{
		Messenger::error("Already have a controls widget for this ModuleControlWidget (%s), so will not create another one.\n", module->uniqueName());
		return;
	}

	// Set a nice icon for the window
	setWindowIcon(ModuleBlock::modulePixmap(module));

	// Create a splitter into which we'll add our widgets
	QSplitter* splitter = new QSplitter(Qt::Horizontal, this);

	// Create the controls widget (a ModuleBlock)
	controlsWidget_ = new ModuleBlock(this, module_, dissolve_);
	controlsWidget_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	splitter->addWidget(controlsWidget_);

	// Create a module widget if there are additional GUI elements available for the Module
	moduleWidget_ = module->createWidget(NULL, dissolve_);
	if (moduleWidget_ == NULL) Messenger::printVerbose("Module '%s' did not provide a valid controller widget.\n", module->type());
	else
	{
		moduleWidget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		splitter->addWidget(moduleWidget_);
		splitter->setStretchFactor(1, 5);
	}

	// Create a layout, add the splitter, and add it to the window
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setMargin(4);
	layout->addWidget(splitter);
	setLayout(layout);
}
