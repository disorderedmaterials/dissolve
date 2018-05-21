/*
	*** Flow Block Widget - Functions
	*** src/gui/flowblock_funcs.cpp
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

#include "gui/flowblock.h"
#include "gui/gui.h"
#include "main/duq.h"
#include "classes/configuration.h"

// Constructor
FlowBlock::FlowBlock(QWidget* parent, DUQWindow* duqWindow, ModuleReference* modRef) : QWidget(parent), duqWindow_(duqWindow), duq_(duqWindow->duq())
{
	// Set up user interface
	ui.setupUi(this);

	moduleReference_ = modRef;

	module_ = moduleReference_ ? moduleReference_->module() : NULL;

	refreshing_ = false;

	initialiseWindow(module_);

	initialiseControls(module_);
}

FlowBlock::~FlowBlock()
{
}

// Initialise window
void FlowBlock::initialiseWindow(Module* module)
{
	// Set information panel contents
	if (module)
	{
		CharString topText("%s (%s)", module->name(), module->uniqueName());
		ui.TopLabel->setText(topText.get());
		CharString bottomText;
		RefListIterator<Configuration,bool> configIterator(module->targetConfigurations());
		while (Configuration* cfg = configIterator.iterate())
		{
			if (bottomText.isEmpty()) bottomText.sprintf("Targets: %s", cfg->name());
			else bottomText.strcatf(", %s", cfg->name());
		}
		ui.BottomLabel->setText(bottomText.get());
	}
	else
	{
		ui.TopLabel->setText("? (?) @ ?");
		ui.BottomLabel->setText("Targets: ?");
	}
}

// Initialise controls
void FlowBlock::initialiseControls(Module* module)
{
	if (!module) return;
}

/*
 * Module / Reference Data
 */

// Return reference for associated Module
ModuleReference* FlowBlock::moduleReference()
{
	return moduleReference_;
}

/*
 * Widget Functions
 */

void FlowBlock::closeEvent(QCloseEvent* event)
{
}

// Update controls within widget
void FlowBlock::updateControls()
{
	if (!module_) return;

}

// Disable sensitive controls within widget, ready for main code to run
void FlowBlock::disableSensitiveControls()
{
	// Disable control buttons
	ui.RunButton->setEnabled(false);
	ui.RemoveButton->setEnabled(false);

// 	if (moduleWidget_) moduleWidget_->disableSensitiveControls();
}

// Enable sensitive controls within widget, ready for main code to run
void FlowBlock::enableSensitiveControls()
{
	// Enable control buttons
	ui.RunButton->setEnabled(true);
	ui.RemoveButton->setEnabled(true);

// 	if (moduleWidget_) moduleWidget_->enableSensitiveControls();
}

void FlowBlock::on_RemoveButton_clicked(bool checked)
{
	emit (removeModule(moduleReference_));
}

void FlowBlock::on_RunButton_clicked(bool checked)
{
	if (!module_) return;

	module_->executeMainProcessing(duq_, duq_.worldPool());

	emit moduleRun();
}

void FlowBlock::on_EnabledCheck_clicked(bool checked)
{
	if (refreshing_) return;

	if (!module_) return;

	module_->setEnabled(checked);

	duqWindow_->setModified();
}

void FlowBlock::on_FrequencySpin_valueChanged(int value)
{
	if (refreshing_) return;

	if (!module_) return;
	
	module_->setFrequency(value);

	duqWindow_->setModified();
}
