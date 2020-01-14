/*
	*** Data Viewer - Context Menu
	*** src/gui/viewer/dataviewer_contextmenu.cpp
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

#include "gui/viewer/dataviewer.hui"
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
	RefDataList<QAction,DataViewer*> destinationActions;
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
// 	if (renderableDestinations_.nItems() == 0) copyToMenu->setEnabled(false);
// 	else
// 	{
// 		int count = 1;
// 		RefListIterator<DataViewer> destinationIterator(renderableDestinations_);
// 		while (DataViewer* destination = destinationIterator.iterate())
// 		{
// 			action = copyToMenu->addAction(QString("%1. %2").arg(QString::number(count++)).arg(destination->destinationName()));
// 			destinationActions.append(action, destination);
// 		}
// 	}

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
		DataViewer* destination = destinationActions.dataForItem(selectedAction);
		if (!destination) return;
		destination->createRenderable(rend->type(), rend->objectTag(), rend->name(), "Default");
	}

	// Done
	postRedisplay();
}
