/*
	*** ModuleTab Functions
	*** src/gui/moduletab_funcs.cpp
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

#include "gui/moduletab.h"
#include "gui/gui.h"
#include "gui/modulechartmoduleblock.h"
#include "gui/modulewidget.h"
#include "gui/widgets/nocontrols.h"
#include "main/dissolve.h"
#include "base/lineparser.h"

// Constructor / Destructor
ModuleTab::ModuleTab(DissolveWindow* dissolveWindow, Dissolve& dissolve, QTabWidget* parent, const char* title, Module* module) : ListItem<ModuleTab>(), MainTab(dissolveWindow, dissolve, parent, module->uniqueName(), this), module_(module)
{
	ui_.setupUi(this);

	controlsWidget_ = NULL;
	moduleWidget_ = NULL;
	splitter_ = new QSplitter(Qt::Horizontal, this);

	// Create a layout, add the splitter, and add it to the window
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setMargin(4);
	layout->addWidget(splitter_);
	setLayout(layout);

	refreshing_ = false;

	initialiseControls(module_);

	updateControls();
}

ModuleTab::~ModuleTab()
{
}

/*
 * MainTab Reimplementations
 */

// Return tab type
MainTab::TabType ModuleTab::type() const
{
	return MainTab::ModuleTabType;
}

/*
 * Module Target
 */

// Return displayed Module
const Module* ModuleTab::module() const
{
	return module_;
}

// Initialise controls for the specified Module
void ModuleTab::initialiseControls(Module* module)
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

	// Create the controls widget (a ModuleChartModuleBlock)
	controlsWidget_ = new ModuleChartModuleBlock(NULL, dissolveWindow_, module);
	controlsWidget_->setSettingsExpanded(true, true);
	controlsWidget_->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	controlsWidget_->hideRemoveButton();
	controlsWidget_->hideSettingsButton();
	splitter_->addWidget(controlsWidget_);

	// Create a module widget if there are additional GUI elements available for the Module
	moduleWidget_ = module->createWidget(NULL, dissolve_);
	if (moduleWidget_ == NULL) Messenger::printVerbose("Module '%s' did not provide a valid controller widget.\n", module->type());
	else
	{
		moduleWidget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		splitter_->addWidget(moduleWidget_);
		splitter_->setStretchFactor(1, 5);
	}

	// Connect signals/slots
	connect(controlsWidget_, SIGNAL(updateModuleWidget(int)), this, SLOT(updateModuleWidget(int)));
}

/*
 * Update
 */

// Update controls in tab
void ModuleTab::updateControls()
{
	if (!module_) return;

	refreshing_ = true;

	if (controlsWidget_) controlsWidget_->updateControls();
	if (moduleWidget_) moduleWidget_->updateControls();

	refreshing_ = false;
}

// Disable sensitive controls within tab
void ModuleTab::disableSensitiveControls()
{
	if (controlsWidget_) controlsWidget_->disableSensitiveControls();

	if (moduleWidget_) moduleWidget_->disableSensitiveControls();
}

// Enable sensitive controls within tab
void ModuleTab::enableSensitiveControls()
{
	if (controlsWidget_) controlsWidget_->enableSensitiveControls();

	if (moduleWidget_) moduleWidget_->enableSensitiveControls();
}

// Update controls in module widget only
void ModuleTab::updateModuleWidget(int flags)
{
	if (moduleWidget_) moduleWidget_->updateControls(flags);
}

/*
 * State
 */

// Read widget state through specified LineParser
bool ModuleTab::readState(LineParser& parser, const CoreData& coreData)
{
	// Read state information for the tab : CollapsedIndex(int)
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	int collapsedIndex = parser.argi(0);
	QList<int> widgetSizes;
	if ((collapsedIndex == 0) && controlsWidget_) widgetSizes << 0 << 1;
	else if ((collapsedIndex == 1) && moduleWidget_) widgetSizes << 1 << 0;
	else if (collapsedIndex == -1) widgetSizes << 1 << 1;
	splitter_->setSizes(widgetSizes);

	// Read any state information associated with the displayed ModuleWidget
	if (module_ && moduleWidget_)
	{
		if (!moduleWidget_->readState(parser)) return false;
	}

	return true;
}

// Write widget state through specified LineParser
bool ModuleTab::writeState(LineParser& parser)
{
	// Write state information for the tab : CollapsedIndex(int)
	int collapsedIndex = -1;
	QList<int> sizes = splitter_->sizes();
	if (sizes.at(0) == 0) collapsedIndex = 0;
	else if (moduleWidget_ && (sizes.at(1) == 0)) collapsedIndex = 1;
	if (!parser.writeLineF("%i\n", collapsedIndex)) return false;

	// Write any state information associated with the displayed ModuleWidget
	if (module_ && moduleWidget_)
	{
		if (!moduleWidget_->writeState(parser)) return false;
	}

	return true;
}
