/*
	*** Flow Block Widget - Functions
	*** src/gui/flowblock_funcs.cpp
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

#include "gui/flowblock.h"
#include "gui/gui.h"
#include "gui/keywordwidgets.h"
#include "main/dissolve.h"
#include "classes/configuration.h"
#include <QPainter>

// Constructor
FlowBlock::FlowBlock(QWidget* parent, DissolveWindow* dissolveWindow, ModuleReference* modRef) : QWidget(parent), dissolveWindow_(dissolveWindow), dissolve_(dissolveWindow->dissolve())
{
	// Set up user interface
	ui.setupUi(this);

	ui.KeywordsWidget->setVisible(false);

	moduleReference_ = modRef;

	module_ = moduleReference_ ? moduleReference_->module() : NULL;

	refreshing_ = false;

	initialiseWindow(module_);

	updateControls();
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
		CharString bottomText("Runs @ %s", module->frequencyDetails(dissolve_.iteration()));
		ui.BottomLabel->setText(bottomText.get());
	}
	else
	{
		ui.TopLabel->setText("? (?) @ ?");
		ui.BottomLabel->setText("Runs @ ");
	}

	// Set up our keywords widget
	ui.KeywordsWidget->setUp(dissolveWindow_, module_);
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
 * QWidget Reimplementations
 */

// Paint event
void FlowBlock::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	painter.setBrush(QBrush(Qt::red, Qt::SolidPattern));
	painter.drawRect(0, 0, width(), height());
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
	ui.EnabledButton->setChecked(module_->enabled());

	// Update keywords
	ui.KeywordsWidget->updateControls();
}

// Disable sensitive controls, ready for main code to run
void FlowBlock::disableSensitiveControls()
{
	ui.KeywordsWidget->setEnabled(false);
	ui.RunButton->setEnabled(false);
	ui.EnabledButton->setEnabled(false);
	ui.RemoveButton->setEnabled(false);
}

// Enable sensitive controls, ready for main code to run
void FlowBlock::enableSensitiveControls()
{
	ui.KeywordsWidget->setEnabled(true);
	ui.RunButton->setEnabled(true);
	ui.EnabledButton->setEnabled(true);
	ui.RemoveButton->setEnabled(true);
}

// Return right-hand-side flow anchor point
QPoint FlowBlock::globalRightHandFlowAnchor() const
{
	QPoint X = mapToGlobal(rect().topRight() + QPoint(1, 0));
	QPoint Y = mapToGlobal(ui.HeaderFrame->rect().topRight()) + QPoint(0, ui.HeaderFrame->height()/2);
	return QPoint(X.x(), Y.y());
}

// Return left-hand-side flow anchor point
QPoint FlowBlock::globalLeftHandFlowAnchor() const
{
	QPoint X = mapToGlobal(rect().topLeft() + QPoint(-1, 0));
	QPoint Y = mapToGlobal(ui.HeaderFrame->rect().topLeft()) + QPoint(0, ui.HeaderFrame->height()/2);
	return QPoint(X.x(), Y.y());
}

void FlowBlock::on_ToggleKeywordsButton_clicked(bool checked)
{
	ui.KeywordsWidget->setVisible(checked);

	adjustSize();
	updateGeometry();

	emit(settingsToggled());
}

void FlowBlock::on_RemoveButton_clicked(bool checked)
{
	emit (removeModule(moduleReference_));
}

void FlowBlock::on_RunButton_clicked(bool checked)
{
	if (!module_) return;

	module_->executeMainProcessing(dissolve_, dissolve_.worldPool());

	updateControls();

	emit moduleRun();
}

void FlowBlock::on_EnabledButton_clicked(bool checked)
{
	if (refreshing_) return;

	if (!module_) return;

	module_->setEnabled(checked);

	dissolveWindow_->setModified();
}

// void FlowBlock::on_FrequencySpin_valueChanged(int value)
// {
// 	if (refreshing_) return;
// 
// 	if (!module_) return;
// 	
// 	module_->setFrequency(value);
// 
// 	dissolveWindow_->setModified();
// }
