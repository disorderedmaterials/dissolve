/*
	*** ModuleChart Module Block Widget - Functions
	*** src/gui/modulechartmoduleblock_funcs.cpp
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

#include "gui/modulechartmoduleblock.h"
#include "gui/modulechartmetrics.h"
#include "gui/gui.h"
#include "gui/keywordwidgets.h"
#include "main/dissolve.h"
#include "classes/configuration.h"
#include <QPainter>

// Constructor
ModuleChartModuleBlock::ModuleChartModuleBlock(QWidget* parent, DissolveWindow* dissolveWindow, ModuleReference* modRef) : QWidget(parent), ModuleChartBlock(dissolveWindow, dissolveWindow->dissolve())
{
	// Set up user interface
	ui.setupUi(this);

	// Set necessary values on the widget itself
	ModuleChartMetrics metrics;
	setContentsMargins(metrics.blockMargins());
// 	setAutoFillBackground(true);

	// Hide the keywords control frame to start with
	ui.KeywordsControlFrame->setVisible(false);

	// Set ModuleReference and Module pointers
	moduleReference_ = modRef;
	module_ = moduleReference_ ? moduleReference_->module() : NULL;

	// Set up our keywords widget
	ui.KeywordsWidget->setUp(dissolveWindow_, module_);

	updateControls();
}

ModuleChartModuleBlock::~ModuleChartModuleBlock()
{
}

/*
 * Module / Reference Data
 */

// Return reference for associated Module
ModuleReference* ModuleChartModuleBlock::moduleReference()
{
	return moduleReference_;
}

/*
 * QWidget Reimplementations
 */

// Paint event
void ModuleChartModuleBlock::paintEvent(QPaintEvent* event)
{
	if (!module_) return;

	ModuleChartMetrics metrics;

	QPainter painter(this);
	
	QPen borderPen;
	borderPen.setWidth(metrics.blockBorderWidth());
	painter.setPen(borderPen);

	QPainterPath borderPath;
	borderPath.moveTo(metrics.blockBorderMidPoint(), metrics.blockBorderMidPoint());
	borderPath.lineTo(metrics.blockBorderMidPoint(), metrics.blockDentOffset());
	borderPath.arcTo(metrics.blockBorderMidPoint() - metrics.blockDentRadius(), metrics.blockDentOffset()+metrics.blockBorderMidPoint(), metrics.blockDentRadius()*2, metrics.blockDentRadius()*2, 90, -180);
	borderPath.lineTo(metrics.blockBorderMidPoint(), height() - metrics.blockBorderWidth());
	borderPath.lineTo(width()-metrics.blockBorderWidth(), height() - metrics.blockBorderWidth());
	borderPath.lineTo(width()-metrics.blockBorderWidth(), metrics.blockBorderMidPoint());
	borderPath.closeSubpath();

	const int* colour = module_->colour();
	QColor blockColour = QColor(colour[0], colour[1], colour[2]).lighter(200);
	QLinearGradient linearGrad(QPointF(0, 0), QPointF(100, 50));
	linearGrad.setColorAt(0, blockColour);
	linearGrad.setColorAt(1, Qt::white);
	painter.setBrush(linearGrad);
// 	painter.setBrush(Qt::white);

	// Ready - draw the border + fill!
	painter.drawPath(borderPath);
}

/*
 * Block Type
 */

// Return type of this block
ModuleChartBlock::ModuleChartBlockType ModuleChartModuleBlock::blockType()
{
	return ModuleChartBlock::ModuleBlockType;
}

/*
 * Widget Functions
 */

// Return underlying widget
QWidget* ModuleChartModuleBlock::widget()
{
	return this;
}

// Update controls within widget
void ModuleChartModuleBlock::updateControls()
{
	if (!module_) return;

	// Set information panel contents
	CharString topText("%s (%s)", module_->type(), module_->uniqueName());
	ui.TopLabel->setText(topText.get());
	CharString bottomText("Runs @ %s", module_->frequencyDetails(dissolve_.iteration()));
	ui.BottomLabel->setText(bottomText.get());
	
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
	ui.EnabledButton->setChecked(module_->enabled());

	// Update keywords
	ui.KeywordsWidget->updateControls();
}

// Disable sensitive controls, ready for main code to run
void ModuleChartModuleBlock::disableSensitiveControls()
{
	ui.KeywordsControlFrame->setEnabled(false);
	ui.RunButton->setEnabled(false);
	ui.EnabledButton->setEnabled(false);
	ui.RemoveButton->setEnabled(false);
}

// Enable sensitive controls, ready for main code to run
void ModuleChartModuleBlock::enableSensitiveControls()
{
	ui.KeywordsControlFrame->setEnabled(true);
	ui.RunButton->setEnabled(true);
	ui.EnabledButton->setEnabled(true);
	ui.RemoveButton->setEnabled(true);
}

void ModuleChartModuleBlock::on_ToggleKeywordsButton_clicked(bool checked)
{
	ui.KeywordsControlFrame->setVisible(checked);

	adjustSize();
	updateGeometry();

	emit(settingsToggled());
}

void ModuleChartModuleBlock::on_RemoveButton_clicked(bool checked)
{
	emit (removeModule(moduleReference_));
}

void ModuleChartModuleBlock::on_RunButton_clicked(bool checked)
{
	if (!module_) return;

	module_->executeProcessing(dissolve_, dissolve_.worldPool());

	updateControls();

	emit moduleRun();
}

void ModuleChartModuleBlock::on_EnabledButton_clicked(bool checked)
{
	if (refreshing_) return;

	if (!module_) return;

	module_->setEnabled(checked);

	dissolveWindow_->setModified();
}

/*
 * Geometry
 */

// Return width of underlying widget
int ModuleChartModuleBlock::widgetWidth() const
{
	return minimumSize().width();
}

// Return height of underlying widget
int ModuleChartModuleBlock::widgetHeight() const
{
	return minimumSize().height();
}

// Set underlying widget geometry
void ModuleChartModuleBlock::setWidgetGeometry(int left, int top, int width, int height)
{
	setGeometry(left, top, width, height);
}
