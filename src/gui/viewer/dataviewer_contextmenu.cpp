/*
	*** Data Viewer - Context Menu
	*** src/gui/viewer/dataviewer_context.cpp
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
#include "templates/variantpointer.h"
#include <QClipboard>

/// Initialise context menu
void DataViewer::initialiseContextMenu()
{
	// Set font for menu, and create italic version
	QFont menuFont = font();
	menuFont.setPointSize(8);
	contextMenu_.setFont(menuFont);
	QFont italicFont = font();
	italicFont.setItalic(true);

	// Create special menu items
	currentDataMenu_ = contextMenu_.addMenu("Data");

// 	QAction* action = contextMenu_.addAction("Import...");
// 	action->setData(UChromaBase::UChromaImportDialog);
// 	connect(action, SIGNAL(triggered(bool)), this, SLOT(showDialog(bool)));

	// View-wide functions
	contextMenu_.addSeparator();
	QAction* action = contextMenu_.addAction("Copy image");
	connect(action, SIGNAL(triggered(bool)), this, SLOT(copyViewToClipboard(bool)));
}

// Update dynamic aspects of context menu before display
void DataViewer::updateContextMenu()
{
	// Update current data
	currentDataMenu_->clear();
	for (Renderable* rend = renderables_.first(); rend != NULL; rend = rend->next)
	{
		// Add an action containing the Renderable data
		QAction* action = currentDataMenu_->addAction(rend->name());
		action->setData(VariantPointer<Renderable>(rend));
		action->setCheckable(true);
		action->setChecked(rend->isVisible());
		connect(action, SIGNAL(triggered(bool)), this, SLOT(contextMenuSetDataVisible(bool)));
	}
}

/*
 * Private Slots
 */

// Show dialog
void DataViewer::showDialog(bool checked)
{
	// Get sender
	QAction* action = (QAction*) sender();
	if (!action) return;

// 	showDialog(action->data().toInt());
}

// Set visibility of Renderable target
void DataViewer::contextMenuSetDataVisible(bool checked)
{
	// Get collection from data attached to sender
	QAction* action = (QAction*) sender();
	if (!action) return;
	Renderable* rend = VariantPointer<Renderable>(action->data());
	if (rend) rend->setVisible(checked);

	postRedisplay();
}
