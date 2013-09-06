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

#include "gui/speciesviewer.uih"
// #include "gui/mainwindow.h"
// #include "classes/atomtype.h"
#include "classes/species.h"
#include <QtGui/QMouseEvent>

/*
// Mouse Input
*/

// Qt Signal (mouse press event)
void SpeciesViewer::mousePressEvent(QMouseEvent* event)
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
		SpeciesAtom* i;
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
			bool same = true;
			for (RefListItem<SpeciesAtom,int>* ri = sourceSpecies_->selectedAtoms(); ri != NULL; ri = ri->next)
			{
				if (ri->item->element() != el)
				{
					same = false;
					break;
				}
			}
			
			// If all Atoms in the selection are *not* the same element, then disable the SetIsotope menu and the SelectSame action
// 			QMenu* typeMenu = mainWindow_->ui.menuSelectionSetAtomType;
// 			typeMenu->setEnabled(same);
// 			mainWindow_->ui.actionSelectionSelectSame->setEnabled(same);
			// TODO TODO TODO
// 			if (same)
// 			{
// 				Dnchar text;
// 				QAction* action;
// 				// Elements are all the same, so present list of available AtomTypes in menu
// 				typeMenu->setEnabled(true);
// 				typeMenu->clear();
// 				int count = 0;
// 				for (AtomType* at = mainWindow_->dUQ().atomTypes(); at != NULL; at = at->next)
// 				{
// 					if (at->element() != el) continue;
// 					action = typeMenu->addAction(at->name());
// 					QObject::connect(action, SIGNAL(triggered(bool)), mainWindow_, SLOT(selectionAtomTypeChanged(bool)));
// 					++count;
// 				}
// 				
// 				// Add dummy entry if there were no available AtomTypes
// 				if (count == 0)
// 				{
// 					action = typeMenu->addAction("No AtomTypes Available");
// 					action->setEnabled(false);
// 				}
// 			}
// 			
			// Ready - execute the popup menu
// 			if (atomContextMenu_) atomContextMenu_->exec(event->globalPos());
			
			// Clear buttonstate
			buttonStateOnPress_ = 0;
		}
	}
}

// Qt Signal (mouse release event)
void SpeciesViewer::mouseReleaseEvent(QMouseEvent* event)
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
			for (SpeciesAtom* i = sourceSpecies_->atoms(); i != NULL; i = i->next)
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
			for (SpeciesAtom* i = sourceSpecies_->atoms(); i != NULL; i = i->next)
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
	
// 	if (sourceSpecies_) mainWindow_->refresh(speciesUpdateTargets_);
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
}
