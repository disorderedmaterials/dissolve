/*
	*** ModuleTab Functions
	*** src/gui/moduletab_funcs.cpp
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

#include "gui/moduletab.h"
#include "gui/gui.h"
#include "gui/modulewidget.h"
#include "gui/widgets/nocontrols.h"
#include "main/dissolve.h"
#include "base/lineparser.h"

// Constructor / Destructor
ModuleTab::ModuleTab(DissolveWindow* dissolveWindow, Dissolve& dissolve, QTabWidget* parent, const char* title, Module* module) : MainTab(dissolveWindow, dissolve, parent, module->uniqueName(), this), module_(module)
{
	ui.setupUi(this);

	refreshing_ = false;

	initialiseControls(module_);

	updateControls();
}

ModuleTab::~ModuleTab()
{
}

/*
 * Data
 */

// Return tab type
const char* ModuleTab::tabType() const
{
	return "ModuleTab";
}

/*
 * Update
 */

// Initialise controls for the specified Module
void ModuleTab::initialiseControls(Module* module)
{
	if (!module) return;

	// Set up our keywords widget
	ui.KeywordsWidget->setUp(dissolveWindow_, module);

	// Create the module widget (if this Module has one)
	QVBoxLayout* widgetLayout = new QVBoxLayout(ui.WidgetWidget);
	widgetLayout->setContentsMargins(0,0,0,0);
	widgetLayout->setSpacing(0);
	moduleWidget_ = module->createWidget(ui.WidgetWidget, dissolve_);
	if (moduleWidget_ == NULL)
	{
		NoControlsWidget* ncw = new NoControlsWidget(ui.WidgetWidget);
		widgetLayout->addWidget(ncw);
	}
	else widgetLayout->addWidget(moduleWidget_);
}

// Update header texts
void ModuleTab::updateHeaderTexts()
{
	CharString topText("%s (%s)", module_->type(), module_->uniqueName());
	ui.TopLabel->setText(topText.get());
	CharString bottomText("Runs @ %s", module_->frequencyDetails(dissolve_.iteration()));
	ui.BottomLabel->setText(bottomText.get());
}

// Update controls in tab
void ModuleTab::updateControls()
{
	if (!module_) return;

	refreshing_ = true;

	// Update header labels
	updateHeaderTexts();

	// Make sure tooltip on HeaderFrame is up-to-date
	CharString toolTip("Targets: ");
	RefListIterator<Configuration,bool> configIterator(module_->targetConfigurations());
	while (Configuration* cfg = configIterator.iterate())
	{
		if (configIterator.isFirst()) toolTip.strcatf("%s", cfg->name());
		else toolTip.strcatf(", %s", cfg->name());
	}
	ui.HeaderFrame->setToolTip(toolTip.get());

	// Set button status
	ui.ToggleKeywordsButton->setChecked(ui.KeywordsGroup->isVisible());
	ui.EnabledButton->setChecked(module_->enabled());

	// Update frequency spin
	ui.FrequencySpin->setValue(module_->frequency());

	// Update keyword widget
	ui.KeywordsWidget->updateControls();

	// Update module control widget (if there is one)
	if (moduleWidget_) moduleWidget_->updateControls();

	refreshing_ = false;
}

// Disable sensitive controls within tab, ready for main code to run
void ModuleTab::disableSensitiveControls()
{
	// Disable control widgets
	ui.RunButton->setEnabled(false);
	ui.EnabledButton->setEnabled(false);
	ui.FrequencySpin->setEnabled(false);

	// Disable keywords widget
	ui.KeywordsGroup->setEnabled(false);

	// Disable module control widget (if there is one)
	if (moduleWidget_) moduleWidget_->disableSensitiveControls();
}

// Enable sensitive controls within tab, ready for main code to run
void ModuleTab::enableSensitiveControls()
{
	// Enable control widgets
	ui.RunButton->setEnabled(true);
	ui.EnabledButton->setEnabled(true);
	ui.FrequencySpin->setEnabled(true);

	// Enable keywords widget
	ui.KeywordsGroup->setEnabled(true);

	// Enable module control widget (if there is one)
	if (moduleWidget_) moduleWidget_->enableSensitiveControls();
}

/*
 * Widget Functions
 */

void ModuleTab::on_ToggleKeywordsButton_clicked(bool checked)
{
	if (refreshing_) return;

	ui.KeywordsGroup->setVisible(checked);
}

void ModuleTab::on_RunButton_clicked(bool checked)
{
	if (!module_) return;

	module_->executeProcessing(dissolve_, dissolve_.worldPool());

	updateControls();

	emit(moduleRun());
}

void ModuleTab::on_EnabledButton_clicked(bool checked)
{
	if (refreshing_) return;

	module_->setEnabled(checked);

	dissolveWindow_->setModified();
}

void ModuleTab::on_FrequencySpin_valueChanged(int value)
{
	if (refreshing_) return;

	module_->setFrequency(value);

	dissolveWindow_->setModified();

	updateHeaderTexts();
}

/*
 * State
 */

// Write widget state through specified LineParser
bool ModuleTab::writeState(LineParser& parser)
{
	// Write state information for the tab : KeywordsShown(bool)
	if (!parser.writeLineF("%s\n", DissolveSys::btoa(ui.ToggleKeywordsButton->isChecked()))) return false;

	// Write any state information associated with the displayed ModuleWidget
	if (module_ && moduleWidget_)
	{
		if (!moduleWidget_->writeState(parser)) return false;
	}

	return true;
}

// Read widget state through specified LineParser
bool ModuleTab::readState(LineParser& parser)
{
	// Read state information for the tab : KeywordsShown(bool)
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	bool keywordsVisible = parser.argb(0);
	ui.ToggleKeywordsButton->setChecked(keywordsVisible);
	ui.KeywordsGroup->setVisible(keywordsVisible);

	// Read any state information associated with the displayed ModuleWidget
	if (module_ && moduleWidget_)
	{
		if (!moduleWidget_->readState(parser)) return false;
	}

	return true;
}
