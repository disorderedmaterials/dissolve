/*
	*** Base Viewer Input Functions
	*** src/gui/viewer/viewer_input.cpp
	Copyright T. Youngs 2013-2019

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

#include "gui/viewer/viewer.hui"
#include <QMouseEvent>
#include <QMenu>

// Mouse press event
void BaseViewer::mousePressEvent(QMouseEvent* event)
{
	// Handle button presses (button down) from the mouse
	buttonState_ = event->buttons();
	Qt::KeyboardModifiers km = event->modifiers();

	// Store event information
	rMouseDown_.set(event->x(), event->y(), 0.0);

	// Handle the event
	mousePressed();
}

// Mouse release event
void BaseViewer::mouseReleaseEvent(QMouseEvent* event)
{
	// Handle the event
	mouseReleased();

	// Clear button state
	buttonState_ = 0;
}

// Mouse move event
void BaseViewer::mouseMoveEvent(QMouseEvent* event)
{
	// Handle the event
	mouseMoved(event->x() - rMouseLast_.x, event->y() - rMouseLast_.y, event->modifiers());

	rMouseLast_.set(event->x(), event->y(), 0.0);

	setFocus();
}

// Mouse wheel event
void BaseViewer::wheelEvent(QWheelEvent* event)
{
	// Handle the event
	mouseWheeled(event->delta());

	postRedisplay();
}

// Mouse double click event
void BaseViewer::mouseDoubleClickEvent(QMouseEvent* event)
{
	// Handle the event
	mouseDoubleClicked();
}

// Return mouse coordinates at last mousedown event
Vec3<int> BaseViewer::rMouseDown() const
{
	return rMouseDown_;
}

// Return mouse coordinates at last mousemove event
Vec3<int> BaseViewer::rMouseLast() const
{
	return rMouseLast_;
}

// Key press event
void BaseViewer::keyPressEvent(QKeyEvent *event)
{
	// Handle the event
	if (keyPressed(event->key(), event->modifiers())) event->accept();
	else event->ignore();
}

// Key release event
void BaseViewer::keyReleaseEvent(QKeyEvent *event)
{
	// Handle the event
	if (keyReleased(event->key(), event->modifiers())) event->accept();
	else event->ignore();
}
