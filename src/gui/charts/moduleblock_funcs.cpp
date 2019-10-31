/*
	*** Module Block Widget - Functions
	*** src/gui/charts/moduleblock_funcs.cpp
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

#include "gui/charts/moduleblock.h"
#include "gui/charts/modulelistmetrics.h"
#include "gui/modulewidget.h"
#include "main/dissolve.h"
#include "module/module.h"
#include "templates/variantpointer.h"
#include <QFile>
#include <QPainter>

// Constructor
ModuleBlock::ModuleBlock(QWidget* parent, Module* module, Dissolve& dissolve) : QWidget(parent), ChartBlock(), dissolve_(dissolve)
{
	// Set up user interface
	ui_.setupUi(this);

	refreshing_ = false;

	// Set necessary values on the widget itself
	ModuleListChartMetrics metrics;
	setContentsMargins(metrics.blockMargins());
	dragHandleRect_ = QRect(0, 0, width(), height());

	// Set the Module pointer
	module_ = module;

	// Hide the keywords control frame to start with
	ui_.KeywordsControlWidget->setVisible(false);

	// Set up our keywords widget
	ui_.ModuleKeywordsWidget->setUp(module_->keywords(), dissolve_.constCoreData());
	connect(ui_.ModuleKeywordsWidget, SIGNAL(dataModified()), this, SLOT(keywordDataModified()));
	connect(ui_.ModuleKeywordsWidget, SIGNAL(setUpRequired()), this, SLOT(setUpModule()));

	// Set the icon label
	ui_.IconLabel->setPixmap(modulePixmap(module_));

	// Update our controls
	updateControls();

	updateGeometry();
}

ModuleBlock::~ModuleBlock()
{
}

/*
 * Module Target
 */

// Run the set-up stage of the associated Module
void ModuleBlock::setUpModule()
{
	if (!module_) return;

	// Run the Module's set-up stage
	module_->setUp(dissolve_, dissolve_.worldPool());

	emit(updateModuleWidget(ModuleWidget::ResetGraphDataTargetsFlag));
}

// Return displayed Module
Module* ModuleBlock::module() const
{
	return module_;
}

/*
 * Controls
 */

// Set whether the keywords widget is expanded or not, and whether this is permanent
void ModuleBlock::setKeywordsExpanded(bool expanded, bool permanent)
{
	on_ToggleKeywordsButton_clicked(expanded);

	ui_.ToggleKeywordsButton->setDisabled(permanent);
}

// Hide the remove button (e.g. when shown in a ModuleTab)
void ModuleBlock::hideRemoveButton()
{
	ui_.RemoveButton->setVisible(false);
}

// Hide the toggle keywords button (e.g. when shown in a ModuleTab)
void ModuleBlock::hideKeywordsButton()
{
	ui_.ToggleKeywordsButton->setVisible(false);
}

// Return suitable QPixmap for supplied Module
QPixmap ModuleBlock::modulePixmap(const Module* module)
{
	if (module) return modulePixmap(module->type());

	return QPixmap(":/modules/icons/modules_generic.svg");
}

// Return suitable QPixmap for supplied Module type
QPixmap ModuleBlock::modulePixmap(QString moduleType)
{
	// Construct the name of the icon for this module in our resource file
	QString iconName = QString(":/modules/icons/modules_%1.svg").arg(moduleType.toLower());
	if (QFile::exists(iconName)) return QPixmap(iconName);

	return QPixmap(":/modules/icons/modules_generic.svg");
}

void ModuleBlock::on_ToggleKeywordsButton_clicked(bool checked)
{
	ui_.KeywordsControlWidget->setVisible(checked);

	adjustSize();
	updateGeometry();

	emit(keywordsToggled());
}

void ModuleBlock::on_RemoveButton_clicked(bool checked)
{
	emit (remove(module_));
}

/*
 * QWidget Reimplementations
 */

// Paint event
void ModuleBlock::paintEvent(QPaintEvent* event)
{
	if (!module_) return;

	ModuleListChartMetrics metrics;

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
 * Type (ChartBlock Reimplementations)
 */

// Return type of this block
const char* ModuleBlock::blockType()
{
	return "Module";
}

/*
 * Widget (ChartBlock Reimplementations)
 */

// Return underlying widget
QWidget* ModuleBlock::widget()
{
	return this;
}

// Return width of underlying widget
int ModuleBlock::widgetWidth() const
{
	return sizeHint().width();
}

// Return height of underlying widget
int ModuleBlock::widgetHeight() const
{
	return sizeHint().height();
}

// Return whether the supplied point (on the parent chart) allows a drag operation to begin
bool ModuleBlock::isDragPoint(QPoint point) const
{
	if (dragHandleRect_.translated(geometry().left(), geometry().top()).contains(point)) return true;

	return false;
}

/*
 * Update (ChartBlock Reimplementations)
 */

// Update controls within widget
void ModuleBlock::updateControls()
{
	if (!module_) return;

	refreshing_ = true;

	// Set information panel contents
	CharString topText("%s (%s)", module_->type(), module_->uniqueName());
	ui_.TopLabel->setText(topText.get());
	ui_.FrequencyLabel->setText(QString("(%1)").arg(module_->frequencyDetails(dissolve_.iteration())));

	// Set 'enabled' button status
	ui_.EnabledButton->setChecked(module_->enabled());

	// Set frequency spin
	ui_.FrequencySpin->setValue(module_->frequency());

	// Update Configuration list and HeaderFrame tooltip
	ui_.ConfigurationTargetList->clear();
	CharString toolTip("Targets: ");
	ListIterator<Configuration> configIterator(dissolve_.constConfigurations());
	while (Configuration* cfg = configIterator.iterate())
	{
		QListWidgetItem* item = new QListWidgetItem(cfg->name(), ui_.ConfigurationTargetList);
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
	ui_.ConfigurationTargetGroup->setVisible((!module_->configurationLocal()) && (module_->nTargetableConfigurations() != 0));
	ui_.HeaderFrame->setToolTip(toolTip.get());

	// Update keywords
	ui_.ModuleKeywordsWidget->updateControls();

	refreshing_ = false;
}

// Disable sensitive controls
void ModuleBlock::disableSensitiveControls()
{
	ui_.KeywordsControlWidget->setEnabled(false);
	ui_.RemoveButton->setEnabled(false);
}

// Enable sensitive controls
void ModuleBlock::enableSensitiveControls()
{
	ui_.KeywordsControlWidget->setEnabled(true);
	ui_.RemoveButton->setEnabled(true);
}

/*
 * Signals / Slots
 */

// Keyword data for node has been modified
void ModuleBlock::keywordDataModified()
{
	emit(dataModified());
}
