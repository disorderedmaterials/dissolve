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
#include "gui/modulechartmoduleblock.h"
#include "gui/modulewidget.h"
#include "gui/widgets/subwindow.h"
#include "module/module.h"
#include "main/dissolve.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include <QGridLayout>
#include <QLabel>

// Constructor
ModuleControlWidget::ModuleControlWidget(DissolveWindow* dissolveWindow, Module* module, const char* title) : SubWidget(dissolveWindow, title), module_(module), dissolveWindow_(dissolveWindow), dissolve_(dissolveWindow->dissolve())
{
	// Set up user interface
	ui.setupUi(this);

	controlsWidget_ = NULL;
	moduleWidget_ = NULL;

	initialiseControls(module_);

	refreshing_ = false;
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
		Messenger::error("Already have a controls widget for this ModuleControlWidget (%s), so will not create another one.\n", title());
		return;
	}

	// Set a nice icon for the window
	setWindowIcon(ModuleChartModuleBlock::modulePixmap(module));

	// Create a splitter into which we'll add our widgets
	QSplitter* splitter = new QSplitter(Qt::Horizontal, this);

	// Create the controls widget (a ModuleChartModuleBlock)
	controlsWidget_ = new ModuleChartModuleBlock(NULL, dissolveWindow_, module);
	controlsWidget_->setSettingsExpanded(true, true);
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

/*
 * SubWidget Reimplementations / Definitions
 */

void ModuleControlWidget::closeEvent(QCloseEvent* event)
{
	emit (windowClosed(title()));
}

// Update controls within widget
void ModuleControlWidget::updateControls()
{
	if (!module_) return;

	refreshing_ = true;

	if (controlsWidget_) controlsWidget_->updateControls();
	if (moduleWidget_) moduleWidget_->updateControls();

	refreshing_ = false;
}

// Disable sensitive controls within widget
void ModuleControlWidget::disableSensitiveControls()
{
	if (controlsWidget_) controlsWidget_->disableSensitiveControls();

	if (moduleWidget_) moduleWidget_->disableSensitiveControls();
}

// Enable sensitive controls within widget
void ModuleControlWidget::enableSensitiveControls()
{
	if (controlsWidget_) controlsWidget_->enableSensitiveControls();

	if (moduleWidget_) moduleWidget_->enableSensitiveControls();
}

// Return string specifying widget type
const char* ModuleControlWidget::widgetType()
{
	return "ModuleControl";
}

// Write widget state through specified LineParser
bool ModuleControlWidget::writeState(LineParser& parser)
{
	if (!module_) return false;

	// Write state data from ModuleWidget (if one exists)
	if (moduleWidget_ && (!moduleWidget_->writeState(parser))) return false;

	return true;
}

// Read widget state through specified LineParser
bool ModuleControlWidget::readState(LineParser& parser)
{
	if (!module_) return false;

	// Read state data from ModuleWidget (if one exists)
	if (moduleWidget_ && (!moduleWidget_->readState(parser))) return false;

	return true;
}
