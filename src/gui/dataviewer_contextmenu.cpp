/*
	*** Data Viewer - Context Menu
	*** src/gui/dataviewer_contextmenu.cpp
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

#include "gui/dataviewer.hui"
#include "gui/gizmo.h"
#include "base/messenger.h"

/*
 * Private Functions
 */

// Show general context menu
void DataViewer::showGeneralContextMenu(QPoint pos)
{
	QMenu menu;
	QAction* action;
	menu.setFont(font());

	// Execute the menu
	menu.exec(mapToGlobal(pos));
}

// Show renderable context menu
void DataViewer::showRenderableContextMenu(QPoint pos, Renderable* rend)
{
	QMenu menu;
	QAction* action;
	RefDataList<QAction,Gizmo*> destinationActions;
	menu.setFont(font());
	QFont italicFont(menu.font());
	italicFont.setItalic(true);

	// Add header
	action = menu.addAction(queryObjectInfo());
	action->setEnabled(false);
	action->setFont(italicFont);
	menu.addSeparator();

	// -- Hide Renderable
	QAction* hideAction = menu.addAction("&Hide");

	// -- Copy To...
	QMenu* copyToMenu = menu.addMenu("&Copy to...");
	copyToMenu->setFont(menu.font());
	// Get list of viable destinations that will accept our data
	RefList<Gizmo> destinations = Gizmo::allThatAccept(Renderable::renderableTypes().keyword(rend->type()));
	if (destinations.nItems() == 0) copyToMenu->setEnabled(false);
	else
	{
		RefListIterator<Gizmo> destinationIterator(destinations);
		while (Gizmo* destination = destinationIterator.iterate())
		{
			action = copyToMenu->addAction(destination->uniqueName());
			destinationActions.append(action, destination);
		}
	}

	// Execute the menu
	QAction* selectedAction = menu.exec(mapToGlobal(pos));

	// Act on the action!
	if (selectedAction == hideAction)
	{
		rend->setVisible(false);
		emit(renderableChanged());
	}
	else if (destinationActions.contains(selectedAction))
	{
		Gizmo* destination = destinationActions.dataForItem(selectedAction);
		if (!destination) return;
		destination->sendData(Renderable::renderableTypes().keyword(rend->type()), rend->objectTag(), rend->name());
	}

	// Done
	postRedisplay();
}
