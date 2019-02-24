/*
	*** Species Viewer - Input
	*** src/gui/viewer/speciesviewer_input.cpp
	Copyright T. Youngs 2019

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

#include "gui/viewer/speciesviewer.hui"
#include "classes/species.h"
#include <QtGui/QMouseEvent>

// Mouse moved
void SpeciesViewer::mouseMoved(int dx, int dy, Qt::KeyboardModifiers modifiers)
{
	// If we are not actually interacting with the view, return now
	if (!interacting()) return;

	bool refresh = false;

	// What we do here depends on the current mode
	switch (interactionMode_)
	{
		case (SpeciesViewer::SelectAreaInteraction):
			// No action to take - the selection box will be drawn from the clicked and current positions (already stored)
			refresh = true;
			break;
		case (SpeciesViewer::RotateViewInteraction):
			// Rotate view
			if (modifiers.testFlag(Qt::ShiftModifier))
			{
			}
			else if (modifiers.testFlag(Qt::ControlModifier))
			{
			}
			else 
			{
				view_.rotateView(-dy/2.0, dx/2.0);
				refresh = true;
			}
			break;
		case (SpeciesViewer::TranslateViewInteraction):
			// If this is a flat view, shift the axis limits rather than translating the view
			if (view_.isFlatView()) view_.shiftFlatAxisLimits(dx, dy);
			else view_.translateView(dx/15.0, dy/15.0, 0.0);
			refresh = true;
			break;
		default:
			break;
	}

	if (refresh) postRedisplay();
}

// Mouse 'wheeled'
void SpeciesViewer::mouseWheeled(int delta)
{
	bool scrollup = delta > 0;

	// Perform camera zoom
	double zrange = view_.axes().stretch(2) * view_.axes().realRange(2);
	if (zrange < 1.0) zrange = 1.0;
	view_.translateView(0.0, 0.0, 0.5*zrange*(scrollup ? -1.0 : 1.0));

	postRedisplay();
}

// Mouse double clicked
void SpeciesViewer::mouseDoubleClicked()
{
}

// Key pressed
bool SpeciesViewer::keyPressed(int key, Qt::KeyboardModifiers modifiers)
{
}

// Key released
bool SpeciesViewer::keyReleased(int key, Qt::KeyboardModifiers modifiers)
{
}

/*
// Qt Signal (mouse release event)
void SpeciesViewer::mouseReleaseEvent(QMouseEvent* event)
{
	// Handle button releases (button up) from the mouse
	Qt::KeyboardModifiers km = event->modifiers();
	
	// Check mouse button state when the mousePressEvent occurred
	if ((buttonStateOnPress_&Qt::LeftButton) && species_)
	{
		Vec4<double> v;
		// If the selectionBox_ is smaller than some critical size, assume it was an attempt to click an Atom
		if (abs(selectionBox_.width()*selectionBox_.height()) <= 9)
		{
			// Clear current selection (unless Ctrl is pressed)
			if (!km.testFlag(Qt::ShiftModifier)) species_->clearAtomSelection();
			for (SpeciesAtom* i = species_->atoms().first(); i != NULL; i = i->next)
			{
				v = modelToScreen(i->r(), 0.3);
				v.x -= event->x();
				v.y -= (height()-event->y());
				if (sqrt(v.x*v.x + v.y*v.y) < v.w)
				{
					species_->selectAtom(i);
					break;
				}
			}
		}
		else
		{
			// Perform Box Select

			// Clear current selection (unless Shift is pressed)
			if (!km.testFlag(Qt::ShiftModifier)) species_->clearAtomSelection();
			for (SpeciesAtom* i = species_->atoms().first(); i != NULL; i = i->next)
			{
				v = modelToScreen(i->r());
				if (selectionBox_.contains(v.x, height()-v.y)) species_->selectAtom(i); 
			}
		}
		
		update();
	}
	
	selectionBox_.setRect(0,0,0,0);
	buttonStateOnPress_ = 0;
	
	update();
	
// 	if (species_) mainWindow_->refresh(speciesUpdateTargets_);
}

// Qt Signal (mouse move event)
void SpeciesViewer::mouseMoveEvent(QMouseEvent* event)
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
void SpeciesViewer::wheelEvent(QWheelEvent* event)
{
	bool scrollup = event->delta() > 0;
	
	// Perform camera zoom
	double dz = -viewMatrix_[14] * 0.15;
	if (scrollup) dz = -dz;
	viewMatrix_.adjustColumn(3, 0.0, 0.0, dz, 0.0);
	// Never let camera z go below -1.0...
	if (viewMatrix_[14] > -0.1) viewMatrix_[14] = -0.1;
	postRedisplay();
}*/
