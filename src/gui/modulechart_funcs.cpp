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
#include <QPropertyAnimation>

// Constructor
ModuleChart::ModuleChart(DissolveWindow* dissolveWindow, ModuleList& modules, QWidget* parent) : QWidget(parent), modules_(modules)
{
	dissolveWindow_ = dissolveWindow;

	// Layout
	minSpacing_ = 32;
	nColumns_ = 1;
	nRows_ = 0;
	horizontalSpacing_ = 0;
	verticalSpacing_ = 0;

	refreshing_ = false;

	// Drag / Drop
	setAcceptDrops(true);
	draggedBlock_ = NULL;
	currentHotSpotIndex_ = -1;

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

	// Draw the background before we do anything else
	QBrush backgroundBrush = QBrush(Qt::black, QPixmap(":/images/images/squares.jpg"));
	painter.fillRect(QRect(0, 0, width(), height()), backgroundBrush);

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
	FlowBlock* block = blockIterator.iterate();
	while (!blockIterator.last())
	{
		// If this block is the draggedBlock (i.e. it is temporarily hidden), move on to the next one
		if (block == draggedBlock_) block = blockIterator.iterate();

		// Grab the next block
		FlowBlock* nextBlock = blockIterator.iterate();

		// Make sure that the next block is not the draggedBlock_
		if (nextBlock == draggedBlock_) nextBlock = blockIterator.iterate();

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

		// Set the next block pointer
		block = nextBlock;
	}

	// Highlight current HotSpot, if there is one
	painter.setPen(Qt::NoPen);
	if (currentHotSpotIndex_ != -1)
	{
		// Get the hot spot from the list
		ModuleChartHotSpot* hotSpot = hotSpots_[currentHotSpotIndex_];

		// If a Module insertion or addition hotspot, highlight the area in yellow
		if (hotSpot->type() == ModuleChartHotSpot::ModuleInsertionHotSpot) painter.fillRect(hotSpot->geometry(), QBrush(QColor(200,200,0,50)));
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

	// Recalculate the layout
	layOutWidgets(true);

	// Begin the drag event
	Qt::DropAction dropAction = drag->exec(Qt::MoveAction);
// 	if (dropAction 
// 	...

	// Nullify the dragged block
	draggedBlock_ = NULL;

	// Recalculate layout
	layOutWidgets(true);
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
	else if (event->mimeData()->hasFormat("image/x-dissolve-paletteblock")) event->accept();
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
	// Check to see if the mouse is currently over any defined hot spot
	ModuleChartHotSpot* spotUnderMouse = hotSpotAt(event->pos());
	if (!spotUnderMouse)
	{
		currentHotSpotIndex_ = -1;
		return;
	}

	/*
	 * Store the index of the hot spot, rather than the pointer to it, since the pointer will become invalid as soon as we
	 * recalculate the layout of the widgets (when the dragged widget is made invisible). The index of the hot spot should always be valid.
	 */
	currentHotSpotIndex_ = hotSpots_.indexOf(spotUnderMouse);

	// There is a hotspot here - do we need to recalculate the layout?
	if ((spotUnderMouse->type() == ModuleChartHotSpot::ModuleInsertionHotSpot) || (spotUnderMouse->type() == ModuleChartHotSpot::ModuleInsertionHotSpot))
	{
		layOutWidgets(true);
	}

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
 * Drag / Drop
 */

// Return drop hotspot, if any, under specified point
ModuleChartHotSpot* ModuleChart::hotSpotAt(QPoint pos)
{
	for (ModuleChartHotSpot* hotSpot = hotSpots_.first(); hotSpot != NULL; hotSpot = hotSpot->next) if (hotSpot->contains(pos)) return hotSpot;

	return NULL;
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
			connect(flowBlock, SIGNAL(settingsToggled()), this, SLOT(recalculateLayout()));
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
void ModuleChart::layOutWidgets(bool animateWidgets)
{
	/*
	 * Determine how many columns we can fit across our current geometry, obeying a minimum spacing between widgets.
	 * We will work from a temporary list of FlowBlocks/sizes constructed from the current list and any temporary placeholder
	 * block (i.e. from a drag/drop operation)
	 * 
	 * Start by trying to lay out everything on one line. If this doesn't fit, decrease the number of columns and try again.
	 */

	// Construct the temporary list, excluding the dragged block if there is one. Store the minimumSize here too.
	RefList<FlowBlock,QSize> blocks;
	RefListIterator<FlowBlock,bool> displayedBlockIterator(displayedWidgets_);
	while (FlowBlock* block = displayedBlockIterator.iterate())
	{
		// If this FlowBlock is being dragged, skip it and make sure its not visible
		if (block == draggedBlock_) block->setVisible(false);
		else blocks.add(block, block->minimumSize());
	}

	// If there is a current hotSpot, add in a suitable 'fake' widget to the list
	if (currentHotSpotIndex_ != -1)
	{
		// If we have a valid draggedBlock_, add
	}

	const int maxWidth = width() - minSpacing_;
	int colCount;
	int totalColumnWidth, lastNCols = 0, maxColumns = blocks.nItems();

	for (nColumns_ = blocks.nItems(); nColumns_ >= 1; --nColumns_)
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
		for (RefListItem<FlowBlock,QSize>* currentBlock = blocks.first(); currentBlock != NULL; currentBlock = currentBlock->next)
		{
			// Grab the FlowBlock in this displayedWidget - it can be NULL, if its a placeholder for an insertion etc.
			FlowBlock* block = currentBlock->item;

			// Make sure the current block is visible
			if (block) block->setVisible(true);

			// Get the width of this block
			int blockWidth = currentBlock->data.width();

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
			if (!currentBlock->next) break;
			
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
	hotSpots_.clear();
	RefListItem<FlowBlock,QSize>* currentBlock = blocks.first();
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

			// Do we still have a valid display widget to consider?
			if (currentBlock)
			{
				// Get the block represented by the displayWidget
				FlowBlock* block = currentBlock->item;

				// Check the current height against the required height for this block's widget (if we still have a current one) and update if necessary
				if (currentBlock->data.height() > rowMaxHeight) rowMaxHeight = currentBlock->data.height();

				// Set the widget's geometry based on these coordinates and its SizeHint - we give it all the space it needs
				if (animateWidgets)
				{
					QPropertyAnimation *animation = new QPropertyAnimation(block, "geometry");
					animation->setDuration(100);
					animation->setEndValue(QRect(left, top, currentBlock->data.width(), currentBlock->data.height()));
					animation->start();
				}
				else block->setGeometry(left, top, block->minimumSize().width(), block->minimumSize().height());

				// Store max width in our minimum size hint
				if (rowMaxHeight > minimumSizeHint_.height()) minimumSizeHint_ = QSize(0, rowMaxHeight);

				// Work out hot spot for insertion before this block
				ModuleChartHotSpot* hotSpot = hotSpots_.add();
				hotSpot->set(row, ModuleChartHotSpot::ModuleInsertionHotSpot, QRect(left-minSpacing_, top, minSpacing_, 40), block);

				// Move to next displayedWidget
				currentBlock = currentBlock->next;
			}

			// Increase left-hand coordinate
			left += widths_[col] + minSpacing_;
		}

		// Store the row height
		heights_.add(rowMaxHeight);

		// Increase top-side coordinate
		top += rowMaxHeight + minSpacing_;
	}

	// Loop over defined hotspots and set the correct heights based on their row indices
	for (ModuleChartHotSpot* hotSpot = hotSpots_.first(); hotSpot != NULL; hotSpot = hotSpot->next) hotSpot->setAreaHeight(heights_[hotSpot->row()]);

	// Calculate size hint
	// Our requested width is the left-most edge of the left-most column, plus the width of the column, plus the spacing.
	// Our requested height is the top-most edge of the last row, plus the height of the row, plus the spacing.
	sizeHint_ = QSize(lefts_.last() + widths_.last() + minSpacing_, tops_.last() + heights_.last() + minSpacing_);

	// Finalise minimum size hint - we just need to add on the surrounding margins
	minimumSizeHint_ += QSize(2*minSpacing_, 2*minSpacing_);

	updateGeometry();

	repaint();
}

// Recalculate layout (after widget size change etc.)
void ModuleChart::recalculateLayout()
{
	layOutWidgets(false);
}
