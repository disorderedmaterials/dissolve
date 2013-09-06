/*
	*** Viewer Input Functions
	*** src/gui/viewer_input.cpp
	Copyright T. Youngs 2012-2013

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

#include "gui/viewer.uih"
#include "gui/mainwindow.h"
#include "classes/atomtype.h"
#include "classes/species.h"
#include <QtGui/QMouseEvent>

/*
// Mouse Input
*/

// Qt Signal (mouse press event)
void Viewer::mousePressEvent(QMouseEvent* event)
{
	// Handle button presses (button down) from the mouse
	buttonStateOnPress_ = event->buttons();
// 	Qt::KeyboardModifiers km = event->modifiers();

	// Store event information
	rMouseDown_.set(event->x(), event->y(), 0.0);
	
	// Do something with the button press event (e.g. context menu function)?
	if ((buttonStateOnPress_.testFlag(Qt::RightButton)))
	{
		// Clear buttonstate afterwards
		buttonStateOnPress_ = 0;
	}
}

// Qt Signal (mouse release event)
void Viewer::mouseReleaseEvent(QMouseEvent* event)
{
	// Handle button releases (button up) from the mouse
	Qt::KeyboardModifiers km = event->modifiers();
	
	// Check mouse button state when the mousePressEvent occurred
	if ((buttonStateOnPress_&Qt::LeftButton))
	{

		update();
	}
	
	buttonStateOnPress_ = 0;
}

// Qt Signal (mouse move event)
void Viewer::mouseMoveEvent(QMouseEvent* event)
{
	Vec3<double> delta;
	Matrix4 A;
	
	// Get event information and position delta
	Qt::KeyboardModifiers km = event->modifiers();
	delta.set(event->x(), event->y(), 0.0);
	delta = delta - rMouseLast_;

	if (buttonStateOnPress_&Qt::LeftButton)
	{
	}
	else if (buttonStateOnPress_&Qt::RightButton)
	{
		// View manipulation
		if (km.testFlag(Qt::ShiftModifier))
		{
		}
		else if (km.testFlag(Qt::ControlModifier))
		{
		}
		else 
		{
			// Rotate the view ('XY plane')
			A.createRotationXY(delta.y/2.0, delta.x/2.0);
			A.copyTranslationAndScaling(viewMatrix_);
			// Reset translation and scaling on original matrix, and multiply
			viewMatrix_.removeTranslationAndScaling();
			viewMatrix_ = A * viewMatrix_;
		}
	}
	else if (buttonStateOnPress_&Qt::MidButton)
	{
		viewMatrix_.adjustColumn(3, delta.x/5.0, -delta.y/15.0, 0.0, 0.0);
	}
	
	rMouseLast_.set(event->x(), event->y(), 0.0);
	setFocus();
	postRedisplay();
}

// Qt Signal (mouse wheel event)
void Viewer::wheelEvent(QWheelEvent* event)
{
	bool scrollup = event->delta() > 0;
	
	// Perform camera zoom
	double dz = -viewMatrix_[14] * 0.15;
	if (scrollup) dz = -dz;
	viewMatrix_.adjustColumn(3, 0.0, 0.0, dz, 0.0);
	// Never let camera z go below -1.0...
	if (viewMatrix_[14] > -0.1) viewMatrix_[14] = -0.1;
	postRedisplay();
}

// Return mouse coordinates at last mousedown event
Vec3<double> Viewer::rMouseDown()
{
	return rMouseDown_;
}

// Return mouse coordinates at last mousemove event
Vec3<double> Viewer::rMouseLast()
{
	return rMouseLast_;
}

/*
// Key Input
*/

// Qt Slot (key press event)
void Viewer::keyPressEvent(QKeyEvent* event)
{
	// Check datamodel...
	bool refresh = false, ignore = true;
	Qt::KeyboardModifiers km = event->modifiers();
	
	switch (event->key())
	{
		case (Qt::Key_Left):
			if (km&Qt::ControlModifier)
			{
				printf("Why doesn't this ever get printed?\n");
			}
//			else source->rotateView( keyModifier_[Viewer::ShiftKey] ? -1.0 : -10.0, 0.0);
			refresh = true;
			ignore = false;
			break;
		case (Qt::Key_Right):
//			source->rotateView( keyModifier_[Viewer::ShiftKey] ? 1.0 : 10.0, 0.0);
			refresh = true;
			ignore = false;
			break;
		case (Qt::Key_Up):
//			source->rotateView(0.0, keyModifier_[Viewer::ShiftKey] ? -1.0 : -10.0);
			refresh = true;
			ignore = false;
			break;
		case (Qt::Key_Down):
//			source->rotateView(0.0, keyModifier_[Viewer::ShiftKey] ? 1.0 : 10.0);
			refresh = true;
			ignore = false;
			break;
		case (Qt::Key_Escape):
//			gui.mainWindow()->cancelCurrentMode();
			refresh = true;
			ignore = false;
			break;
		// Cycle render styles
		case (Qt::Key_F8):
			ignore = false;
			break;
		// Cycle colouring styles
		case (Qt::Key_F9):
			ignore = false;
			break;
		default:
			break;
	}
	
	// Update display if necessary
	if (refresh) postRedisplay();
	if (ignore) event->ignore();
	else event->accept();
}

// Qt Slot (key release event)
void Viewer::keyReleaseEvent(QKeyEvent* event)
{
	event->ignore();
}
