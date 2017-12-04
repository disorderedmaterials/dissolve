/*
	*** Mime Tree Widget
	*** src/gui/mimetreewidget_funcs.cpp
	Copyright T. Youngs 2012-2017

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/mimetreewidget.hui"
#include "gui/mimetreewidgetitem.h"
#include <QMouseEvent>
#include <QDrag>
#include <QApplication>

// Constructor
MimeTreeWidget::MimeTreeWidget(QWidget* parent) : QTreeWidget(parent)
{
	setAcceptDrops(true);
}

/*
 * Signals / Slots
 */

void MimeTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
	// Is the correct data type being dragged over us? No!
	event->ignore();
}

void MimeTreeWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
	// Object has been dragged outside the organiser
	update();
	event->accept();
}

void MimeTreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
	event->ignore();
}

void MimeTreeWidget::dropEvent(QDropEvent *event)
{
	event->ignore();
}

void MimeTreeWidget::mousePressEvent(QMouseEvent* event)
{
	// If the mouse press wasn't left button, ignore it
	if (event->buttons().testFlag(Qt::LeftButton))
	{
		// Set current position
		dragStartPos_ = event->pos();
	}

	QTreeWidget::mousePressEvent(event);
}

void MimeTreeWidget::mouseMoveEvent(QMouseEvent* event)
{
	// If the mouse press wasn't left button, ignore it
	if (!event->buttons().testFlag(Qt::LeftButton))
	{
		QTreeWidget::mouseMoveEvent(event);
		return;
	}

	// Check move distance and begin drag if necessary
	if ((event->pos() - dragStartPos_).manhattanLength() < QApplication::startDragDistance())
	{
		QTreeWidget::mouseMoveEvent(event);
		return;
	}

	// First, get the QTreeWidgetItem under the mouse pointer
	QTreeWidgetItem* item = itemAt(dragStartPos_);
	if (!item) return;

	// Cast the item into a MimeTreeWidgetItem
	MimeTreeWidgetItem* mimeItem = (MimeTreeWidgetItem*) item;
	if (!item)
	{
		printf("Failed to cast QTreeWidgetItem into MimeTreeWidgetItem.\n");
		return;
	}

	// Construct mime data for drag event - copy MimeStrings object from MimeTreeWidget
	MimeStrings* mimeData = new MimeStrings;
	mimeData->add(mimeItem->mimeStrings());

	QDrag *drag = new QDrag(this);
	drag->setMimeData(mimeData);

	// Begin the drag event
	if (drag->exec(Qt::MoveAction) != Qt::MoveAction)
	{
		// Reset everything, and do not perform a move
// 		interactionMode_ = NoInteraction;
// 		interactionPane_ = NULL;
		update();
	}

	event->accept();
}

void MimeTreeWidget::mouseReleaseEvent(QMouseEvent* event)
{
}

