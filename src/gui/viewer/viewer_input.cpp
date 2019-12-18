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
	// Store the current button state and mouse position (with inverted y coordinate)
	buttonState_ = event->buttons();
	rMouseDown_.set(event->x(), contextHeight_ - event->y(), 0.0);
	rMouseLast_ = rMouseDown_;
	mouseDownModifiers_ = event->modifiers();

	// If a 2D view, store the clicked local coordinate
	if (view_.isFlatView()) clicked2DAxesCoordinates_ = screenTo2DAxes(event->x(), contextHeight_ - event->y());

	interacting_ = true;

	// Handle the event
	startInteraction();
}

// Mouse release event
void BaseViewer::mouseReleaseEvent(QMouseEvent* event)
{
	// Handle the event
	endInteraction();

	postRedisplay();

	// Clear button state and interaction flag
	buttonState_ = 0;
	interacting_ = false;
}

// Mouse move event
void BaseViewer::mouseMoveEvent(QMouseEvent* event)
{
	const int dx = event->x() - rMouseLast_.x;
	const int dy = (contextHeight_ - event->y()) - rMouseLast_.y;

	// Store the new mouse coordinate with inverted y coordinate
	rMouseLast_.set(event->x(), contextHeight_ - event->y(), 0.0);

	// If a 2D view, store the current local Axes coordinate
	if (view_.isFlatView()) current2DAxesCoordinates_ = screenTo2DAxes(rMouseLast_.x, rMouseLast_.y);

	// Handle the event, passing the delta position
	mouseMoved(dx, dy);

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
	if (keyPressed(event->key())) event->accept();
	else event->ignore();
}

// Key release event
void BaseViewer::keyReleaseEvent(QKeyEvent *event)
{
	// Handle the event
	if (keyReleased(event->key())) event->accept();
	else event->ignore();
}

/*
 * Protected Virtuals
 */

// Mouse moved
void BaseViewer::mouseMoved(int dx, int dy)
{
}

// Mouse 'wheeled'
void BaseViewer::mouseWheeled(int delta)
{
	bool scrollup = delta > 0;

	// Perform camera zoom
	double zrange = view_.axes().stretch(2) * view_.axes().realRange(2);
	if (zrange < 1.0) zrange = 1.0;
	view_.translateView(0.0, 0.0, 0.5*zrange*(scrollup ? -1.0 : 1.0));

	// Never let camera z go above z = 1.0...
	Vec3<double> trans = view_.viewTranslation();
	if (trans.z > 1.0) trans.z = 1.0;
	view_.setViewTranslation(trans.x, trans.y, trans.z);

	postRedisplay();
}

// Mouse double clicked
void BaseViewer::mouseDoubleClicked()
{
}

// Key pressed
bool BaseViewer::keyPressed(int key)
{
	return false;
}

// Key released
bool BaseViewer::keyReleased(int key)
{
	return false;
}
