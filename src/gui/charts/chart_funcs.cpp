/*
	*** Base Chart
	*** src/gui/charts/chart_funcs.cpp
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

#include "gui/charts/chart.hui"
#include "gui/widgets/mimestrings.h"
#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>

// Constructor
ChartBase::ChartBase(QWidget* parent) : QWidget(parent)
{
	refreshing_ = false;

	// Options
	resizeToWidgets_ = true;

	// Drag / Drop
	setAcceptDrops(true);
	draggedBlock_ = NULL;
	externalObjectDragged_ = false;
	currentHotSpotIndex_ = -1;
}

ChartBase::~ChartBase()
{
}

/*
 * Chart Display
 */

// Return if a ChartBlock can be dragged from the supplied position
ChartBlock* ChartBase::dragBlockAt(QPoint point)
{
	// Check through all the defined ChartBlocks and see if we grab one
	RefListIterator<ChartBlock> chartBlockIterator(chartBlocks_);
	while (ChartBlock* block = chartBlockIterator.iterate()) if (block->isDragPoint(point)) return block;

	return NULL;
}

/*
 * Update
 */

// Update controls within widget
void ChartBase::updateControls()
{
	// Ensure that blocks for display are up-to-date
	updateContentBlocks();

	// Update the layout
	layOutWidgets();
}

// Disable sensitive controls within widget
void ChartBase::disableSensitiveControls()
{
	RefListIterator<ChartBlock> chartBlockIterator(chartBlocks_);
	while (ChartBlock* block = chartBlockIterator.iterate()) block->disableSensitiveControls();
}

// Enable sensitive controls within widget
void ChartBase::enableSensitiveControls()
{
	RefListIterator<ChartBlock> chartBlockIterator(chartBlocks_);
	while (ChartBlock* block = chartBlockIterator.iterate()) block->enableSensitiveControls();
}

/*
 * QWidget Reimplementations
 */

// Geometry changed
void ChartBase::resizeEvent(QResizeEvent* event)
{
	layOutWidgets();

	repaint();
}

// Size hint
QSize ChartBase::sizeHint() const
{
	return sizeHint_;
}

// Minimum size hint
QSize ChartBase::minimumSizeHint() const
{
	return minimumSizeHint_;
}

// Mouse press event
void ChartBase::mousePressEvent(QMouseEvent* event)
{
	mouseDownPosition_ = event->pos();
}

// Mouse move event
void ChartBase::mouseMoveEvent(QMouseEvent* event)
{
	// If the left button is not down, nothing to do here
	if (!(event->buttons() & Qt::LeftButton)) return;

	// Check to see if we should begin a drag event based on the length of the click-drag so far
	if ((event->pos() - mouseDownPosition_).manhattanLength() < QApplication::startDragDistance()) return;

	// If a ChartBlock header was clicked on at the original position, begin the drag
	draggedBlock_ = dragBlockAt(mouseDownPosition_);
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

	// Recalculate the layout
	layOutWidgets(true);

	repaint();

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
void ChartBase::mouseReleaseEvent(QMouseEvent* event)
{
}

// Mouse double-click event
void ChartBase::mouseDoubleClickEvent(QMouseEvent* event)
{
	// If the left button is not down, nothing to do here
	if (!(event->buttons() & Qt::LeftButton)) return;

	// Was a ChartBlock's header was under the mouse?
	ChartBlock* chartBlock = dragBlockAt(event->pos());
	if (!chartBlock) return;

	// Attempt to open the Module in a ModuleTab
// 	dissolveWindow_->addModuleTab(moduleBlock->module());
}

// Drag enter event
void ChartBase::dragEnterEvent(QDragEnterEvent* event)
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
			event->accept();
		}
		else event->ignore();
	}
}

// Drag leave event
void ChartBase::dragLeaveEvent(QDragLeaveEvent* event)
{
	// Object has been dragged outside the widget
	resetAfterDrop();

	update();

	event->accept();
}

// Draw move event
void ChartBase::dragMoveEvent(QDragMoveEvent* event)
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
		layOutWidgets(true);
	}

	repaint();
}

// Drop event
void ChartBase::dropEvent(QDropEvent* event)
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

// 		// Cast the dragged block up to a ProcedureChartNodeBlock
// 		ProcedureChartNodeBlock* block = dynamic_cast<ProcedureChartNodeBlock*>(draggedBlock_);
// 		if (!block) return;

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
 * Drag / Drop
 */

// Return drop hotspot, if any, under specified point
ChartHotSpot* ChartBase::hotSpotAt(QPoint point)
{
	for (ChartHotSpot* hotSpot = hotSpots_.first(); hotSpot != NULL; hotSpot = hotSpot->next()) if (hotSpot->contains(point)) return hotSpot;

	return NULL;
}

// Reset after drop
void ChartBase::resetAfterDrop(bool animate)
{
	currentHotSpotIndex_ = -1;
	draggedBlock_ = NULL;
	externalObjectDragged_ = false;
	externalDragObjectData_.clear();

	layOutWidgets(animate);

	repaint();
}

/*
 * Widget Layout
 */

// Lay out widgets
void ChartBase::layOutWidgets(bool animate)
{
	// Determine the new sizes / positions of all widgets
	QSize requiredSize = calculateNewWidgetGeometry(minimumSizeHint_);

	// Commit new block geometries
	RefListIterator<ChartBlock> chartBlockIterator(chartBlocks_);
	while (ChartBlock* block = chartBlockIterator.iterate())
	{
		block->setNewGeometry(animate);
	}

	// Alter our minimum size hint if requested
	if (resizeToWidgets_)
	{
		minimumSizeHint_ = requiredSize;
		sizeHint_ = minimumSizeHint_;
		updateGeometry();

		emit(requiredSizeChanged());
	}

	repaint();
}

// Recalculate layout (after widget size change etc.)
void ChartBase::recalculateLayout()
{
	layOutWidgets(false);
}

/*
 * Signals / Slots
 */

// Displayed chart data has been modified in some way
void ChartBase::chartDataModified()
{
	emit(dataModified());
}
