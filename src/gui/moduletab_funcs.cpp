/*
	*** ModuleTab Functions
	*** src/gui/moduletab_funcs.cpp
	Copyright T. Youngs 2012-2018

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/moduletab.h"
#include "gui/gui.h"
#include "gui/modulewidget.h"
#include "gui/widgets/nocontrols.h"
#include "main/duq.h"
#include "base/lineparser.h"

// Constructor / Destructor
ModuleTab::ModuleTab(DUQWindow* duqWindow, DUQ& duq, QTabWidget* parent, const char* title, Module* module) : MainTab(duqWindow, duq, parent, module->uniqueName(), this), module_(module)
{
	ui.setupUi(this);

	refreshing_ = false;

	// Set up our keywords widget
	ui.KeywordsFrame->setUp(duqWindow_, module_);

	// Create the module widget (if this Module has one)
	QVBoxLayout* widgetLayout = new QVBoxLayout(ui.WidgetWidget);
	widgetLayout->setContentsMargins(0,0,0,0);
	widgetLayout->setSpacing(0);
	moduleWidget_ = module->createWidget(ui.WidgetWidget, duq);
	if (moduleWidget_ == NULL)
	{
		NoControlsWidget* ncw = new NoControlsWidget;
		widgetLayout->addWidget(ncw);
	}
	else widgetLayout->addWidget(moduleWidget_);

	updateControls();
}

ModuleTab::~ModuleTab()
{
}

/*
 * Update
 */

// Update header texts
void ModuleTab::updateHeaderTexts()
{
	CharString topText("%s (%s)", module_->name(), module_->uniqueName());
	ui.TopLabel->setText(topText.get());
	CharString bottomText("Runs @ %s", module_->frequencyDetails(duq_.iteration()));
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
		if (configIterator.first()) toolTip.strcatf("%s", cfg->name());
		else toolTip.strcatf(", %s", cfg->name());
	}
	ui.HeaderFrame->setToolTip(toolTip.get());

	// Set button status
	ui.ToggleKeywordsButton->setChecked(ui.KeywordsGroup->isVisible());
	ui.EnabledButton->setChecked(module_->enabled());

	// Update frequency spin
	ui.FrequencySpin->setValue(module_->frequency());

	// Update keyword widget
	ui.KeywordsFrame->updateControls();

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
 * State
 */

// Write widget state through specified LineParser
bool ModuleTab::writeState(LineParser& parser)
{
	return true;
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

	module_->executeMainProcessing(duq_, duq_.worldPool());

	emit(moduleRun());
}

void ModuleTab::on_EnabledButton_clicked(bool checked)
{
	if (refreshing_) return;

	module_->setEnabled(checked);

	duqWindow_->setModified();
}

void ModuleTab::on_FrequencySpin_valueChanged(int value)
{
	if (refreshing_) return;

	module_->setFrequency(value);

	duqWindow_->setModified();

	updateHeaderTexts();
}
