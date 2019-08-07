/*
	*** ModuleChart Module Block Widget - Functions
	*** src/gui/modulechartmoduleblock_funcs.cpp
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

#include "gui/modulechartmoduleblock.h"
#include "gui/modulechartmetrics.h"
#include "gui/gui.h"
#include "gui/keywordwidgets.h"
#include "main/dissolve.h"
#include "classes/configuration.h"
#include "templates/variantpointer.h"
#include <QFile>
#include <QPainter>

// Constructor
ProcedureChartNodeBlock::ProcedureChartNodeBlock(QWidget* parent, DissolveWindow* dissolveWindow, Module* module) : QWidget(parent), ModuleChartBlock(dissolveWindow, dissolveWindow->dissolve())
{
	// Set up user interface
	ui.setupUi(this);

	refreshing_ = false;

	// Set necessary values on the widget itself
	ModuleChartMetrics metrics;
	setContentsMargins(metrics.blockMargins());

	// Hide the keywords control frame to start with
	ui.KeywordsControlWidget->setVisible(false);

	// Set Module pointers
	module_ = module;

	// Set up our keywords widget
	ui.KeywordsWidget->setUp(dissolveWindow_, module_);

	// Set the icon label
	ui.IconLabel->setPixmap(modulePixmap(module_));

	// Update our controls
	updateControls();
}

ProcedureChartNodeBlock::~ProcedureChartNodeBlock()
{
}

/*
 * Module
 */

// Return associated Module
Module* ProcedureChartNodeBlock::module()
{
	return module_;
}

/*
 * QWidget Reimplementations
 */

// Paint event
void ProcedureChartNodeBlock::paintEvent(QPaintEvent* event)
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

	painter.setBrush(Qt::white);

	// Ready - draw the border + fill!
	painter.drawPath(borderPath);
}

/*
 * Block Type
 */

// Return type of this block
ModuleChartBlock::ModuleChartBlockType ProcedureChartNodeBlock::blockType()
{
	return ModuleChartBlock::ModuleBlockType;
}

/*
 * Widget Functions
 */

// Return underlying widget
QWidget* ProcedureChartNodeBlock::widget()
{
	return this;
}

// Set whether the settings are expanded or not, and whether this is permanent
void ProcedureChartNodeBlock::setSettingsExpanded(bool expanded, bool permanent)
{
	on_ToggleSettingsButton_clicked(expanded);

	ui.ToggleSettingsButton->setDisabled(permanent);
}

// Hide the remove button (e.g. when shown in a ModuleTab)
void ProcedureChartNodeBlock::hideRemoveButton()
{
	ui.RemoveButton->setVisible(false);
}

// Update controls within widget
void ProcedureChartNodeBlock::updateControls()
{
	if (!module_) return;

	refreshing_ = true;

	// Set information panel contents
	CharString topText("%s (%s)", module_->type(), module_->uniqueName());
	ui.TopLabel->setText(topText.get());
	ui.FrequencyLabel->setText(QString("(%1)").arg(module_->frequencyDetails(dissolve_.iteration())));

	// Set 'enabled' button status
	ui.EnabledButton->setChecked(module_->enabled());

	// Set frequency spin
	ui.FrequencySpin->setValue(module_->frequency());

	// Update Configuration list and HeaderFrame tooltip
	ui.ConfigurationTargetList->clear();
	CharString toolTip("Targets: ");
	ListIterator<Configuration> configIterator(dissolveWindow_->dissolve().configurations());
	while (Configuration* cfg = configIterator.iterate())
	{
		QListWidgetItem* item = new QListWidgetItem(cfg->name(), ui.ConfigurationTargetList);
		item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
		item->setData(Qt::UserRole, VariantPointer<Configuration>(cfg));

		if (module_->isTargetConfiguration(cfg))
		{
			item->setCheckState(Qt::Checked);

			if (configIterator.isFirst()) toolTip.strcatf("%s", cfg->name());
			else toolTip.strcatf(", %s", cfg->name());
		}
		else item->setCheckState(Qt::Unchecked);
	}
	ui.ConfigurationTargetGroup->setVisible(!module_->configurationLocal());
	ui.HeaderFrame->setToolTip(toolTip.get());

	// Update keywords
	ui.KeywordsWidget->updateControls();

	refreshing_ = false;
}

// Return suitable QPixmap for supplied Module
QPixmap ProcedureChartNodeBlock::modulePixmap(const Module* module)
{
	if (module) return modulePixmap(module->type());

	return QPixmap(":/modules/icons/modules_generic.svg");
}

// Return suitable QPixmap for supplied Module type
QPixmap ProcedureChartNodeBlock::modulePixmap(QString moduleType)
{
	// Construct the name of the icon for this module in our resource file
	QString iconName = QString(":/modules/icons/modules_%1.svg").arg(moduleType.toLower());
	if (QFile::exists(iconName)) return QPixmap(iconName);

	return QPixmap(":/modules/icons/modules_generic.svg");
}

// Disable sensitive controls, ready for main code to run
void ProcedureChartNodeBlock::disableSensitiveControls()
{
	ui.KeywordsControlWidget->setEnabled(false);
	ui.RunButton->setEnabled(false);
	ui.EnabledButton->setEnabled(false);
	ui.FrequencySpin->setEnabled(false);
	ui.RemoveButton->setEnabled(false);
}

// Enable sensitive controls, ready for main code to run
void ProcedureChartNodeBlock::enableSensitiveControls()
{
	ui.KeywordsControlWidget->setEnabled(true);
	ui.RunButton->setEnabled(true);
	ui.EnabledButton->setEnabled(true);
	ui.FrequencySpin->setEnabled(true);
	ui.RemoveButton->setEnabled(true);
}

void ProcedureChartNodeBlock::on_ToggleSettingsButton_clicked(bool checked)
{
	ui.KeywordsControlWidget->setVisible(checked);

	adjustSize();
	updateGeometry();

	emit(settingsToggled());
}

void ProcedureChartNodeBlock::on_RemoveButton_clicked(bool checked)
{
	emit (remove(module_->uniqueName()));
}

void ProcedureChartNodeBlock::on_RunButton_clicked(bool checked)
{
	if (!module_) return;

	module_->executeProcessing(dissolve_, dissolve_.worldPool());

	updateControls();

	emit run();
}

void ProcedureChartNodeBlock::on_EnabledButton_clicked(bool checked)
{
	if (refreshing_) return;

	if (!module_) return;

	module_->setEnabled(checked);

	dissolveWindow_->setModified();
}

void ProcedureChartNodeBlock::on_FrequencySpin_valueChanged(int value)
{
	if (refreshing_) return;

	module_->setFrequency(value);

	ui.FrequencyLabel->setText(QString("(%1)").arg(module_->frequencyDetails(dissolve_.iteration())));
}

void ProcedureChartNodeBlock::on_ConfigurationTargetList_itemChanged(QListWidgetItem* item)
{
	if (refreshing_) return;

	// Get Configuration for item
	Configuration* cfg = VariantPointer<Configuration>(item->data(Qt::UserRole));

	// If the item is unchecked, make sure it is not present in the Module's Configuration targets
	if (item->checkState() == Qt::Checked) module_->removeTargetConfiguration(cfg);
	else module_->addTargetConfiguration(cfg);
}

/*
 * Geometry
 */

// Return width of underlying widget
int ProcedureChartNodeBlock::widgetWidth() const
{
	return minimumSize().width();
}

// Return height of underlying widget
int ProcedureChartNodeBlock::widgetHeight() const
{
	return minimumSize().height();
}

// Set underlying widget geometry
void ProcedureChartNodeBlock::setWidgetGeometry(int left, int top, int width, int height)
{
	setGeometry(left, top, width, height);
}

// Return whether the supplied point (in local widget coordinates) allows a drag operation to begin
bool ProcedureChartNodeBlock::isDragPoint(QPoint point) const
{
	// -- Use something like: if (geometry_.contains(widget()->mapFromGlobal(globalPos))) ...
	return true;
}
