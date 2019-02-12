/*
	*** Data Viewer - Context Menu
	*** src/gui/viewer/dataviewer_context.cpp
	Copyright T. Youngs 2013-2019

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
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
	currentCollectionsMenu_ = contextMenu_.addMenu("Collections");

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
	// Update current Collections
	currentCollectionsMenu_->clear();
	for (Collection* collection = collections(); collection != NULL; collection = collection->next)
	{
		// Add an action for the Collection
		QAction* action = currentCollectionsMenu_->addAction(collection->name());
		action->setData(VariantPointer<Collection>(collection));
		action->setCheckable(true);
		action->setChecked(view_.collectionIsTarget(collection));
		connect(action, SIGNAL(triggered(bool)), this, SLOT(setCollectionDisplayed(bool)));
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

	showDialog(action->data().toInt());
}

// Set display status of Collection target
void DataViewer::setCollectionDisplayed(bool checked)
{
	// Get collection from data attached to sender
	QAction* action = (QAction*) sender();
	if (!action) return;
	Collection* collection = VariantPointer<Collection>(action->data());

	// Add / remove the specified Collection as a target for the current view pane
	if (checked) view_.addCollectionTarget(collection);
	else view_.removeCollectionTarget(collection);

	postRedisplay();
}
