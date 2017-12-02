/*
	*** Object Tree
	*** src/gui/objecttree_funcs.cpp
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

#include "gui/objecttree.hui"
#include "gui/objectdata.h"
#include <QMouseEvent>
#include <QDrag>
#include <QApplication>

// Constructor
ObjectTree::ObjectTree(QWidget* parent) : QTreeWidget(parent)
{
	setAcceptDrops(true);
}

/*
 * Object Data
 */

// Set object data
bool ObjectTree::addObjectData(ObjectData* data, QTreeWidgetItem* item, int column)
{
	if (!item) return false;

	QString objectName = item->data(column, Qt::UserRole).toString();
	data->addObject(qPrintable(objectName));
}

/*
 * Signals / Slots
 */

void ObjectTree::dragEnterEvent(QDragEnterEvent *event)
{
	// Is the correct data type being dragged over us? No!
	event->ignore();
}

void ObjectTree::dragLeaveEvent(QDragLeaveEvent *event)
{
	// Object has been dragged outside the organiser
	update();
	event->accept();
}

void ObjectTree::dragMoveEvent(QDragMoveEvent *event)
{
	event->ignore();
}

void ObjectTree::dropEvent(QDropEvent *event)
{
	event->ignore();
}

void ObjectTree::mousePressEvent(QMouseEvent* event)
{
	// If the mouse press wasn't left button, ignore it
	if (event->buttons().testFlag(Qt::LeftButton))
	{
		// Set current position
		dragStartPos_ = event->pos();
	}

	QTreeWidget::mousePressEvent(event);
}

void ObjectTree::mouseMoveEvent(QMouseEvent* event)
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

	// Construct mime data for drag event
	ObjectData* mimeData = new ObjectData;
	addObjectData(mimeData, item, 0);

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

void ObjectTree::mouseReleaseEvent(QMouseEvent* event)
{
}

