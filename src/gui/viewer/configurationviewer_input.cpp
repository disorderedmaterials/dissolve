/*
	*** Config Viewer Input Functions
	*** src/gui/viewer/configviewer_input.cpp
	Copyright T. Youngs 2012-2013

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

#include "gui/viewer/configviewer.hui"
#include "classes/species.h"
#include <QtGui/QMouseEvent>

/*
 * Mouse Input
 */

// Qt Signal (mouse press event)
void ConfigurationViewer::mousePressEvent(QMouseEvent* event)
{
	// Handle button presses (button down) from the mouse
	buttonStateOnPress_ = event->buttons();
// 	Qt::KeyboardModifiers km = event->modifiers();

	// Store event information
	rMouseDown_.set(event->x(), event->y(), 0.0);
	
	// Do something with the button press event (e.g. context menu function)?
	if (buttonStateOnPress_.testFlag(Qt::RightButton))
	{
	
	}
}

// Qt Signal (mouse release event)
void ConfigurationViewer::mouseReleaseEvent(QMouseEvent* event)
{
	// Handle button releases (button up) from the mouse
	Qt::KeyboardModifiers km = event->modifiers();
	
	// Check mouse button state when the mousePressEvent occurred
	if (buttonStateOnPress_&Qt::LeftButton)
	{
	}
	
	selectionBox_.setRect(0,0,0,0);
	buttonStateOnPress_ = 0;
}

// Qt Signal (mouse move event)
void ConfigurationViewer::mouseMoveEvent(QMouseEvent* event)
{
	Vec3<double> delta;
	Matrix4 A;
	
	// Get event information and position delta
	Qt::KeyboardModifiers km = event->modifiers();
	delta.set(event->x(), event->y(), 0.0);
	delta = delta - rMouseLast_;

	if (buttonStateOnPress_&Qt::LeftButton)
	{
		selectionBox_.setRect(rMouseDown_.x, rMouseDown_.y, event->x()-rMouseDown_.x, event->y()-rMouseDown_.y);
	}
	else if (buttonStateOnPress_&Qt::RightButton)
	{
		// View manipulation
		if (km&Qt::ShiftModifier)
		{
		}
		else if (km&Qt::ControlModifier)
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
void ConfigurationViewer::wheelEvent(QWheelEvent* event)
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
