/*
	*** Module Chart
	*** src/gui/modulechart_funcs.cpp
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

#include "gui/modulechart.hui"
#include "gui/modulechartmetrics.h"
#include "gui/modulechartinsertionblock.h"
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
	columnSpacing_ = 32;
	nColumns_ = 1;
	nRows_ = 0;
	horizontalSpacing_ = 0;
	verticalSpacing_ = 0;

	refreshing_ = false;

	// Drag / Drop
	setAcceptDrops(true);
	draggedBlock_ = NULL;
	insertionWidget_ = NULL;
	currentHotSpotIndex_ = -1;

	// Create the insertion widget if we don't already have one
	insertionWidget_ = new ModuleChartInsertionBlock(this, dissolveWindow_);
	insertionWidget_->setVisible(false);

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
	// Create a metrics object
	ModuleChartMetrics metrics;

	// Draw suitable connecting lines between widgets, illustrating the execution path of the code
	QPainter painter(this);

	// Draw the background before we do anything else
	QBrush backgroundBrush = QBrush(Qt::black, QPixmap(":/images/images/squares.jpg"));
	painter.fillRect(QRect(0, 0, width(), height()), backgroundBrush);

	// Set up some QPens
	QPen solidPen(Qt::black);
	solidPen.setWidth(metrics.blockBorderWidth());
	QPen dottedPen(Qt::gray);
	dottedPen.setWidth(metrics.blockBorderWidth());
	dottedPen.setStyle(Qt::DotLine);
	dottedPen.setCapStyle(Qt::RoundCap);

	int col = 0, row = 0;
	QPoint p1, p2;
	RefListIterator<ModuleChartBlock,ModuleChartModuleBlock*> blockIterator(displayBlocks_);
	while (ModuleChartBlock* block = blockIterator.iterate())
	{
		// If this is the last block then there is nothing more to do
		if (blockIterator.isLast()) break;

		// Peek the next block
		ModuleChartBlock* nextBlock = blockIterator.peek();

		/*
		 * Draw a connecting line from the right-hand side of this block, to the left-hand side of the next one.
		 * We will always draw the line at a y-coordinate 16 pixels below the tops of the widgets, for consistency.
		 */

		// If the two blocks are on the same row, just draw a simple solid horizontal line. If not, draw a dashed line in parts.
		if (col < (nColumns_-1))
		{
			painter.setPen(solidPen);
			p1 = QPoint(lefts_[col]+widths_[col], tops_[row]+metrics.blockBorderWidth()/2 + metrics.blockDentOffset() + metrics.blockDentRadius());
			p2 = QPoint(lefts_[col+1]+metrics.blockBorderWidth()/2, p1.y());
			painter.drawLine(p1, p2);
			painter.setBrush(Qt::black);
			painter.drawEllipse(p2, metrics.blockDentRadius()-metrics.blockBorderWidth()-1, metrics.blockDentRadius()-metrics.blockBorderWidth()-1);
		}
		else
		{
			// Work out the mid-line y-coordinate to follow, between the two rows
			int yMid = tops_[row+1] - metrics.chartRowSpacing()/2;
			painter.setPen(dottedPen);

			// Move out into the right-hand margin, making sure we are outside the widest widget in the column
			p1 = QPoint(lefts_[col]+widths_[col], tops_[row]+metrics.blockBorderWidth()/2 + metrics.blockDentOffset() + metrics.blockDentRadius());
			p2 = p1 + QPoint(widths_[col] - block->widgetWidth() + metrics.chartMinimumColumnSpacing()/2, 0);
			painter.drawLine(p1, p2);

			// Move down to the midpoint between rows
			p1 = QPoint(p2.x(), yMid);
			painter.drawLine(p2, p1);

			// Move back across to the left-hand-side margin
			p2 = QPoint(metrics.chartMinimumColumnSpacing()/2, p1.y());
			painter.drawLine(p1, p2);

			// Drop down to the next widget's level
			QPoint p3 = QPoint(lefts_[0]+metrics.blockBorderWidth()/2, tops_[row+1]+metrics.blockBorderWidth()/2 + metrics.blockDentOffset() + metrics.blockDentRadius());
			p1 = QPoint(p2.x(), p3.y());
			painter.drawLine(p2, p1);

			// And finally on to the widget's left-hand-side
			painter.drawLine(p1, p3);

			// Finish by drawing the dot joiner
			painter.setBrush(Qt::gray);
			painter.drawEllipse(p3, metrics.blockDentRadius()-metrics.blockBorderWidth()-1, metrics.blockDentRadius()-metrics.blockBorderWidth()-1);
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

	// TEST - Highlight all hotspots
	if (false)
	{
		ListIterator<ModuleChartHotSpot> hotSpotIterator(hotSpots_);
		while (ModuleChartHotSpot* hotSpot = hotSpotIterator.iterate()) painter.fillRect(hotSpot->geometry(), QBrush(QColor(200,200,0,50)));
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
	mouseDownPosition_ = event->pos();
}

// Mouse move event
void ModuleChart::mouseMoveEvent(QMouseEvent* event)
{
	// If the left button is not down, nothing to do here
	if (!(event->buttons() & Qt::LeftButton)) return;

	// Check to see if we should begin a drag event based on the length of the click-drag so far
	if ((event->pos() - mouseDownPosition_).manhattanLength() > QApplication::startDragDistance())
	{
		// If a ModuleChartModuleBlock header was clicked on at the original position, begin the drag
		draggedBlock_ = moduleChartModuleBlockHeaderAt(mapToGlobal(mouseDownPosition_));
		if (!draggedBlock_) return;

		// Generate mime data for the event
		QByteArray itemData;
		QDataStream dataStream(&itemData, QIODevice::WriteOnly);
		dataStream << draggedBlock_;
		QMimeData* mimeData = new QMimeData;
		mimeData->setData("image/x-dissolve-moduleblock", itemData);

		// Construct the drag object
		QDrag* drag = new QDrag(this);
		drag->setMimeData(mimeData);
	// 	drag->setHotSpot(dragStartPosition_ -
		drag->setPixmap(draggedBlock_->grab());

		// Recreate the displayed widgets (so our dragged block is excluded)
		recreateDisplayWidgets();

		// Recalculate the layout
		layOutWidgets(true);

		repaint();

		// Begin the drag event
		Qt::DropAction dropAction = drag->exec(Qt::MoveAction);
	// 	if (dropAction 
	// 	...

		// Nullify the dragged block
		draggedBlock_ = NULL;

		// Recreate the displayed widgets
		recreateDisplayWidgets();

		// Recalculate layout
		layOutWidgets(true);
	}
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

	// Was a ModuleChartModuleBlock's header was under the mouse?
	ModuleChartModuleBlock* moduleBlock = moduleChartModuleBlockHeaderAt(mapToGlobal(event->pos()));
	if (!moduleBlock) return;

	// Attempt to open the Module in a ModuleTab
	Module* module = moduleBlock->moduleReference()->module();
	if (!module) return;
	dissolveWindow_->addModuleTab(module);
}

// Drag enter event
void ModuleChart::dragEnterEvent(QDragEnterEvent* event)
{
	// Is the correct data type being dragged over us?
	if (event->mimeData()->hasFormat("image/x-dissolve-moduleblock")) event->accept();
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
		// If there was a previous hot-spot, recreate the display widgets and lay out again
		bool previousHotSpot = currentHotSpotIndex_ != -1;

		currentHotSpotIndex_ = -1;

		if (previousHotSpot)
		{
			recreateDisplayWidgets();

			layOutWidgets(true);
		}

		repaint();

		return;
	}

	/*
	 * Store the index of the hot spot, rather than the pointer to it, since the pointer will become invalid as soon as we
	 * recalculate the layout of the widgets (when the dragged widget is made invisible). The index of the hot spot should always be valid.
	 */
	currentHotSpotIndex_ = hotSpots_.indexOf(spotUnderMouse);

	// There is a hotspot here - do we need to recalculate the layout?
	if (spotUnderMouse->type() == ModuleChartHotSpot::ModuleInsertionHotSpot)
	{
		recreateDisplayWidgets();

		layOutWidgets(true);
	}

	repaint();
}

// Drop event
void ModuleChart::dropEvent(QDropEvent* event)
{
	if (event->mimeData()->hasFormat("image/x-dissolve-moduleblock"))
	{
		// Check for a current hot spot and ensure we have a current draggedBlock_
		ModuleChartHotSpot* hotSpot = (currentHotSpotIndex_ == -1 ? NULL : hotSpots_[currentHotSpotIndex_]);
		if ((!hotSpot) || (!draggedBlock_))
		{
			event->ignore();
			resetAfterDrop();
			return;
		}

		// Get the ModuleReference of the dragged block, and check it has a parent list
		ModuleReference* targetReference = draggedBlock_->moduleReference();
		if (targetReference->parentList())
		{
			// Get the ModuleReference before which we are going to move the targetReference
			if (hotSpot->moduleBlockAfter() == NULL)
			{
				// No next block, so move widget to the end of the list
				targetReference->parentList()->modules().moveBefore(targetReference, NULL);
			}
			else
			{
				ModuleReference* beforeReference = hotSpot->moduleBlockAfter()->moduleReference();
				targetReference->parentList()->modules().moveBefore(targetReference, beforeReference);
			}

			updateControls();

			// Widgets are almost in the right place, so don't animate anything
			resetAfterDrop(false);
		}
		else printf("ModuleReference from dragged block has no parent list, so can't move it.\n");
	}
	else
	{
		event->ignore();

		resetAfterDrop();
	}
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

// Reset after drop
void ModuleChart::resetAfterDrop(bool animateWidgets)
{
	currentHotSpotIndex_ = -1;
	draggedBlock_ = NULL;

	recreateDisplayWidgets();

	layOutWidgets(animateWidgets);

	repaint();
}

/*
 * Display Widgets
 */

// Recreate list of widgets for display
void ModuleChart::recreateDisplayWidgets()
{
	// Clear the current RefList
	displayBlocks_.clear();

	/*
	 * Loop over current module widgets and add them to the list.
	 * If there is a current draggedBlock_, exclude this from the display list.
	 */

	// Get the current hotSpot, if there is one - we will add a placeholder widget for it
	ModuleChartHotSpot* hotSpot = currentHotSpotIndex_ != -1 ? hotSpots_[currentHotSpotIndex_] : NULL;
	insertionWidget_->setVisible(hotSpot);

	RefListIterator<ModuleChartModuleBlock,bool> moduleChartModuleBlockIterator(moduleWidgets_);
	while (ModuleChartModuleBlock* block = moduleChartModuleBlockIterator.iterate())
	{
		// If the block is excluded, make sure it is hidden
		if (block == draggedBlock_) block->setVisible(false);
		else
		{
			// If there is a valid hotspot, check if this is the block before which it should appear.
			if (hotSpot && (hotSpot->moduleBlockAfter() == block))
			{
				displayBlocks_.add(insertionWidget_, NULL);
			}

			// Make the block visible, and add the original pointer to the block as the RefListItem data
			block->setVisible(true);
			displayBlocks_.add(block, block);
		}
	}

	// Hotspot might be after the last displayed widget, so check here...
	if (hotSpot && (hotSpot->moduleBlockAfter() == NULL)) displayBlocks_.add(insertionWidget_, NULL);
}

// Find ModuleChartModuleBlock displaying specified ModuleReference
RefListItem<ModuleChartModuleBlock,bool>* ModuleChart::moduleChartModuleBlockReference(ModuleReference* modRef)
{
	RefListIterator<ModuleChartModuleBlock,bool> moduleChartModuleBlockIterator(moduleWidgets_);
	while (ModuleChartModuleBlock* block = moduleChartModuleBlockIterator.iterate()) if (block->moduleReference() == modRef) return moduleChartModuleBlockIterator.currentItem();

	return NULL;
}

// Return the ModuleChartModuleBlock clicked on its header at the specified position (if any)
ModuleChartModuleBlock* ModuleChart::moduleChartModuleBlockHeaderAt(QPoint globalPos)
{
	RefListIterator<ModuleChartModuleBlock,bool> moduleChartModuleBlockIterator(moduleWidgets_);
	while (ModuleChartModuleBlock* block = moduleChartModuleBlockIterator.iterate()) if (block->ui.HeaderFrame->geometry().contains(block->mapFromGlobal(globalPos))) return block;

	return NULL;
}

// Update controls within widget
void ModuleChart::updateControls()
{
	// Step through the Modules in the list, we'll construct a new RefList of current widgets in the process.
	RefList<ModuleChartModuleBlock,bool> newModuleWidgets;
	for (ModuleReference* modRef = modules_.modules().first(); modRef != NULL; modRef = modRef->next)
	{
		// For this ModuleReference, does a current ModuleChartModuleBlock match?
		RefListItem<ModuleChartModuleBlock,bool>* blockRef = moduleChartModuleBlockReference(modRef);
		if (blockRef)
		{
			// Widget already exists, so remove the reference from the old list and add it to our new one
			RefListItem<ModuleChartModuleBlock,bool>* newItem = newModuleWidgets.add(blockRef->item, blockRef->data);
			moduleWidgets_.remove(blockRef);
			newItem->item->updateControls();
		}
		else
		{
			// No current ModuleChartModuleBlock reference, so must create suitable widget
			ModuleChartModuleBlock* mcmBlock = new ModuleChartModuleBlock(this, dissolveWindow_, modRef);
			connect(mcmBlock, SIGNAL(settingsToggled()), this, SLOT(recalculateLayout()));
			newModuleWidgets.add(mcmBlock);
		}
	}

	// For any items that remain in moduleWidgets_ we must now delete the widgets, as they are no longer used
	RefListIterator<ModuleChartModuleBlock,bool> widgetRemover(moduleWidgets_);
	while (ModuleChartModuleBlock* block = widgetRemover.iterate()) delete block;

	// Copy the new RefList
	moduleWidgets_ = newModuleWidgets;

	// Need to recreate the display widgets list now
	recreateDisplayWidgets();

	// Update the layout
	layOutWidgets();
}

// Disable sensitive controls within widget, ready for main code to run
void ModuleChart::disableSensitiveControls()
{
	// TODO Need to disable drag/drop here too
	RefListIterator<ModuleChartModuleBlock,bool> moduleBlockIterator(moduleWidgets_);
	while (ModuleChartModuleBlock* block = moduleBlockIterator.iterate()) block->disableSensitiveControls();
}

// Enable sensitive controls within widget, ready for main code to run
void ModuleChart::enableSensitiveControls()
{
	RefListIterator<ModuleChartModuleBlock,bool> moduleBlockIterator(moduleWidgets_);
	while (ModuleChartModuleBlock* block = moduleBlockIterator.iterate()) block->enableSensitiveControls();
}

/*
 * Widget Layout
*/

// Lay out widgets snaking horizontally
void ModuleChart::layOutWidgets(bool animateWidgets)
{
	/*
	 * Determine how many columns we can fit across our current geometry, obeying a minimum spacing between widgets.
	 * 
	 * Start by trying to lay out everything on one line. If this doesn't fit, decrease the number of columns and try again.
	 */

	ModuleChartMetrics metrics;

	// Reset column spacing before we begin
	columnSpacing_ = metrics.chartMinimumColumnSpacing();

	const int maxWidth = width() - metrics.chartMargin()*2;
	int colCount, totalColumnWidth;
	const int maxColumns = displayBlocks_.nItems();

	for (nColumns_ = maxColumns; nColumns_ >= 1; --nColumns_)
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
		RefListIterator<ModuleChartBlock,ModuleChartModuleBlock*> blockIterator(displayBlocks_);
		while (ModuleChartBlock* block = blockIterator.iterate())
		{
			// Get the width of this block
			int blockWidth = block->widgetWidth();

			// If this is the first row, add our width to the widths_ array, and increase totalColumnWidth
			if (nRows_ == 1)
			{
				widths_.add(blockWidth);
				totalColumnWidth += blockWidth;
				if (colCount > 0) totalColumnWidth += columnSpacing_;

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
			if (blockIterator.isLast()) break;
			
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
	if (nColumns_ == 0)
	{
		nColumns_ = 1;
		widths_.add(width() - 2*metrics.chartMargin());
	}

	// Determine new spacing between columns
	totalColumnWidth = 0;
	for (int n = 0; n<nColumns_; ++n) totalColumnWidth += widths_[n];
	columnSpacing_ = nColumns_ == 1 ? 0 : (maxWidth - totalColumnWidth) / (nColumns_ - 1);

	// Work out row / column top-lefts, and adjust spacing between widgets to fill entire horizontal space
	tops_.clear();
	lefts_.clear();
	heights_.clear();
	hotSpots_.clear();
	int top = metrics.chartMargin(), rowMaxHeight;
	int blockCount = 0;
	RefListIterator<ModuleChartBlock,ModuleChartModuleBlock*> blockIterator(displayBlocks_);
	for (int row = 0; row < nRows_; ++row)
	{
		// Add the top coordinate of this row to our array
		tops_.add(top);

		int left = metrics.chartMargin();
		rowMaxHeight = 0;
		for (int col = 0; col < nColumns_; ++col)
		{
			// Store this column left-hand position
			if (row == 0) lefts_.add(left);

			// Do we still have a valid display widget to consider?
			if (ModuleChartBlock* block = blockIterator.iterate())
			{
				// Set next block and get its ModuleChartModuleBlock pointer
				ModuleChartBlock* nextBlock = blockIterator.peek();
				ModuleChartModuleBlock* nextModuleBlock = nextBlock ? blockIterator.peekData() : NULL;

				// Get the width and height of this block
				int blockWidth = block->widgetWidth();
				int blockHeight = block->widgetHeight();

				// Check the current height against the required height for this block's widget (if we still have a current one) and update if necessary
				if (blockHeight > rowMaxHeight) rowMaxHeight = blockHeight;

				// Set the widget's geometry based on these coordinates and its SizeHint - we give it all the space it needs
				if (animateWidgets && (block->blockType() != ModuleChartBlock::InsertionBlockType))
				{
					QPropertyAnimation *animation = new QPropertyAnimation(block->widget(), "geometry");
					animation->setDuration(100);
					animation->setEndValue(QRect(left, top, blockWidth, blockHeight));
					animation->start();
				}
				else if (block->blockType() == ModuleChartBlock::InsertionBlockType)
				{
					/*
					 * If the insertion point is not the first block on the row, set its height to be the same as the previous block.
					 * If it is the first block on the line, and there is a valid block after, set its height to be the same as the next block.
					 * Otherwise, set to its minimum height.
					 */
					if (col > 0) block->setWidgetGeometry(left, top, widths_[col], blockIterator.peekPrevious()->widgetHeight());
					else if (col < (nColumns_-1)) block->setWidgetGeometry(left, top, widths_[col], blockIterator.peek()->widgetHeight());
					block->setWidgetGeometry(left, top, widths_[col], 64);
					
				}
				else block->setWidgetGeometry(left, top, blockWidth, blockHeight);

				// Store max width in our minimum size hint
				if (rowMaxHeight > minimumSizeHint_.height()) minimumSizeHint_ = QSize(0, rowMaxHeight);

				/*
				 * Work out hot spot for insertion before this block.
				 * If the current block is a ModuleChartInsertionBlock, extend the hotspot to cover the area including this block.
				 */
				ModuleChartHotSpot* hotSpot = hotSpots_.add();
				if (block->blockType() == ModuleChartBlock::InsertionBlockType) hotSpot->set(row, ModuleChartHotSpot::ModuleInsertionHotSpot, QRect(left-columnSpacing_, top, columnSpacing_+widths_[col], rowMaxHeight), nextModuleBlock);
				else hotSpot->set(row, ModuleChartHotSpot::ModuleInsertionHotSpot, QRect(left-columnSpacing_, top, columnSpacing_, rowMaxHeight), blockIterator.currentData());

				// If this is the last block (but not an insertion block) add on a final hotspot
				if (blockIterator.isLast() && (block->blockType() != ModuleChartBlock::InsertionBlockType))
				{
					ModuleChartHotSpot* hotSpot = hotSpots_.add();
					if (col == (nColumns_-1)) hotSpot->set(row, ModuleChartHotSpot::ModuleInsertionHotSpot, QRect(left+widths_[col], top, columnSpacing_, blockHeight), NULL);
					else hotSpot->set(row, ModuleChartHotSpot::ModuleInsertionHotSpot, QRect(left+widths_[col], top, columnSpacing_+widths_[col+1], blockHeight), NULL);
				}
			}

			// Increase left-hand coordinate
			left += widths_[col] + columnSpacing_;
		}

		// Store the row height
		heights_.add(rowMaxHeight);

		// Increase top-side coordinate
		top += rowMaxHeight + metrics.chartRowSpacing();
	}

	// Loop over defined hotspots and set the correct heights based on their row indices
	for (ModuleChartHotSpot* hotSpot = hotSpots_.first(); hotSpot != NULL; hotSpot = hotSpot->next) hotSpot->setAreaHeight(heights_[hotSpot->row()]);

	// Calculate size hint
	// Our requested width is the left-most edge of the left-most column, plus the width of the column, plus the spacing.
	// Our requested height is the top-most edge of the last row, plus the height of the row, plus the spacing.
	if (displayBlocks_.nItems() == 0) sizeHint_ = QSize(0,0);
	else sizeHint_ = QSize(metrics.chartMargin() + lefts_.last() + widths_.last(), metrics.chartMargin() + tops_.last() + heights_.last());

	// Finalise minimum size hint - we just need to add on the surrounding margins
	minimumSizeHint_ += QSize(2*metrics.chartMargin(), 2*metrics.chartMargin());

	updateGeometry();

	repaint();
}

// Recalculate layout (after widget size change etc.)
void ModuleChart::recalculateLayout()
{
	layOutWidgets(false);
}

/*
 * State I/O
 */

// Write widget state through specified LineParser
bool ModuleChart::writeState(LineParser& parser)
{
	return true;
}

// Read widget state through specified LineParser
bool ModuleChart::readState(LineParser& parser)
{
	return true;
}
