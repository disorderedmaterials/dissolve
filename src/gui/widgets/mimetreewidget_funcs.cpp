// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/widgets/mimetreewidget.hui"
#include "gui/widgets/mimetreewidgetitem.h"
#include <QApplication>
#include <QDrag>
#include <QMouseEvent>

MimeTreeWidget::MimeTreeWidget(QWidget *parent) : QTreeWidget(parent) { setAcceptDrops(true); }

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

void MimeTreeWidget::dragMoveEvent(QDragMoveEvent *event) { event->ignore(); }

void MimeTreeWidget::dropEvent(QDropEvent *event) { event->ignore(); }

void MimeTreeWidget::mousePressEvent(QMouseEvent *event)
{
    // If the mouse press wasn't left button, ignore it
    if (event->buttons().testFlag(Qt::LeftButton))
    {
        // Set current position
        dragStartPos_ = event->pos();
    }

    QTreeWidget::mousePressEvent(event);
}

void MimeTreeWidget::mouseMoveEvent(QMouseEvent *event)
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
    QTreeWidgetItem *item = itemAt(dragStartPos_);
    if (!item)
        return;

    // Cast the item into a MimeTreeWidgetItem
    auto *mimeItem = dynamic_cast<MimeTreeWidgetItem *>(item);
    if (!item)
        return;

    // Construct mime data for drag event - copy MimeStrings object from MimeTreeWidget
    auto *mimeData = new MimeStrings;
    mimeData->add(mimeItem->mimeStrings());

    auto *drag = new QDrag(this);
    drag->setMimeData(mimeData);

    // Begin the drag event
    if (drag->exec(Qt::MoveAction) != Qt::MoveAction)
    {
        // Reset everything, and do not perform a move
        update();
    }

    event->accept();
}

void MimeTreeWidget::mouseReleaseEvent(QMouseEvent *event) { QTreeWidget::mouseReleaseEvent(event); }
