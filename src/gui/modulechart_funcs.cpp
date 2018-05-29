/*
	*** Module Chart
	*** src/gui/modulechart_funcs.cpp
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

#include "gui/modulechart.hui"
#include "gui/flowblock.h"
#include "gui/gui.h"
#include "module/list.h"
#include "module/module.h"
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>

// Constructor
ModuleChart::ModuleChart(DissolveWindow* dissolveWindow, ModuleList& modules, QWidget* parent) : QWidget(parent), modules_(modules)
{
	dissolveWindow_ = dissolveWindow;

	// Drag / Drop
	draggedBlock_ = NULL;

	// Layout
	minSpacing_ = 32;
	nColumns_ = 1;
	nRows_ = 0;
	horizontalSpacing_ = 0;
	verticalSpacing_ = 0;

	refreshing_ = false;

	// Enable drag/drop
	setAcceptDrops(true);

	updateControls();
}

ModuleChart::~ModuleChart()
{
}

/*
 * QWidget Reimplementations
 */

// Paint event
void ModuleChart::paintEvent(QPaintEvent* event)
{
	// Draw suitable connecting lines between widgets, illustrating the execution path of the code
	QPainter painter(this);

	// Set up some QPens
	QPen solidPen(Qt::black);
	solidPen.setWidth(2.0);
	QPen dottedPen(Qt::gray);
	dottedPen.setWidth(2.0);
	dottedPen.setStyle(Qt::DotLine);
	dottedPen.setCapStyle(Qt::RoundCap);

	// Set up a simple path representing an arrow
	QPainterPath rightArrow;
	rightArrow.moveTo(50,50);
	rightArrow.lineTo(-5, 3);
	rightArrow.lineTo(-5, -3);
	rightArrow.closeSubpath();

	RefListIterator<FlowBlock,bool> blockIterator(displayedWidgets_);
	int col = 0, row = 0;
	QPoint p1, p2;
	while (FlowBlock* block = blockIterator.iterate())
	{
		// If this is the last block, there is nothing more to do
		if (blockIterator.last()) break;

		// Grab the next block
		FlowBlock* nextBlock = blockIterator.peek();

		/*
		 * Draw a connecting line from the right-hand side of this block, to the left-hand side of the next one.
		 * We will always draw the line at a y-coordinate 16 pixels below the tops of the widgets, for consistency.
		 */

		// If the two blocks are on the same row, just draw a simple solid horizontal line. If not, draw a dashed line in parts.
		if (col < (nColumns_-1))
		{
			painter.setPen(solidPen);
			p1 = mapFromGlobal(block->globalRightHandFlowAnchor());
			p2 = mapFromGlobal(nextBlock->globalLeftHandFlowAnchor());
			painter.drawLine(p1, p2);
		}
		else
		{
			// Work out the mid-line y-coordinate to follow, between the two rows
			int yMid = tops_[row+1] - minSpacing_/2;
			painter.setPen(dottedPen);

			// Move out into the right-hand margin, making sure we are outside the widest widget in the column
			p1 = mapFromGlobal(block->globalRightHandFlowAnchor());
			p2 = p1 + QPoint(widths_[col] - block->width() + minSpacing_/2, 0);
			painter.drawLine(p1, p2);

			// Move down to the midpoint between rows
			p1 = QPoint(p2.x(), yMid);
			painter.drawLine(p2, p1);

			// Move back across to the left-hand-side margin
			p2 = QPoint(minSpacing_/2, p1.y());
			painter.drawLine(p1, p2);

			// Drop down to the next widget's level
			QPoint p3 = mapFromGlobal(nextBlock->globalLeftHandFlowAnchor());
			p1 = QPoint(p2.x(), p3.y());
			painter.drawLine(p2, p1);

			// And finally on to the widget's left-hand-side
			painter.drawLine(p1, p3);
		}

		++col;
		if (col == nColumns_)
		{
			col = 0;
			++row;
		}
	}
}

// Geometry changed
void ModuleChart::resizeEvent(QResizeEvent* event)
{
	// Work out the new number of columns / rows in the layout, and adjust widget positions to suit
	layOutWidgets();

	repaint();
}

// Size hint
QSize ModuleChart::sizeHint() const
{
	return sizeHint_;
}

// Minimum size hint
QSize ModuleChart::minimumSizeHint() const
{
	return minimumSizeHint_;
}

// Mouse press event
void ModuleChart::mousePressEvent(QMouseEvent* event)
{
	// If the left-button was pressed *on a FlowBlock HeaderFrame*, store the current click position. Otherwise do nothing
	if (event->button() == Qt::LeftButton)
	{
		// Check object under mouse
		draggedBlock_ = flowBlockHeaderAt(mapToGlobal(event->pos()));
// 		if (draggedBlock_) printf("BLOCK = %s\n", draggedBlock_->moduleReference()->module()->uniqueName());
		dragStartPosition_ = event->pos();
	}
}

// Mouse move event
void ModuleChart::mouseMoveEvent(QMouseEvent* event)
{
	// If the left button is not down, nothing to do here
	if (!(event->buttons() & Qt::LeftButton)) return;

	// If we have no draggedBlock_, then nothing to do
	if (!draggedBlock_) return;

	// Check to see if we should begin a drag event based on the length of the click-drag so far
	if ((event->pos() - dragStartPosition_).manhattanLength() < QApplication::startDragDistance()) return;

	// Generate mime data for the event
	QByteArray itemData;
	QDataStream dataStream(&itemData, QIODevice::WriteOnly);
	dataStream << draggedBlock_;
	QMimeData* mimeData = new QMimeData;
	mimeData->setData("image/x-dissolve-flowblock", itemData);

	// Construct the drag object
	QDrag* drag = new QDrag(this);
	drag->setMimeData(mimeData);
// 	drag->setHotSpot(dragStartPosition_ -
	drag->setPixmap(draggedBlock_->grab());

	// Begin the drag event
	Qt::DropAction dropAction = drag->exec(Qt::MoveAction);
// 	if (dropAction 
// 	...
}

// Mouse release event
void ModuleChart::mouseReleaseEvent(QMouseEvent* event)
{

}

// Mouse double-click event
void ModuleChart::mouseDoubleClickEvent(QMouseEvent* event)
{
	// If the left button is not down, nothing to do here
	if (!(event->buttons() & Qt::LeftButton)) return;

	// Was a FlowBlock's header was under the mouse?
	FlowBlock* block = flowBlockHeaderAt(mapToGlobal(event->pos()));
	if (!block) return;

	// Attempt to open the Module in a ModuleTab
	Module* module = block->moduleReference()->module();
	if (!module) return;
	dissolveWindow_->addModuleTab(module);
}

// Drag enter event
void ModuleChart::dragEnterEvent(QDragEnterEvent* event)
{
	// Is the correct data type being dragged over us?
	if (event->mimeData()->hasFormat("image/x-dissolve-flowblock")) event->accept();
	else event->ignore();
}

// Drag leave event
void ModuleChart::dragLeaveEvent(QDragLeaveEvent* event)
{
	// Object has been dragged outside the widget
	update();
	event->accept();
}

// Draw move event
void ModuleChart::dragMoveEvent(QDragMoveEvent* event)
{
	repaint();
}

// Drop event
void ModuleChart::dropEvent(QDropEvent* event)
{
	if (event->mimeData()->hasFormat("image/x-dissolve-flowblock"))
	{
	}
	else event->ignore();
}

/*
 * Display Widgets
 */

// Find FlowBlock displaying specified ModuleReference
RefListItem<FlowBlock,bool>* ModuleChart::flowBlockReference(ModuleReference* modRef)
{
	RefListIterator<FlowBlock,bool> flowBlockIterator(displayedWidgets_);
	while (FlowBlock* block = flowBlockIterator.iterate()) if (block->moduleReference() == modRef) return flowBlockIterator.currentItem();

	return NULL;
}

// Return the FlowBlock clicked on its header at the specified position (if any)
FlowBlock* ModuleChart::flowBlockHeaderAt(QPoint globalPos)
{
	RefListIterator<FlowBlock,bool> flowBlockIterator(displayedWidgets_);
	while (FlowBlock* block = flowBlockIterator.iterate()) if (block->ui.HeaderFrame->geometry().contains(block->mapFromGlobal(globalPos))) return block;

	return NULL;
}

// Update controls within widget
void ModuleChart::updateControls()
{
	// Step through the Modules in the list, we'll construct a new RefList of displayed widgets in the process.
	RefList<FlowBlock,bool> newDisplayedWidgets;
	for (ModuleReference* modRef = modules_.modules().first(); modRef != NULL; modRef = modRef->next)
	{
		// For this ModuleReference, does a current FlowBlock match?
		RefListItem<FlowBlock,bool>* blockRef = flowBlockReference(modRef);
		if (blockRef)
		{
			// Widget already exists, so remove the reference from the old list and add it to our new one
			newDisplayedWidgets.add(blockRef->item, blockRef->data);
			displayedWidgets_.remove(blockRef);
			blockRef->item->updateControls();
		}
		else
		{
			// No current FlowBlock reference, so must create suitable widget
			FlowBlock* flowBlock = new FlowBlock(this, dissolveWindow_, modRef);
			connect(flowBlock, SIGNAL(settingsToggled()), this, SLOT(layOutWidgets()));
			newDisplayedWidgets.add(flowBlock);
		}
	}

	// Any items remaining in the displayedWidgets_ list must now be deleted? TODO
// 	RefListIterator<FlowBlock,bool> removalIterator(displayedWidgets_);
// 	while (FlowBlock* block = removalIterator.iterate()) 

	// Copy the new RefList
	displayedWidgets_ = newDisplayedWidgets;

	// Update the layout
	layOutWidgets();
}

// Disable sensitive controls within widget, ready for main code to run
void ModuleChart::disableSensitiveControls()
{
	// TODO Need to disable drag/drop here too
	RefListIterator<FlowBlock,bool> flowBlockIterator(displayedWidgets_);
	while (FlowBlock* block = flowBlockIterator.iterate()) block->disableSensitiveControls();
}

// Enable sensitive controls within widget, ready for main code to run
void ModuleChart::enableSensitiveControls()
{
	RefListIterator<FlowBlock,bool> flowBlockIterator(displayedWidgets_);
	while (FlowBlock* block = flowBlockIterator.iterate()) block->enableSensitiveControls();
}

/*
 * Widget Layout
*/

// Lay out widgets snaking horizontally
void ModuleChart::layOutHorizontally()
{
	/*
	 * Determine how many columns we can fit across our current geometry, obeying a minimum spacing between widgets.
	 * Start by trying to lay out everything on one line. If this doesn't fit, decrease the number of columns and try again.
	 */
	const int maxWidth = width() - minSpacing_;
	int colCount;
	int totalColumnWidth, lastNCols = 0, maxColumns = displayedWidgets_.nItems();

	for (nColumns_ = displayedWidgets_.nItems(); nColumns_ >= 1; --nColumns_)
	{
		/*
		 * Loop over FlowBlocks, summing their widths as we go for each row.
		 * Also keep track of total column width over all rows - if we exceed this at any point we must continue the outer loop (reduce nColumns_).
		 */
		nRows_ = 1;
		colCount = 0;
		totalColumnWidth = 0;
		widths_.clear();
		minimumSizeHint_ = QSize(0,0);
		RefListIterator<FlowBlock,bool> blockIterator(displayedWidgets_);
		while (FlowBlock* block = blockIterator.iterate())
		{
			// Get the width of this block
			int blockWidth = block->minimumSize().width();

			// If this is the first row, add our width to the widths_ array, and increase totalColumnWidth
			if (nRows_ == 1)
			{
				widths_.add(blockWidth);
				totalColumnWidth += blockWidth + minSpacing_;

				// Store max width in our minimum size hint
				if (blockWidth > minimumSizeHint_.width()) minimumSizeHint_ = QSize(blockWidth, 0);
			}
			else
			{
				// Not the first row, so check this width against the one stored for the current column.
				if (blockWidth > widths_[colCount])
				{
					// The current widget is wider than any other in this column. Need to adjust the totalColumnWidth taking account of the difference
					totalColumnWidth += (blockWidth - widths_[colCount]);
					widths_[colCount] = blockWidth;
				}
			}

			// Check if we have exceeded the available width. However, don't break if the number of columns is 1, since we still need to assess the full column width
			if ((totalColumnWidth > maxWidth) && (nColumns_ > 1)) break;

			// Added this widget OK - if it was the last one in the list, we have found a suitable number of columns
			if (blockIterator.last()) break;
			
			// Not the end of the list, so increase column count and check against the current nColumns_
			++colCount;
			if (colCount == nColumns_)
			{
				colCount = 0;
				++nRows_;
			}
		}

		// If we get to here and we haven't failed, we must have succeeded!
		if (totalColumnWidth <= maxWidth) break;
	}

	// If we get to this point and there are zero columns, there wasn't enough width to fit a single column of widgets in, but we still need one!
	// nRows should always be correct
	if (nColumns_ == 0) nColumns_ = 1;

	// Work out row / column top-lefts
	tops_.clear();
	lefts_.clear();
	heights_.clear();
	RefListIterator<FlowBlock,bool> blockIterator(displayedWidgets_);
	int top = minSpacing_, rowMaxHeight;
	for (int row = 0; row < nRows_; ++row)
	{
		// Add the top coordinate of this row to our array
		tops_.add(top);

		int left = minSpacing_;
		rowMaxHeight = 0;
		for (int col = 0; col < nColumns_; ++col)
		{
			// Store this column left-hand position
			if (row == 0) lefts_.add(left);

			// Get FlowBlock for this grid reference (i.e. the next one in the list)
			FlowBlock* block = blockIterator.iterate();

			// Do we still have a valid block?
			if (block)
			{
				// Check the current height against the required height for this block's widget (if we still have a current one) and update if necessary
				if (block->minimumSize().height() > rowMaxHeight) rowMaxHeight = block->minimumSize().height();
 
				// Set the widget's geometry based on these coordinates and its SizeHint - we give it all the space it needs
				block->setGeometry(left, top, block->minimumSize().width(), block->minimumSize().height());

				// Store max width in our minimum size hint
				if (rowMaxHeight > minimumSizeHint_.height()) minimumSizeHint_ = QSize(0, rowMaxHeight);
			}

			// Increase left-hand coordinate
			left += widths_[col] + minSpacing_;
		}

		// Store the row height
		heights_.add(rowMaxHeight);

		// Increase top-side coordinate
		top += rowMaxHeight + minSpacing_;
	}

	// Calculate size hint
	// Our requested width is the left-most edge of the left-most column, plus the width of the column, plus the spacing.
	// Our requested height is the top-most edge of the last row, plus the height of the row, plus the spacing.
	sizeHint_ = QSize(lefts_.last() + widths_.last() + minSpacing_, tops_.last() + heights_.last() + minSpacing_);

	// Finalise minimum size hint - we just need to add on the surrounding margins
	minimumSizeHint_ += QSize(2*minSpacing_, 2*minSpacing_);
}

// Lay out widgets snaking vertically
void ModuleChart::layOutVertically()
{
}

// Lay out the current widgets according to our available geometry
void ModuleChart::layOutWidgets()
{
	layOutHorizontally();

	updateGeometry();

	repaint();
}
