/*
	*** ModuleList Chart
	*** src/gui/charts/modulelist.cpp
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

#include "gui/charts/modulelist.h"
#include "gui/charts/modulelistmetrics.h"
#include "gui/charts/moduleblock.h"
#include "gui/widgets/mimestrings.h"
#include "module/list.h"
#include "module/module.h"
#include <QApplication>
#include <QDrag>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QWidget>

// Constructor
ModuleListChart::ModuleListChart(ModuleList* moduleList, Dissolve& dissolve) : ChartBase(), dissolve_(dissolve)
{
	refreshing_ = false;

	resizeToWidgets_ = true;

	// Target ModuleLayer
	moduleList_ = moduleList;

	updateContentBlocks();

	recalculateLayout();

	// Create the insertion widget if we don't already have one
// 	insertionWidget_ = new ModuleLayerChartInsertionBlock(this, dissolveWindow_);
// 	insertionWidget_->setVisible(false);

	updateControls();
}

ModuleListChart::~ModuleListChart()
{
}

/*
 * QWidget Reimplementations
 */

// Paint event
void ModuleListChart::paintEvent(QPaintEvent* event)
{
	// Draw suitable connecting lines between widgets, illustrating the execution path of the code
	QPainter painter(this);

	// Draw the background before we do anything else
	QBrush backgroundBrush = QBrush(Qt::black, QPixmap(":/images/images/squares.jpg"));
	painter.fillRect(QRect(0, 0, width(), height()), backgroundBrush);

	// Set up some QPens
	QPen solidPen(Qt::black);
	solidPen.setWidth(metrics_.blockBorderWidth());
	QPen dottedPen(Qt::gray);
	dottedPen.setWidth(metrics_.blockBorderWidth());
	dottedPen.setStyle(Qt::DotLine);
	dottedPen.setCapStyle(Qt::RoundCap);

	// TEST - Highlight all hotspots
	if (false)
	{
		ListIterator<ChartHotSpot> hotSpotIterator(hotSpots_);
		while (ChartHotSpot* hotSpot = hotSpotIterator.iterate()) painter.fillRect(hotSpot->geometry(), QBrush(QColor(200,200,0,50)));
	}
}

/*
 * Chart Blocks
 */

// Find ModuleBlock displaying specified Module
ModuleBlock* ModuleListChart::moduleBlock(Module* module)
{
	RefListIterator<ModuleBlock> moduleBlockIterator(moduleBlockWidgets_);
	while (ModuleBlock* block = moduleBlockIterator.iterate()) if (block->module() == module) return block;

	return NULL;
}

// Update the content block widgets against the current target data
void ModuleListChart::updateContentBlocks()
{
	if (!moduleList_) return;

	// Create a temporary list that will store our widgets to be 'reused'
	RefList<ModuleBlock> newWidgets;

	// Iterate through the nodes in this sequence, searching for their widgets in the oldWidgetsList
	ListIterator<Module> moduleIterator(moduleList_->modules());
	while (Module* module = moduleIterator.iterate())
	{
		// Does this Module have an existing widget?
		ModuleBlock* block = moduleBlock(module);
		if (block)
		{
			// Widget already exists, so remove the reference from nodeWidgets_ and add it to the new list
			newWidgets.append(block);
			moduleBlockWidgets_.remove(block);
			Messenger::printVerbose("Using existing ModuleBlock %p for Module %p (%s).\n", block, module, module->uniqueName());
		}
		else
		{
			// No current widget, so must create one
			block = new ModuleBlock(this, module, dissolve_);
			connect(block, SIGNAL(dataModified()), this, SLOT(chartDataModified()));
			newWidgets.append(block);
			chartBlocks_.append(block);
			Messenger::printVerbose("Creating new ModuleBlock %p for Module %p (%s).\n", block, module, module->uniqueName());
		}
	}

	// Any widgets remaining in moduleBlockWidgets_ are no longer used, and can thus be deleted
	RefListIterator<ModuleBlock> widgetRemover(moduleBlockWidgets_);
	while (ModuleBlock* block = widgetRemover.iterate())
	{
		chartBlocks_.remove(block);
		delete block;
	}

	// Copy the new list
	moduleBlockWidgets_ = newWidgets;
}

/*
 * Drag / Drop
 */

// TODO

/*
 * Widget Layout
*/

// Calculate new widget geometry according to the layout requirements
QSize ModuleListChart::calculateNewWidgetGeometry(QSize currentSize)
{
	/*
	 * Lay out widgets horizontally
	 */

	// Create a metrics object
	ModuleListChartMetrics metrics;

	// Left edge of next widget, and maximum height
	int left = metrics.chartMargin();
	int maxHeight = 0;

	// Loop over widgets
	RefListIterator<ModuleBlock> blockIterator(moduleBlockWidgets_);
	while (ModuleBlock* block = blockIterator.iterate())
	{
		// Set left edge of this widget
		block->setNewPosition(left, metrics.chartMargin());

		// Try to give our block its preferred (minimum) size
		QSize minSize = block->widget()->minimumSizeHint();
		block->setNewSize(minSize.width(), minSize.height());

		// Add on width of widget to left edge
		left += minSize.width();

		// Check maximal height
		if (minSize.height() > maxHeight) maxHeight = minSize.height();

		// If there is a block following this one, add on space for the connecting line
		if (!blockIterator.isLast()) left += 40;
	}

	// Finalise and return required size
	return QSize(left + metrics.chartMargin(), maxHeight + 2*metrics.chartMargin());
}

/*
 * State I/O
 */

// Write widget state through specified LineParser
bool ModuleListChart::writeState(LineParser& parser)
{
	return true;
}

// Read widget state through specified LineParser
bool ModuleListChart::readState(LineParser& parser)
{
	return true;
}
