/*
	*** Procedure Chart
	*** src/gui/charts/procedure_funcs.cpp
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

#include "gui/charts/procedure.hui"
#include "gui/charts/proceduremetrics.h"
#include "gui/charts/procedurenode.h"
#include "gui/widgets/mimestrings.h"
#include "procedure/procedure.h"
#include "procedure/nodes/sequence.h"
#include <QApplication>
#include <QDrag>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QWidget>

// Constructor
ProcedureChart::ProcedureChart(QWidget* parent) : QWidget(parent)
{
	refreshing_ = false;

	resizeToWidgets_ = true;

	// Target Procedure
	procedure_ = NULL;

	// Drag / Drop
	setAcceptDrops(true);
	draggedBlock_ = NULL;
	externalObjectDragged_ = false;
// 	insertionWidget_ = NULL;
	currentHotSpotIndex_ = -1;

	// Create the insertion widget if we don't already have one
// 	insertionWidget_ = new ProcedureChartInsertionBlock(this, dissolveWindow_);
// 	insertionWidget_->setVisible(false);

	updateControls();
}

ProcedureChart::~ProcedureChart()
{
}

/*
 * Chart Display
 */

// Return if a ChartBlock can be dragged from the current position
ChartBlock* ProcedureChart::dragBlockAt(QPoint globalPos)
{
	// Check through all the defined ChartBlocks and see if we grab one
	RefListIterator<ChartBlock> chartBlockIterator(chartBlocks_);
	while (ChartBlock* block = chartBlockIterator.iterate()) if (block->isDragPoint(globalPos)) return block;

	return NULL;
}

/*
 * Update
 */

// Update controls within widget
void ProcedureChart::updateControls()
{
	// Ensure that blocks for display are up-to-date
	updateContentBlocks();

	// Need to recreate the display widgets list now
// 	recreateDisplayWidgets();

	// Update the layout
	layOutWidgets();
}

// Disable sensitive controls within widget, ready for main code to run
void ProcedureChart::disableSensitiveControls()
{
	RefListIterator<ChartBlock> chartBlockIterator(chartBlocks_);
	while (ChartBlock* block = chartBlockIterator.iterate()) block->disableSensitiveControls();
}

// Enable sensitive controls within widget, ready for main code to run
void ProcedureChart::enableSensitiveControls()
{
	RefListIterator<ChartBlock> chartBlockIterator(chartBlocks_);
	while (ChartBlock* block = chartBlockIterator.iterate()) block->enableSensitiveControls();
}

















/*
 * QWidget Reimplementations
 */

// Paint event
void ProcedureChart::paintEvent(QPaintEvent* event)
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

// 	int col = 0, row = 0;
// 	QPoint p1, p2;
// 	RefDataListIterator<ProcedureChartBlock,ChartBlock*> blockIterator(displayBlocks_);
// 	while (ProcedureChartBlock* block = blockIterator.iterate())
// 	{
// 		// If this is the last block then there is nothing more to do
// 		if (blockIterator.isLast()) break;
// 
// 		// Peek the next block
// 		ProcedureChartBlock* nextBlock = blockIterator.peek();
// 
// 		/*
// 		 * Draw a connecting line from the right-hand side of this block, to the left-hand side of the next one.
// 		 * We will always draw the line at a y-coordinate 16 pixels below the tops of the widgets, for consistency.
// 		 */
// 
// 		// If the two blocks are on the same row, just draw a simple solid horizontal line. If not, draw a dashed line in parts.
// 		if (col < (nColumns_-1))
// 		{
// 			painter.setPen(solidPen);
// 			p1 = QPoint(lefts_[col]+widths_[col], tops_[row]+metrics.blockBorderWidth()/2 + metrics.blockDentOffset() + metrics.blockDentRadius());
// 			p2 = QPoint(lefts_[col+1]+metrics.blockBorderWidth()/2, p1.y());
// 			painter.drawLine(p1, p2);
// 			painter.setBrush(Qt::black);
// 			painter.drawEllipse(p2, metrics.blockDentRadius()-metrics.blockBorderWidth()-1, metrics.blockDentRadius()-metrics.blockBorderWidth()-1);
// 		}
// 		else
// 		{
// 			// Work out the mid-line y-coordinate to follow, between the two rows
// 			int yMid = tops_[row+1] - metrics.chartRowSpacing()/2;
// 			painter.setPen(dottedPen);
// 
// 			// Move out into the right-hand margin, making sure we are outside the widest widget in the column
// 			p1 = QPoint(lefts_[col]+widths_[col], tops_[row]+metrics.blockBorderWidth()/2 + metrics.blockDentOffset() + metrics.blockDentRadius());
// 			p2 = p1 + QPoint(widths_[col] - block->widgetWidth() + metrics.chartMinimumColumnSpacing()/2, 0);
// 			painter.drawLine(p1, p2);
// 
// 			// Move down to the midpoint between rows
// 			p1 = QPoint(p2.x(), yMid);
// 			painter.drawLine(p2, p1);
// 
// 			// Move back across to the left-hand-side margin
// 			p2 = QPoint(metrics.chartMinimumColumnSpacing()/2, p1.y());
// 			painter.drawLine(p1, p2);
// 
// 			// Drop down to the next widget's level
// 			QPoint p3 = QPoint(lefts_[0]+metrics.blockBorderWidth()/2, tops_[row+1]+metrics.blockBorderWidth()/2 + metrics.blockDentOffset() + metrics.blockDentRadius());
// 			p1 = QPoint(p2.x(), p3.y());
// 			painter.drawLine(p2, p1);
// 
// 			// And finally on to the widget's left-hand-side
// 			painter.drawLine(p1, p3);
// 
// 			// Finish by drawing the dot joiner
// 			painter.setBrush(Qt::gray);
// 			painter.drawEllipse(p3, metrics.blockDentRadius()-metrics.blockBorderWidth()-1, metrics.blockDentRadius()-metrics.blockBorderWidth()-1);
// 		}
// 
// 		++col;
// 		if (col == nColumns_)
// 		{
// 			col = 0;
// 			++row;
// 		}
// 
// 		// Set the next block pointer
// 		block = nextBlock;
// 	}

	// TEST - Highlight all hotspots
	if (false)
	{
		ListIterator<ChartHotSpot> hotSpotIterator(hotSpots_);
		while (ChartHotSpot* hotSpot = hotSpotIterator.iterate()) painter.fillRect(hotSpot->geometry(), QBrush(QColor(200,200,0,50)));
	}
}

// Geometry changed
void ProcedureChart::resizeEvent(QResizeEvent* event)
{
	layOutWidgets();

	repaint();
}

// Size hint
QSize ProcedureChart::sizeHint() const
{
	return sizeHint_;
}

// Minimum size hint
QSize ProcedureChart::minimumSizeHint() const
{
	return minimumSizeHint_;
}

// Mouse press event
void ProcedureChart::mousePressEvent(QMouseEvent* event)
{
	mouseDownPosition_ = event->pos();
}

// Mouse move event
void ProcedureChart::mouseMoveEvent(QMouseEvent* event)
{
	// If the left button is not down, nothing to do here
	if (!(event->buttons() & Qt::LeftButton)) return;

	// Check to see if we should begin a drag event based on the length of the click-drag so far
	if ((event->pos() - mouseDownPosition_).manhattanLength() < QApplication::startDragDistance()) return;

	// If a ChartBlock header was clicked on at the original position, begin the drag
	draggedBlock_ = dragBlockAt(mapToGlobal(mouseDownPosition_));
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
	drag->setPixmap(draggedBlock_->widget()->grab());

	// Recreate the displayed widgets (so our dragged block is excluded)
// 	recreateDisplayWidgets();

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
// 	recreateDisplayWidgets();

	// Recalculate layout
	layOutWidgets(true);
}

// Mouse release event
void ProcedureChart::mouseReleaseEvent(QMouseEvent* event)
{
}

// Mouse double-click event
void ProcedureChart::mouseDoubleClickEvent(QMouseEvent* event)
{
	// If the left button is not down, nothing to do here
	if (!(event->buttons() & Qt::LeftButton)) return;

	// Was a ChartBlock's header was under the mouse?
	ChartBlock* moduleBlock = dragBlockAt(mapToGlobal(event->pos()));
	if (!moduleBlock) return;

	// Attempt to open the Module in a ModuleTab
// 	dissolveWindow_->addModuleTab(moduleBlock->module());
}

// Drag enter event
void ProcedureChart::dragEnterEvent(QDragEnterEvent* event)
{
	// Is the correct data type being dragged over us?
	if (event->mimeData()->hasFormat("image/x-dissolve-moduleblock")) event->accept();
	else if (event->mimeData()->hasFormat("image/x-dissolve-paletteblock")) event->accept();
	else if (event->mimeData()->hasFormat("dissolve/mimestrings"))
	{
		// Cast into a MimeStrings object
		const MimeStrings* mimeStrings = dynamic_cast<const MimeStrings*>(event->mimeData());
		if (!mimeStrings) return;

		// Check if the relevant datum is present
		if (mimeStrings->hasData(MimeString::ModuleType))
		{
			externalObjectDragged_ = true;
			externalDragObjectData_ = mimeStrings->data(MimeString::ModuleType);
// 			recreateDisplayWidgets();
			event->accept();
		}
		else event->ignore();
	}
}

// Drag leave event
void ProcedureChart::dragLeaveEvent(QDragLeaveEvent* event)
{
	// Object has been dragged outside the widget
	resetAfterDrop();

	update();

	event->accept();
}

// Draw move event
void ProcedureChart::dragMoveEvent(QDragMoveEvent* event)
{
	// Check to see if the mouse is currently over any defined hot spot
	ChartHotSpot* spotUnderMouse = hotSpotAt(event->pos());
	if (!spotUnderMouse)
	{
		// If there was a previous hot-spot, recreate the display widgets and lay out again
		bool previousHotSpot = currentHotSpotIndex_ != -1;

		currentHotSpotIndex_ = -1;

		if (previousHotSpot)
		{
// 			recreateDisplayWidgets();

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
	if (spotUnderMouse->type() == ChartHotSpot::InsertionHotSpot)
	{
// 		recreateDisplayWidgets();

		layOutWidgets(true);
	}

	repaint();
}


/*
 * Drag / Drop
 */

// Return drop hotspot, if any, under specified point
ChartHotSpot* ProcedureChart::hotSpotAt(QPoint pos)
{
	for (ChartHotSpot* hotSpot = hotSpots_.first(); hotSpot != NULL; hotSpot = hotSpot->next) if (hotSpot->contains(pos)) return hotSpot;

	return NULL;
}

// Reset after drop
void ProcedureChart::resetAfterDrop(bool animate)
{
	currentHotSpotIndex_ = -1;
	draggedBlock_ = NULL;
	externalObjectDragged_ = false;
	externalDragObjectData_.clear();

// 	recreateDisplayWidgets();

	layOutWidgets(animate);

	repaint();
}

/*
 * Display Widgets
 */

// Recreate list of widgets for display
// void ProcedureChart::recreateDisplayWidgets()
// {
// 	// Clear the current RefList
// 	displayBlocks_.clear();
// 
// 	/*
// 	 * Loop over current module widgets and add them to the list.
// 	 * If there is a current draggedBlock_, exclude this from the display list.
// 	 */
// 
// 	// Get the current hotSpot, if there is one - we will add a placeholder widget for it
// 	ChartHotSpot* hotSpot = currentHotSpotIndex_ != -1 ? hotSpots_[currentHotSpotIndex_] : NULL;
// 	insertionWidget_->setVisible(hotSpot);
// 
// 	RefListIterator<ChartBlock> moduleChartModuleBlockIterator(moduleWidgets_);
// 	while (ChartBlock* block = moduleChartModuleBlockIterator.iterate())
// 	{
// 		// If the block is excluded, make sure it is hidden
// 		if (block == draggedBlock_) block->setVisible(false);
// 		else
// 		{
// 			// If there is a valid hotspot, check if this is the block before which it should appear.
// 			if (hotSpot && (hotSpot->moduleBlockAfter() == block))
// 			{
// 				displayBlocks_.append(insertionWidget_, NULL);
// 			}
// 
// 			// Make the block visible, and add the original pointer to the block as the RefListItem data
// 			block->setVisible(true);
// 			displayBlocks_.append(block, block);
// 		}
// 	}
// 
// 	// Hotspot might be after the last displayed widget, so check here...
// 	if (hotSpot && (hotSpot->moduleBlockAfter() == NULL)) displayBlocks_.append(insertionWidget_, NULL);
// 
// 	// Set the icons in the insertionWidget_ to reflect the drop action
// 	if (hotSpot)
// 	{
// 		if (externalObjectDragged_) insertionWidget_->setDisplayModuleInsertion(externalDragObjectData_);
// 		else insertionWidget_->setDisplayModuleMove();
// 	}
// }



// Recalculate layout (after widget size change etc.)
void ProcedureChart::recalculateLayout()
{
	layOutWidgets(false);
}

/*
 * State I/O
 */

// Write widget state through specified LineParser
bool ProcedureChart::writeState(LineParser& parser)
{
	return true;
}

// Read widget state through specified LineParser
bool ProcedureChart::readState(LineParser& parser)
{
	return true;
}

/*
 * Specific SHIT
 */

/*
 * Target Procedure
 */

// Target Procedure for display
void ProcedureChart::setProcedure(Procedure* procedure)
{
	procedure_ = procedure;

	updateContentBlocks();

	recalculateLayout();
}

/*
 * Chart Blocks
 */

// Update the content block widgets against the current target data for the supplied SequenceNode
void ProcedureChart::updateContentBlocks(const SequenceProcedureNode* sequence, RefList<ProcedureChartNodeBlock>& oldSequenceWidgets)
{
	// Create a temporary list that will store our widgets to be 'reused'
	RefList<ProcedureChartNodeBlock> newSequenceWidgets;

	// Iterate through the nodes in this sequence, searching for their widgets in the oldWidgetsList
	ListIterator<ProcedureNode> nodeIterator(sequence->sequence());
	while (ProcedureNode* node = nodeIterator.iterate())
	{
		// Does this node have an existing widget?
		ProcedureChartNodeBlock* block = nodeBlock(node);
		if (block)
		{
			// Widget already exists, so remove the reference from nodeWidgets_ and add it to the new list
			newSequenceWidgets.append(block);
			oldSequenceWidgets.remove(block);
			Messenger::printVerbose("Using existing ProcedureChartNodeBlock %p for node %p (%s).\n", block, node, node->name());
		}
		else
		{
			// No current widget, so must create one
			block = new ProcedureChartNodeBlock(this, node);
// 			connect(mcmBlock, SIGNAL(settingsToggled()), this, SLOT(recalculateLayout()));
// 			connect(mcmBlock, SIGNAL(remove(QString)), this, SLOT(removeModule(QString)));
			newSequenceWidgets.append(block);
			chartBlocks_.append(block);
			Messenger::printVerbose("Creating new ProcedureChartNodeBlock %p for node %p (%s).\n", block, node, node->name());
		}

		// If the node has a branch, deal with it here
		if (node->hasBranch()) updateContentBlocks(node->branch(), block->branchWidgets());
	}

	// Any widgets remaining in oldSequenceWidgets are no longer used, and can thus be deleted
	RefListIterator<ProcedureChartNodeBlock> widgetRemover(oldSequenceWidgets);
	while (ProcedureChartNodeBlock* block = widgetRemover.iterate())
	{
		chartBlocks_.remove(block);
		delete block;
	}

	// Copy the new list
	oldSequenceWidgets = newSequenceWidgets;
}

// Update the content block widgets against the current target data
void ProcedureChart::updateContentBlocks()
{
	if (!procedure_) return;

	// Start with the root sequence node of the Procedure - we deal recursively with the rest
	updateContentBlocks(&procedure_->rootSequence(), rootSequenceNodeWidgets_);
}

// Find ProcedureChartNodeBlock displaying specified ProcedureNode anywhere in the heirarchy of nodes
ProcedureChartNodeBlock* ProcedureChart::nodeBlock(ProcedureNode* node)
{
	return nodeBlock(node, rootSequenceNodeWidgets_);
}

// Find ProcedureChartNodeBlock displaying specified ProcedureNode in the supplied list
ProcedureChartNodeBlock* ProcedureChart::nodeBlock(ProcedureNode* node, const RefList<ProcedureChartNodeBlock>& list)
{
	RefListIterator<ProcedureChartNodeBlock> nodeBlockIterator(list);
	while (ProcedureChartNodeBlock* block = nodeBlockIterator.iterate())
	{
		if (block->node() == node) return block;

		// Search the branch list of this node
		ProcedureChartNodeBlock* branchBlock = nodeBlock(node, block->branchWidgets());
		if (branchBlock) return branchBlock;
	}

	return NULL;
}

/*
 * Drag / Drop
 */

// Drop event
void ProcedureChart::dropEvent(QDropEvent* event)
{
	if (event->mimeData()->hasFormat("image/x-dissolve-moduleblock"))
	{
		// Check for a current hot spot and ensure we have a current draggedBlock_
		ChartHotSpot* hotSpot = (currentHotSpotIndex_ == -1 ? NULL : hotSpots_[currentHotSpotIndex_]);
		if ((!hotSpot) || (!draggedBlock_))
		{
			event->ignore();
			resetAfterDrop();
			return;
		}

		// Cast the dragged block up to a ProcedureChartNodeBlock
		ProcedureChartNodeBlock* block = dynamic_cast<ProcedureChartNodeBlock*>(draggedBlock_);
		if (!block) return;

		// TODO
// 		Module* targetModule = draggedBlock_->module();
// 
// 		// Get the Module before which we are going to move the targetModule
// 		Module* beforeModule = hotSpot->moduleBlockAfter() ? hotSpot->moduleBlockAfter()->module() : NULL;
// 
// 		// Check if the dragged Module is back in its original position (in which case we don't flag a change)
// 		if (targetModule->next != beforeModule)
// 		{
// 			modules_.modules().moveBefore(targetModule, beforeModule);
// 
// 			// Flag that the current data has changed
// // 			dissolveWindow_->setModified();
// 		}

		updateControls();

		// Widgets are almost in the right place, so don't animate anything
		resetAfterDrop(false);
	}
	else if (event->mimeData()->hasFormat("dissolve/mimestrings"))
	{
		// Cast into a MimeStrings object
		const MimeStrings* mimeStrings = dynamic_cast<const MimeStrings*>(event->mimeData());
		if (!mimeStrings) return;
		if (!mimeStrings->hasData(MimeString::ModuleType))
		{
			event->ignore();
			resetAfterDrop();
			return;
		}

		// Check for a current hot spot and ensure we have a current palette Module
		ChartHotSpot* hotSpot = (currentHotSpotIndex_ == -1 ? NULL : hotSpots_[currentHotSpotIndex_]);
		if ((!hotSpot) || (!externalObjectDragged_))
		{
			event->ignore();
			resetAfterDrop();
			return;
		}

		// Create the new node
		// TODO
// 		Module* newModule = dissolveWindow_->dissolve().createModuleInstance(qPrintable(externalDragObjectData_));

// 		// Get the ModuleReference before which we are going to move the targetReference
// 		if (hotSpot->moduleBlockAfter() == NULL)
// 		{
// 			// No next block, so add the new Module to the end of the current list
// 			modules_.own(newModule);
// 		}
// 		else
// 		{
// 			// Insert the new Module before the next block
// 			modules_.own(newModule, hotSpot->moduleBlockAfter()->module());
// 		}

		updateControls();

		// Widgets are almost in the right place, so don't animate anything
		resetAfterDrop(false);

		// Flag that the current data has changed
// 		dissolveWindow_->setModified();
	}
	else
	{
		event->ignore();

		resetAfterDrop();
	}
}

/*
 * Widget Layout
*/

// Calculate geometries for the widgets in the supplied sequence list
void ProcedureChart::calculateGeometries(RefList<ProcedureChartNodeBlock>& nodeWidgets, QSize& requiredSize, int& indentLevel)
{
	// Precalculate some useful metrics
	const int leftIndent = indentLevel * metrics_.indentWidth();

	// Loop over widgets in this sequence
	RefListIterator<ProcedureChartNodeBlock> widgetIterator(nodeWidgets);
	while (ProcedureChartNodeBlock* block = widgetIterator.iterate())
	{
		// Set basic position of the block, accounting for the indent
		block->setNewPosition(leftIndent, requiredSize.height());

		// Try to give our block its preferred (minimum) size
		QSize minSize = block->widget()->minimumSizeHint();
		block->setNewSize(minSize.width(), minSize.height());

		// Update the maximum width if necessary
		int blockWidth = block->widgetWidth() + leftIndent;
		if (blockWidth > requiredSize.width()) requiredSize.setWidth(blockWidth);

		// Increase the required height
		requiredSize.setHeight(requiredSize.height() + block->widgetHeight());

		// If this block has sub-blocks (i.e. the node has a branch), recurse in to it
		if (block->branchWidgets().nItems() > 0)
		{
			// Increase the indent level
			++indentLevel;

			// Lay out the branch widgets
			calculateGeometries(block->branchWidgets(), requiredSize, indentLevel);

			// Decrease the indent level
			--indentLevel;
		}
	}
}

// Calculate new widget geometry according to the layout requirements
QSize ProcedureChart::calculateNewWidgetGeometry(QSize currentSize)
{
	/*
	 * ProcedureNode layout is a single vertical column, with branches incrementally indented
	 */

	// Set initial indent level and widget position
	int indentLevel = 0;
	QSize requiredSize(0, metrics_.blockBorderMidPoint());

	// Begin by calling the layout function for the root sequence - we recurse from there
	calculateGeometries(rootSequenceNodeWidgets_, requiredSize, indentLevel);

	return requiredSize;
}

// Lay out widgets
void ProcedureChart::layOutWidgets(bool animate)
{
	// Determine the new sizes / positions of all widgets
	QSize requiredSize = calculateNewWidgetGeometry(minimumSizeHint_);

	// Alter our minimum size hint if requested
	if (resizeToWidgets_)
	{
		// Finalise minimum size hint - we just need to add on the surrounding margins
		requiredSize += QSize(2*metrics_.chartMargin(), 2*metrics_.chartMargin());
		minimumSizeHint_ = requiredSize;
		sizeHint_ = minimumSizeHint_;

		updateGeometry();
	}

	// Commit new block geometries
	RefListIterator<ChartBlock> chartBlockIterator(chartBlocks_);
	while (ChartBlock* block = chartBlockIterator.iterate())
	{
		block->setNewGeometry(animate);
	}

	repaint();
}
