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
#include <QPainter>

// Constructor
ProcedureChartNodeBlock::ProcedureChartNodeBlock(QWidget* parent, ProcedureNode* node) : QWidget(parent), ChartBlock()
{
	// Set up user interface
	ui_.setupUi(this);

	refreshing_ = false;

	// Set necessary values on the widget itself
	ProcedureChartMetrics metrics;
	setContentsMargins(metrics.blockMargins());

	// Set the node pointer
	node_ = node;

	// Hide the keywords control frame to start with
// 	ui_.KeywordsControlWidget->setVisible(false);


	// Set up our keywords widget
	// TODO Have to remove dependency on DissolveWindow from KeywordsWidget
// 	ui_.KeywordsWidget->setUp(dissolveWindow_, module_);

	// Set the icon label
// 	ui_.IconLabel->setPixmap(modulePixmap(module_));

	// Update our controls
	updateControls();

	updateGeometry();
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

	ui_.ToggleSettingsButton->setDisabled(permanent);
}

// Hide the remove button (e.g. when shown in a ModuleTab)
void ProcedureChartNodeBlock::hideRemoveButton()
{
	ui_.RemoveButton->setVisible(false);
}

// Return RefList of widgets that exist in the branch of our Procedure node
RefList<ProcedureChartNodeBlock>& ProcedureChartNodeBlock::branchWidgets()
{
	return branchWidgets_;
}

void ProcedureChartNodeBlock::on_ToggleSettingsButton_clicked(bool checked)
{
	ui_.KeywordsControlWidget->setVisible(checked);

	adjustSize();
	updateGeometry();

	emit(settingsToggled());
}

void ProcedureChartNodeBlock::on_RemoveButton_clicked(bool checked)
{
	emit (remove(node_));
}

/*
 * QWidget Reimplementations
 */

// Paint event
void ProcedureChartNodeBlock::paintEvent(QPaintEvent* event)
{
	if (!node_) return;

	ProcedureChartMetrics metrics;

	QPainter painter(this);
	
	QPen borderPen;
	borderPen.setWidth(metrics.blockBorderWidth());
	painter.setPen(borderPen);

	QPainterPath borderPath;
	borderPath.moveTo(metrics.blockBorderMidPoint(), metrics.blockBorderMidPoint());
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
	return sizeHint().width();
}

// Return height of underlying widget
int ProcedureChartNodeBlock::widgetHeight() const
{
	return sizeHint().height();
}

// Return whether the supplied point (on the parent chart) allows a drag operation to begin
bool ProcedureChartNodeBlock::isDragPoint(QPoint point) const
{
	if (geometry().contains(point)) return true;

	return false;
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
	ui_.TopLabel->setText(node_->name());

	// Update keywords
	ui_.KeywordsWidget->updateControls();

	refreshing_ = false;
}

// Disable sensitive controls, ready for main code to run
void ProcedureChartNodeBlock::disableSensitiveControls()
{
	ui_.KeywordsControlWidget->setEnabled(false);
	ui_.RemoveButton->setEnabled(false);
}

// Enable sensitive controls, ready for main code to run
void ProcedureChartNodeBlock::enableSensitiveControls()
{
	ui_.KeywordsControlWidget->setEnabled(true);
	ui_.RemoveButton->setEnabled(true);
}
