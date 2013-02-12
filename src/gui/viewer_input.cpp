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
	if ((buttonStateOnPress_.testFlag(Qt::RightButton)) && sourceSpecies_)
	{
		// Firstly, determine whether we are on top of an Atom...
		Vec4<double> v;
		Atom* i;
		for (i = sourceSpecies_->atoms(); i != NULL; i = i->next)
		{
			v = modelToScreen(i->r(), 0.3);
			v.x -= event->x();
			v.y -= (height()-event->y());
			if (sqrt(v.x*v.x + v.y*v.y) < v.w) break;
		}

		// If we *are* over an Atom, then there is more to do...
		if (i != NULL)
		{
			// If this Atom is *not* currently selected, select it after deselecting all others
			if (!sourceSpecies_->isAtomSelected(i))
			{
				sourceSpecies_->clearAtomSelection();
				sourceSpecies_->selectAtom(i);
			}
			
			// Now we have update the Atom selection (if it was necessary) we must update the
			// available isotope options in the context menu. This depends on all Atoms in the
			// current selection being of the same element.
			int el = sourceSpecies_->selectedAtoms()->item->element();
			bool same = TRUE;
			for (RefListItem<Atom,int>* ri = sourceSpecies_->selectedAtoms(); ri != NULL; ri = ri->next)
			{
				if (ri->item->element() != el)
				{
					same = FALSE;
					break;
				}
			}
			
			// If all Atoms in the selection are *not* the same element, then disable the SetIsotope menu and the SelectSame action
			QMenu* typeMenu = mainWindow_->ui.menuSelectionSetAtomType;
			typeMenu->setEnabled(same);
			mainWindow_->ui.actionSelectionSelectSame->setEnabled(same);
			
			if (same)
			{
				Dnchar text;
				QAction* action;
				// Elements are all the same, so present list of available AtomTypes in menu
				typeMenu->setEnabled(TRUE);
				typeMenu->clear();
				int count = 0;
				for (AtomType* at = mainWindow_->dUQ().atomTypes(); at != NULL; at = at->next)
				{
					if (at->element() != el) continue;
					action = typeMenu->addAction(at->name());
					QObject::connect(action, SIGNAL(triggered(bool)), mainWindow_, SLOT(selectionAtomTypeChanged(bool)));
					++count;
				}
				
				// Add dummy entry if there were no available AtomTypes
				if (count == 0)
				{
					action = typeMenu->addAction("No AtomTypes Available");
					action->setEnabled(FALSE);
				}
			}
			
			// Ready - execute the popup menu
			if (atomContextMenu_) atomContextMenu_->exec(event->globalPos());
			
			// Clear buttonstate
			buttonStateOnPress_ = 0;
		}
	}
}

// Qt Signal (mouse release event)
void Viewer::mouseReleaseEvent(QMouseEvent* event)
{
	// Handle button releases (button up) from the mouse
	Qt::KeyboardModifiers km = event->modifiers();
	
	// Check mouse button state when the mousePressEvent occurred
	if ((buttonStateOnPress_&Qt::LeftButton) && sourceSpecies_)
	{
		Vec4<double> v;
		// If the selectionBox_ is smaller than some critical size, assume it was an attempt to click an Atom
		if (abs(selectionBox_.width()*selectionBox_.height()) <= 9)
		{
			// Clear current selection (unless Ctrl is pressed)
			if (!km.testFlag(Qt::ShiftModifier)) sourceSpecies_->clearAtomSelection();
			for (Atom* i = sourceSpecies_->atoms(); i != NULL; i = i->next)
			{
				v = modelToScreen(i->r(), 0.3);
				v.x -= event->x();
				v.y -= (height()-event->y());
				if (sqrt(v.x*v.x + v.y*v.y) < v.w)
				{
					sourceSpecies_->selectAtom(i);
					break;
				}
			}
		}
		else
		{
			// Perform Box Select

			// Clear current selection (unless Shift is pressed)
			if (!km.testFlag(Qt::ShiftModifier)) sourceSpecies_->clearAtomSelection();
			for (Atom* i = sourceSpecies_->atoms(); i != NULL; i = i->next)
			{
				v = modelToScreen(i->r());
				if (selectionBox_.contains(v.x, height()-v.y)) sourceSpecies_->selectAtom(i); 
			}
		}
		
		update();
	}
	
	selectionBox_.setRect(0,0,0,0);
	buttonStateOnPress_ = 0;
	
	update();
	
	if (sourceSpecies_) mainWindow_->refresh(speciesUpdateTargets_);
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

// Set bit-array of GUI quantities to update after viewed model is modified
void Viewer::setSpeciesUpdateTargets(int targets)
{
	speciesUpdateTargets_ = targets;
}

// Set atom context menu to call
void Viewer::setAtomContextMenu(QMenu* menu)
{
	atomContextMenu_ = menu;
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
	bool refresh = FALSE, ignore = TRUE;
	Qt::KeyboardModifiers km = event->modifiers();
	
	switch (event->key())
	{
		case (Qt::Key_Left):
			if (km&Qt::ControlModifier)
			{
				printf("Why doesn't this ever get printed?\n");
			}
//			else source->rotateView( keyModifier_[Viewer::ShiftKey] ? -1.0 : -10.0, 0.0);
			refresh = TRUE;
			ignore = FALSE;
			break;
		case (Qt::Key_Right):
//			source->rotateView( keyModifier_[Viewer::ShiftKey] ? 1.0 : 10.0, 0.0);
			refresh = TRUE;
			ignore = FALSE;
			break;
		case (Qt::Key_Up):
//			source->rotateView(0.0, keyModifier_[Viewer::ShiftKey] ? -1.0 : -10.0);
			refresh = TRUE;
			ignore = FALSE;
			break;
		case (Qt::Key_Down):
//			source->rotateView(0.0, keyModifier_[Viewer::ShiftKey] ? 1.0 : 10.0);
			refresh = TRUE;
			ignore = FALSE;
			break;
		case (Qt::Key_Escape):
//			gui.mainWindow()->cancelCurrentMode();
			refresh = TRUE;
			ignore = FALSE;
			break;
		// Cycle render styles
		case (Qt::Key_F8):
			ignore = FALSE;
			break;
		// Cycle colouring styles
		case (Qt::Key_F9):
			ignore = FALSE;
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
