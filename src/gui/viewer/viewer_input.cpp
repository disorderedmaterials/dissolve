/*
	*** Base Viewer Input Functions
	*** src/gui/viewer/viewer_input.cpp
	Copyright T. Youngs 2013-2019

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

#include "gui/viewer/viewer.hui"
#include <QMouseEvent>
#include <QMenu>

/*
 * Private Functions
 */

// Mouse press event
void BaseViewer::mousePressEvent(QMouseEvent* event)
{
	// Store the current button state and mouse position
	buttonState_ = event->buttons();
	rMouseDown_.set(event->x(), event->y(), 0.0);

	// Handle the event
	startInteraction(rMouseDown_.x, contextHeight_-rMouseDown_.y, event->modifiers());
}

// Mouse release event
void BaseViewer::mouseReleaseEvent(QMouseEvent* event)
{
	// Handle the event
	endInteraction(rMouseLast_.x, contextHeight_-rMouseLast_.y);

	postRedisplay();

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

/*
 * Private Slots
 */

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

/*
 * Protected Virtuals
 */

// Mouse moved
void BaseViewer::mouseMoved(int dx, int dy, Qt::KeyboardModifiers modifiers)
{
}

// Mouse 'wheeled'
void BaseViewer::mouseWheeled(int delta)
{
}

// Mouse double clicked
void BaseViewer::mouseDoubleClicked()
{
}

// Key pressed
bool BaseViewer::keyPressed(int key, Qt::KeyboardModifiers modifiers)
{
	return false;
}

// Key released
bool BaseViewer::keyReleased(int key, Qt::KeyboardModifiers modifiers)
{
	return false;
}

/*
 * Public Functions
 */

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
