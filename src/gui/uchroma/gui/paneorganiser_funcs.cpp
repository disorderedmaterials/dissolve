/*
	*** Pane Organiser
	*** src/gui/uchroma/gui/paneorganiser_funcs.cpp
	Copyright T. Youngs 2013-2017

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/uchroma/gui/paneorganiser.hui"
#include "gui/uchroma/classes/viewlayout.h"
#include <QPainter>
#include <QMouseEvent>
#include <QMimeData>
#include <QDrag>

// Constructor
PaneOrganiser::PaneOrganiser(QWidget* parent) : QWidget(parent)
{
	setAcceptDrops(true);
 
	// Layout
	paneMargin_= 4;
	viewLayout_ = NULL;
	layoutPixelHeight_ = 1;
	layoutPixelWidth_ = 1;
	paneUnderMouse_ = NULL;
	gridReferenceUnderMouse_ = QPoint();
	clickedGridReference_ = QPoint();
	stretchHandle_ = ViewPane::nHandles;
	interactionMode_ = NoInteraction;
	interactionPane_ = NULL;
}

/*
 * Layout
 */

// Calculate pixel size based on current widget geometry
void PaneOrganiser::calculateLayoutPixelSize()
{
	if (viewLayout_ == NULL) return;

	layoutPixelWidth_ = width() / viewLayout_->nColumns();
	layoutPixelHeight_ = height() / viewLayout_->nRows();
}

// Return grid reference containing coordinates supplied
QPoint PaneOrganiser::gridReference(QPoint point, bool nearest)
{
	if ((point.x() < 0) || (point.x() > width())) return QPoint(-1,-1);
	else if ((point.y() < 0) || (point.y() > height())) return QPoint(-1,-1);

	// Divide by pixel width and return
	if (nearest) return QPoint(round(point.x() / double(layoutPixelWidth_)), round((height() - point.y()) / double(layoutPixelHeight_)));
	else return QPoint(point.x() / layoutPixelWidth_, (height() - point.y()) / layoutPixelHeight_);
}

// Set layout to display / organise
void PaneOrganiser::setViewLayout(ViewLayout* layout)
{
	viewLayout_ = layout;

	if (layout->panes()) emit(currentPaneChanged(layout->panes()->leftEdge(), layout->panes()->bottomEdge()));

	calculateLayoutPixelSize();

	update();
}

/*
 * Interaction
 */

// Update values under mouse
void PaneOrganiser::updateUnderMouse(QPoint pos)
{
	// Get grid reference currently under mouse
	gridReferenceUnderMouse_ = gridReference(pos);

	// Get pane under mouse
	ViewPane* hoverPane = viewLayout_->paneAtGrid(gridReferenceUnderMouse_.x(), gridReferenceUnderMouse_.y());

	// If we are over a pane, see if we are over a handle
	ViewPane::PaneHandle handle = ViewPane::nHandles;
	if (hoverPane)
	{
		// Handle size is twice the paneMargin_
		QRect handleRect(0, 0, paneMargin_*2, paneMargin_*2);
		QPoint paneBottomLeft = QPoint(hoverPane->leftEdge()*layoutPixelWidth_, hoverPane->bottomEdge()*layoutPixelHeight_);
		QPoint paneSize = QPoint(hoverPane->width()*layoutPixelWidth_, hoverPane->height()*layoutPixelHeight_);

		// -- Bottom left
		handleRect.moveBottomLeft(QPoint(paneBottomLeft.x(), height()-paneBottomLeft.y()));
		if (handleRect.contains(pos.x(), pos.y())) handle = ViewPane::BottomLeftHandle;
		// -- Bottom middle
		handleRect.moveBottomLeft(QPoint(paneBottomLeft.x()+0.5*paneSize.x()-paneMargin_, height()-paneBottomLeft.y()));
		if (handleRect.contains(pos.x(), pos.y())) handle = ViewPane::BottomMiddleHandle;
		// -- Bottom right
		handleRect.moveBottomRight(QPoint(paneBottomLeft.x()+paneSize.x(), height()-paneBottomLeft.y()));
		if (handleRect.contains(pos.x(), pos.y())) handle = ViewPane::BottomRightHandle;
		// -- Middle left
		handleRect.moveBottomLeft(QPoint(paneBottomLeft.x(), height()-(paneBottomLeft.y()+0.5*paneSize.y()-paneMargin_)));
		if (handleRect.contains(pos.x(), pos.y())) handle = ViewPane::MiddleLeftHandle;
		// -- Middle right
		handleRect.moveBottomRight(QPoint(paneBottomLeft.x()+paneSize.x(), height()-(paneBottomLeft.y()+0.5*paneSize.y()-paneMargin_)));
		if (handleRect.contains(pos.x(), pos.y())) handle = ViewPane::MiddleRightHandle;
		// -- Top left
		handleRect.moveTopLeft(QPoint(paneBottomLeft.x(), height()-(paneBottomLeft.y()+paneSize.y())));
		if (handleRect.contains(pos.x(), pos.y())) handle = ViewPane::TopLeftHandle;
		// -- Top middle
		handleRect.moveTopLeft(QPoint(paneBottomLeft.x()+0.5*paneSize.x()-paneMargin_, height()-(paneBottomLeft.y()+paneSize.y())));
		if (handleRect.contains(pos.x(), pos.y())) handle = ViewPane::TopMiddleHandle;
		// -- Top right
		handleRect.moveTopRight(QPoint(paneBottomLeft.x()+paneSize.x(), height()-(paneBottomLeft.y()+paneSize.y())));
		if (handleRect.contains(pos.x(), pos.y())) handle = ViewPane::TopRightHandle;
	}

	// Check values and update widget if necessary
	bool updateWidget = false;
	if (hoverPane != paneUnderMouse_) updateWidget = true;
	else if ((handle != stretchHandle_) || (interactionMode_ == StretchInteraction)) updateWidget = true;
	paneUnderMouse_ = hoverPane;
	if (interactionMode_ != StretchInteraction) stretchHandle_ = handle;

	if (updateWidget) update();
}

/*
 * Qt
 */

void PaneOrganiser::dragEnterEvent(QDragEnterEvent *event)
{
	// Is the correct data type being dragged over us?
	if (event->mimeData()->hasFormat("image/x-uchroma-pane")) event->accept();
	else event->ignore();
}

void PaneOrganiser::dragLeaveEvent(QDragLeaveEvent *event)
{
	// Object has been dragged outside the organiser
	update();
	event->accept();
}

void PaneOrganiser::dragMoveEvent(QDragMoveEvent *event)
{
	updateUnderMouse(event->pos());

	if (event->mimeData()->hasFormat("image/x-uchroma-pane"))
	{
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
	else event->ignore();

	update();
}

void PaneOrganiser::dropEvent(QDropEvent *event)
{
	if (event->mimeData()->hasFormat("image/x-uchroma-pane"))
	{
		viewLayout_->translatePane(interactionPane_, gridReferenceUnderMouse_.x()-clickedGridReference_.x(), gridReferenceUnderMouse_.y()-clickedGridReference_.y());

		interactionMode_ = NoInteraction;
		interactionPane_ = NULL;
		update();

		event->setDropAction(Qt::MoveAction);
		event->accept();

		// Need to update the main view now
		emit(updateMainDisplay());
	}
	else event->ignore();
}

void PaneOrganiser::mousePressEvent(QMouseEvent* event)
{
	// If the mouse press wasn't left button, ignore it
	if (! event->buttons().testFlag(Qt::LeftButton)) return;

	// First, get the local grid reference under the mouse pointer
	clickedGridReference_ = gridReference(event->pos());
	if (clickedGridReference_.x() == -1) return;

	// Is there a pane currently under the mouse?
	if (!paneUnderMouse_) return;

	// Emit currentPaneChanged()
	emit(currentPaneChanged(clickedGridReference_.x(), clickedGridReference_.y()));

	// Are we over a handle (in which case start a stretch)? If not, start a drag
	if (stretchHandle_ != ViewPane::nHandles)
	{
		// Start stretch
		interactionMode_ = StretchInteraction;
		interactionPane_ = paneUnderMouse_;
	}
	else
	{
		// Start drag
		interactionMode_ = DragInteraction;
		interactionPane_ = paneUnderMouse_;

		// Draw image of the pane
		QPixmap paneImage(interactionPane_->width()*layoutPixelWidth_, interactionPane_->height()*layoutPixelHeight_);
		int paneIndex = viewLayout_->paneIndex(interactionPane_);
		QPainter painter(&paneImage);
		painter.setBrush(QColor(paneIndex&1 ? 255 : 128, paneIndex&2 ? 255 : 128, paneIndex&4 ? 255 : 128, 255));
		painter.setPen(Qt::NoPen);
		QRect paneRect(0, 0, paneImage.width(), paneImage.height());
		painter.drawRect(paneRect);
		painter.setBrush(Qt::NoBrush);
		painter.setPen(Qt::black);
		painter.drawText(paneRect,  Qt::AlignHCenter | Qt::AlignVCenter, interactionPane_->name());
		painter.end();

		// Construct mime data for drag event
		QByteArray itemData;
		QDataStream dataStream(&itemData, QIODevice::WriteOnly);
		dataStream << clickedGridReference_;
		QMimeData *mimeData = new QMimeData;
		mimeData->setData("image/x-uchroma-pane", itemData);

		QDrag *drag = new QDrag(this);
		drag->setMimeData(mimeData);
		drag->setHotSpot(event->pos() - QPoint(interactionPane_->leftEdge()*layoutPixelWidth_, height()-(interactionPane_->bottomEdge()+interactionPane_->height())*layoutPixelHeight_));
		drag->setPixmap(paneImage);

		// Begin the drag event
		if (drag->exec(Qt::MoveAction) != Qt::MoveAction)
		{
			// Reset everything, and do not perform a move
			interactionMode_ = NoInteraction;
			interactionPane_ = NULL;
			update();
		}
	}

	// Update view immediately so the pane being dragged or stretched is redrawn
	update();
}

void PaneOrganiser::mouseMoveEvent(QMouseEvent* event)
{
	updateUnderMouse(event->pos());

	event->accept();
}

void PaneOrganiser::mouseReleaseEvent(QMouseEvent* event)
{
	// If we were stretching a pane, we must update the pane size here...
	if (interactionMode_ == StretchInteraction)
	{
		QPoint delta(gridReferenceUnderMouse_ - clickedGridReference_);
		interactionPane_->moveHandle(stretchHandle_, delta.x(), delta.y());

		emit(updateMainDisplay());
	}

	// End current interaction mode
	interactionMode_ = NoInteraction;
	stretchHandle_ = ViewPane::nHandles;
	interactionPane_ = NULL;

	update();
}

void PaneOrganiser::resizeEvent(QResizeEvent* event)
{
	calculateLayoutPixelSize();
	update();
}

// Paint event
void PaneOrganiser::paintEvent(QPaintEvent *event)
{
	QBrush paneBrush(QColor(234,234,234));
	QBrush whiteBrush(Qt::white);
	QPen pen(Qt::NoPen);

	// Update pixel sizes
	calculateLayoutPixelSize();

	QPainter painter(this);
	painter.setPen(pen);

	// Set some rectangles
	QRect backgroundRect(0, 0, width(), height());
	QRect handleRect(0, 0, paneMargin_*2, paneMargin_*2);
	QRect paneRect(0, 0, layoutPixelWidth_-2*paneMargin_, layoutPixelHeight_-2*paneMargin_);

	// Draw white background
	painter.setBrush(whiteBrush);
	painter.drawRect(backgroundRect);

	// Check for a valid layout - if there isn't one, we're done
	if (!viewLayout_)
	{
		painter.end();
		return;
	}

	// Draw pane grid
	painter.setBrush(paneBrush);
	for (int x = 0; x < viewLayout_->nColumns(); ++x)
	{
		for (int y = 0; y < viewLayout_->nRows(); ++y)
		{
			paneRect.moveTopLeft(QPoint(x*layoutPixelWidth_+paneMargin_, height()-((y+1)*layoutPixelHeight_)+paneMargin_));
			painter.drawRect(paneRect);
		}
	}

	// Draw panes
	int colourCount = 0;
	QColor paneColour;
	QPoint paneBottomLeft, paneSize, paneDelta;
	for (ViewPane* pane = viewLayout_->panes(); pane != NULL; pane = pane->next)
	{
		// Set pane brush here, so we always cycle through the colours
		paneColour.setRgb(colourCount&1 ? 255 : 0, colourCount&2 ? 255 : 0, colourCount&4 ? 255 : 0, 128);
		painter.setPen(Qt::NoPen);

		++colourCount;

		// Calculate some useful points
		paneBottomLeft = QPoint(pane->leftEdge()*layoutPixelWidth_, pane->bottomEdge()*layoutPixelHeight_);
		paneSize = QPoint(pane->width()*layoutPixelWidth_, pane->height()*layoutPixelHeight_);

		// If this is not the interaction pane, just draw it normally
		if (pane != interactionPane_)
		{
			// Construct a rectangle for this pane
			painter.setBrush(paneColour);
			paneRect.setCoords(paneBottomLeft.x(), height()-paneBottomLeft.y(), paneBottomLeft.x()+paneSize.x(), height()-(paneBottomLeft.y()+paneSize.y()));
			painter.drawRect(paneRect);
		}
		else if (interactionMode_ == DragInteraction)
		{
			// Draw a drop indicator highlighting the new position instead of the actual pane
			paneColour.setAlpha(60);
			painter.setBrush(paneColour);
			QPoint newBottomLeft = QPoint(pane->leftEdge(), pane->bottomEdge()) + (gridReferenceUnderMouse_ - clickedGridReference_);
			paneRect.setCoords(newBottomLeft.x()*layoutPixelWidth_, height()-newBottomLeft.y()*layoutPixelHeight_, (newBottomLeft.x()+pane->width())*layoutPixelWidth_-2, height()-(newBottomLeft.y()+pane->height())*layoutPixelHeight_-2);
			painter.drawRect(paneRect);
		}
		else if (interactionMode_ == StretchInteraction)
		{
			// Recalculate pane geometry to account for the stretch (and so the handles will be drawn properly later)
			paneDelta = QPoint(gridReferenceUnderMouse_ - clickedGridReference_);
			Vec4<int> newGeometry = interactionPane_->geometryAfterHandleMove(stretchHandle_, paneDelta.x(), paneDelta.y());
			paneBottomLeft = QPoint(newGeometry.x*layoutPixelWidth_, newGeometry.y*layoutPixelHeight_);
			paneSize = QPoint(newGeometry.z*layoutPixelWidth_, newGeometry.w*layoutPixelHeight_);

			// Draw the resized rectangle
			QBrush stretchBrush(Qt::DiagCrossPattern);
			stretchBrush.setColor(paneColour);
			painter.setBrush(stretchBrush);
			paneRect.setCoords(paneBottomLeft.x(), height()-paneBottomLeft.y(), paneBottomLeft.x()+paneSize.x(), height()-(paneBottomLeft.y()+paneSize.y()));
			painter.drawRect(paneRect);
		}

		// Draw on text
		painter.setPen(Qt::black);
		painter.drawText(paneRect, Qt::AlignHCenter | Qt::AlignVCenter, pane->name());

		// Draw on resizing handles
		if ((interactionMode_ != DragInteraction) && ((pane == paneUnderMouse_) || (pane == interactionPane_)))
		{
			paneColour.setAlpha(255);
			painter.setPen(Qt::NoPen);
			painter.setBrush(paneColour);
			// -- Bottom left
			handleRect.moveBottomLeft(QPoint(paneBottomLeft.x(), height()-paneBottomLeft.y()));
			painter.setBrush(stretchHandle_ == ViewPane::BottomLeftHandle ? whiteBrush : paneColour);
			painter.drawRect(handleRect);
			// -- Bottom middle
			handleRect.moveBottomLeft(QPoint(paneBottomLeft.x()+0.5*paneSize.x()-paneMargin_, height()-paneBottomLeft.y()));
			painter.setBrush(stretchHandle_ == ViewPane::BottomMiddleHandle ? whiteBrush : paneColour);
			painter.drawRect(handleRect);
			// -- Bottom right
			handleRect.moveBottomRight(QPoint(paneBottomLeft.x()+paneSize.x(), height()-paneBottomLeft.y()));
			painter.setBrush(stretchHandle_ == ViewPane::BottomRightHandle ? whiteBrush : paneColour);
			painter.drawRect(handleRect);
			// -- Middle left
			handleRect.moveBottomLeft(QPoint(paneBottomLeft.x(), height()-(paneBottomLeft.y()+0.5*paneSize.y()-paneMargin_)));
			painter.setBrush(stretchHandle_ == ViewPane::MiddleLeftHandle ? whiteBrush : paneColour);
			painter.drawRect(handleRect);
			// -- Middle right
			handleRect.moveBottomRight(QPoint(paneBottomLeft.x()+paneSize.x(), height()-(paneBottomLeft.y()+0.5*paneSize.y()-paneMargin_)));
			painter.setBrush(stretchHandle_ == ViewPane::MiddleRightHandle ? whiteBrush : paneColour);
			painter.drawRect(handleRect);
			// -- Top left
			handleRect.moveTopLeft(QPoint(paneBottomLeft.x(), height()-(paneBottomLeft.y()+paneSize.y())));
			painter.setBrush(stretchHandle_ == ViewPane::TopLeftHandle ? whiteBrush : paneColour);
			painter.drawRect(handleRect);
			// -- Top middle
			handleRect.moveTopLeft(QPoint(paneBottomLeft.x()+0.5*paneSize.x()-paneMargin_, height()-(paneBottomLeft.y()+paneSize.y())));
			painter.setBrush(stretchHandle_ == ViewPane::TopMiddleHandle ? whiteBrush : paneColour);
			painter.drawRect(handleRect);
			// -- Top right
			handleRect.moveTopRight(QPoint(paneBottomLeft.x()+paneSize.x(), height()-(paneBottomLeft.y()+paneSize.y())));
			painter.setBrush(stretchHandle_ == ViewPane::TopRightHandle ? whiteBrush : paneColour);
			painter.drawRect(handleRect);
		}
	}

	painter.end();
}
