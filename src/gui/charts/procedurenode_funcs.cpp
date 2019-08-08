/*
	*** Procedure Node Block Widget - Functions
	*** src/gui/charts/procedurenode_funcs.cpp
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

#include "gui/charts/procedurenode.h"
#include "gui/charts/proceduremetrics.h"
#include "gui/keywordwidgets.h"
#include "procedure/nodes/node.h"
#include "templates/variantpointer.h"
#include <QFile>
#include <QPainter>

// Constructor
ProcedureChartNodeBlock::ProcedureChartNodeBlock(QWidget* parent, ProcedureNode* node) : QWidget(parent), ChartBlock()
{
	// Set up user interface
	ui.setupUi(this);

	// Set necessary values on the widget itself
	ProcedureChartMetrics metrics;
	setContentsMargins(metrics.blockMargins());

	// Hide the keywords control frame to start with
	ui.KeywordsControlWidget->setVisible(false);

	// Set Module pointers
	node_ = node;

// 	// Set up our keywords widget
	// TODO Have to remove dependency on DissolveWindow from KeywordsWidget
	ui.KeywordsWidget->setUp(dissolveWindow_, module_);

	// Set the icon label
// 	ui.IconLabel->setPixmap(modulePixmap(module_));

	// Update our controls
	updateControls();
}

ProcedureChartNodeBlock::~ProcedureChartNodeBlock()
{
}

/*
 * Node Target
 */

// Return displayed node
ProcedureNode* ProcedureChartNodeBlock::node() const
{
	return node_;
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

// Return RefList of widgets that exist in the branch of our Procedure node
RefList<ProcedureChartNodeBlock>& ProcedureChartNodeBlock::nodeWidgets()
{
	return nodeWidgets_;
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
 * Type (ChartBlock Reimplementations)
 */

// Return type of this block
const char* ProcedureChartNodeBlock::blockType()
{
	return "Node";
}

/*
 * Widget (ChartBlock Reimplementations)
 */

// Return underlying widget
QWidget* ProcedureChartNodeBlock::widget()
{
	return this;
}

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

// Set underlying widget position
void ProcedureChartNodeBlock::setWidgetPosition(int left, int top)
{
	move(left, top);
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

/*
 * Update (ChartBlock Reimplementations)
 */

// Update controls within widget
void ProcedureChartNodeBlock::updateControls()
{
	if (!node_) return;

	refreshing_ = true;

	// Set information panel contents
	CharString topText("%s (%s)", module_->type(), module_->uniqueName());
	ui.TopLabel->setText(topText.get());
	ui.FrequencyLabel->setText(QString("(%1)").arg(module_->frequencyDetails(dissolve_.iteration())));

	// Set 'enabled' button status
	ui.EnabledButton->setChecked(module_->enabled());

	// Update keywords
	ui.KeywordsWidget->updateControls();

	refreshing_ = false;
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
