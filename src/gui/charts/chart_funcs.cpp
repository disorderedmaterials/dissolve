/*
    *** Base Chart
    *** src/gui/charts/chart_funcs.cpp
    Copyright T. Youngs 2012-2020

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

ChartBase::ChartBase(QWidget *parent) : QWidget(parent)
{
    refreshing_ = false;
    selectedBlock_ = nullptr;

    // Options
    resizeToWidgets_ = true;

    // Drag / Drop
    setAcceptDrops(true);
    draggedBlock_ = nullptr;
    currentHotSpot_ = nullptr;
}

ChartBase::~ChartBase() {}

/*
 * Chart Display
 */

// Return if a ChartBlock can be dragged from the supplied position
ChartBlock *ChartBase::dragBlockAt(QPoint point)
{
    // Check through all the defined ChartBlocks and see if we grab one
    for (ChartBlock *block : chartBlocks_)
        if (block->isDragPoint(point))
            return block;

    return nullptr;
}

/*
 * Update
 */

// Update controls within widget
void ChartBase::updateControls()
{
    // Ensure that blocks for display are up-to-date
    updateContentBlocks();

    for (ChartBlock *block : chartBlocks_)
        block->updateControls();

    // Update the layout
    layOutWidgets();
}

// Disable sensitive controls within widget
void ChartBase::disableSensitiveControls()
{
    for (ChartBlock *block : chartBlocks_)
        block->disableSensitiveControls();
}

// Enable sensitive controls within widget
void ChartBase::enableSensitiveControls()
{
    for (ChartBlock *block : chartBlocks_)
        block->enableSensitiveControls();
}

/*
 * QWidget Reimplementations
 */

// Geometry changed
void ChartBase::resizeEvent(QResizeEvent *event)
{
    layOutWidgets();

    repaint();
}

// Size hint
QSize ChartBase::sizeHint() const { return sizeHint_; }

// Minimum size hint
QSize ChartBase::minimumSizeHint() const { return minimumSizeHint_; }

// Mouse press event
void ChartBase::mousePressEvent(QMouseEvent *event) { mouseDownPosition_ = event->pos(); }

// Mouse move event
void ChartBase::mouseMoveEvent(QMouseEvent *event)
{
    // If the left button is not down, nothing to do here
    if (!(event->buttons() & Qt::LeftButton))
        return;

    // Check to see if we should begin a drag event based on the length of the click-drag so far
    if ((event->pos() - mouseDownPosition_).manhattanLength() < QApplication::startDragDistance())
        return;

    // If a ChartBlock header was clicked on at the original position, begin the drag
    draggedBlock_ = dragBlockAt(mouseDownPosition_);
    if (!draggedBlock_)
        return;

    // Generate mime data for the event
    MimeStrings *mimeData = new MimeStrings(mimeInfo(draggedBlock_));

    // Construct the drag object
    auto *drag = new QDrag(this);
    drag->setMimeData(mimeData);
    drag->setPixmap(draggedBlock_->widget()->grab());

    // Recalculate the layout
    layOutWidgets(true);

    repaint();

    // Begin the drag event
    Qt::DropAction dropAction = drag->exec(Qt::MoveAction);
    // 	if (dropAction
    // 	...

    // Nullify the dragged block
    draggedBlock_ = nullptr;

    // Recalculate layout
    layOutWidgets(true);
}

// Mouse release event
void ChartBase::mouseReleaseEvent(QMouseEvent *event)
{
    ChartBlock *newSelection = dragBlockAt(event->pos());

    if (newSelection != selectedBlock_)
    {
        selectedBlock_ = newSelection;
        blockSelectionChanged(selectedBlock_);
        repaint();
    }
}

// Mouse double-click event
void ChartBase::mouseDoubleClickEvent(QMouseEvent *event)
{
    // If the left button is not down, nothing to do here
    if (!(event->buttons() & Qt::LeftButton))
        return;

    // Was a ChartBlock's header was under the mouse?
    ChartBlock *chartBlock = dragBlockAt(event->pos());
    if (!chartBlock)
        return;

    // Perform the double-click action (if relevant)
    blockDoubleClicked(chartBlock);
}

// Drag enter event
void ChartBase::dragEnterEvent(QDragEnterEvent *event)
{
    // Is the correct data type being dragged over us?
    if (!event->mimeData()->hasFormat("dissolve/mimestrings"))
    {
        event->ignore();
        return;
    }

    // Get the MimeStrings data
    const auto *mimeStrings = dynamic_cast<const MimeStrings *>(event->mimeData());
    if (!mimeStrings)
    {
        event->ignore();
        return;
    }

    // Do we accept the dragged object?
    if (!acceptDraggedObject(mimeStrings))
    {
        event->ignore();
        return;
    }
    else
        event->accept();
}

// Drag leave event
void ChartBase::dragLeaveEvent(QDragLeaveEvent *event)
{
    // Always ignore the drag leave event
    // This event triggers when moving the dragged item over another widget displayed in the same chart.
    event->ignore();
}

// Draw move event
void ChartBase::dragMoveEvent(QDragMoveEvent *event)
{
    // Check the current drag position - if the hotspot has changed, lay out the widgets again
    ChartHotSpot *newHotSpot = hotSpotAt(event->pos());
    if (currentHotSpot_ == newHotSpot)
        return;

    currentHotSpot_ = newHotSpot;

    // Handle any actions arising from hovering over the hotspot
    if (handleHotSpotHover(currentHotSpot_))
        layOutWidgets(true);
}

// Drop event
void ChartBase::dropEvent(QDropEvent *event)
{
    // Is the correct data type being dragged over us?
    if (!event->mimeData()->hasFormat("dissolve/mimestrings"))
    {
        event->ignore();
        return;
    }

    // Get the MimeStrings data
    const auto *mimeStrings = dynamic_cast<const MimeStrings *>(event->mimeData());
    if (!mimeStrings)
    {
        event->ignore();
        return;
    }

    // Handle the dropped object
    handleDroppedObject(mimeStrings);

    // Update controls
    updateControls();

    // Widgets are almost in the right place, so don't animate anything
    currentHotSpot_ = nullptr;
    draggedBlock_ = nullptr;

    // 	layOutWidgets(animate);

    // 	repaint();
}

/*
 * Block interaction
 */

// Return whether to accept the dragged object (described by its mime info)
bool ChartBase::acceptDraggedObject(const MimeStrings *strings) { return false; }

// Handle hover over specified hotspot
bool ChartBase::handleHotSpotHover(const ChartHotSpot *hotSpot) { return false; }

// Handle the drop of an object (described by its mime info)
void ChartBase::handleDroppedObject(const MimeStrings *strings) {}

// Return mime info for specified block (owned by this chart)
MimeStrings ChartBase::mimeInfo(ChartBlock *block) { return MimeStrings(); }

// Return hotspot, if any, under specified point
ChartHotSpot *ChartBase::hotSpotAt(QPoint point)
{
    for (auto *hotSpot = hotSpots_.first(); hotSpot != nullptr; hotSpot = hotSpot->next())
        if (hotSpot->contains(point))
            return hotSpot;

    return nullptr;
}

// Reset after drop
void ChartBase::resetAfterDrop(bool animate)
{
    currentHotSpot_ = nullptr;
    draggedBlock_ = nullptr;

    layOutWidgets(animate);

    repaint();
}

// Specified block has been double clicked
void ChartBase::blockDoubleClicked(ChartBlock *block)
{
    // No default action
}

// Block selection has changed
void ChartBase::blockSelectionChanged(ChartBlock *block)
{
    // No default action
}

// The chart has requested removal of one of its blocks
void ChartBase::blockRemovalRequested(const QString &blockIdentifier)
{
    // No default action
}

// Return selected ChartBlock
ChartBlock *ChartBase::selectedBlock() const { return selectedBlock_; }

/*
 * Widget Layout
 */

// Lay out widgets
void ChartBase::layOutWidgets(bool animate)
{
    // Determine the new sizes / positions of all widgets
    QSize requiredSize = calculateNewWidgetGeometry(minimumSizeHint_);

    // Commit new block geometries
    for (ChartBlock *block : chartBlocks_)
        block->setNewGeometry(animate);

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
void ChartBase::recalculateLayout() { layOutWidgets(false); }

/*
 * Signals / Slots
 */

// Displayed chart data has been modified in some way
void ChartBase::chartDataModified() { emit(dataModified()); }
