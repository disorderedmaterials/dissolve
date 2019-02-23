/*
	*** Species Viewer - Context Menu
	*** src/gui/viewer/speciesviewer_context.cpp
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
#include "templates/variantpointer.h"
#include <QClipboard>

/// Initialise context menu
void SpeciesViewer::initialiseContextMenu()
{
// 	// Set font for menu, and create italic version
// 	QFont menuFont = font();
// 	menuFont.setPointSize(8);
// 	contextMenu_.setFont(menuFont);
// 	QFont italicFont = font();
// 	italicFont.setItalic(true);
// 
// 	// View-wide functions
// 	contextMenu_.addSeparator();
// 	QAction* action = contextMenu_.addAction("Copy image");
// 	connect(action, SIGNAL(triggered(bool)), this, SLOT(copyViewToClipboard(bool)));
}

// Update dynamic aspects of context menu before display
void SpeciesViewer::updateContextMenu()
{
}

// 		// Firstly, determine whether we are on top of an Atom...
// 		Vec4<double> v;
// 		SpeciesAtom* i;
// 		for (i = species_->atoms().first(); i != NULL; i = i->next)
// 		{
// // 			v = view_.modelToScreen(i->r(), 0.3);
// // 			v.x -= event->x();
// // 			v.y -= (height()-event->y());
// // 			if (sqrt(v.x*v.x + v.y*v.y) < v.w) break;
// 		}
// 
// 		// If we *are* over an Atom, then there is more to do...
// 		if (i != NULL)
// 		{
// 			// If this Atom is *not* currently selected, select it after deselecting all others
// 			if (!species_->isAtomSelected(i))
// 			{
// 				species_->clearAtomSelection();
// 				species_->selectAtom(i);
// 			}
// 			
// 			// Now we have update the Atom selection (if it was necessary) we must update the
// 			// available isotope options in the context menu. This depends on all Atoms in the
// 			// current selection being of the same element.
// // 			int el = species_->selectedAtoms()->item->element();
// // 			bool same = true;
// // 			for (RefListItem<SpeciesAtom,int>* ri = species_->selectedAtoms(); ri != NULL; ri = ri->next)
// // 			{
// // 				if (ri->item->element() != el)
// // 				{
// // 					same = false;
// // 					break;
// // 				}
// // 			}
// 
// 			// If all Atoms in the selection are *not* the same element, then disable the SetIsotope menu and the SelectSame action
// // 			QMenu* typeMenu = mainWindow_->ui.menuSelectionSetAtomType;
// // 			typeMenu->setEnabled(same);
// // 			mainWindow_->ui.actionSelectionSelectSame->setEnabled(same);
// 			// TODO TODO TODO
// // 			if (same)
// // 			{
// // 				Dnchar text;
// // 				QAction* action;
// // 				// Elements are all the same, so present list of available AtomTypes in menu
// // 				typeMenu->setEnabled(true);
// // 				typeMenu->clear();
// // 				int count = 0;
// // 				for (AtomType* at = mainWindow_->Dissolve().atomTypes(); at != NULL; at = at->next)
// // 				{
// // 					if (at->element() != el) continue;
// // 					action = typeMenu->addAction(at->name());
// // 					QObject::connect(action, SIGNAL(triggered(bool)), mainWindow_, SLOT(selectionAtomTypeChanged(bool)));
// // 					++count;
// // 				}
// // 				
// // 				// Add dummy entry if there were no available AtomTypes
// // 				if (count == 0)
// // 				{
// // 					action = typeMenu->addAction("No AtomTypes Available");
// // 					action->setEnabled(false);
// // 				}
// // 			}
// // 			
// 			// Ready - execute the popup menu
// // 			if (atomContextMenu_) atomContextMenu_->exec(event->globalPos());
// 		}
