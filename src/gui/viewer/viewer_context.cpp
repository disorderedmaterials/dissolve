/*
	*** Viewer Context Menu Functions
	*** src/gui/uchroma/gui/viewer_context.cpp
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

#include "gui/uchroma/gui/viewer.hui"
#include "gui/uchroma/uchromabase.h"
#include "templates/variantpointer.h"
#include <QClipboard>

/*
 * Private Funtions
 */

// Initialise context menu
void UChromaViewer::initialiseContextMenu()
{
	// Set font for menu, and create italic version
	QFont menuFont = font();
	menuFont.setPointSize(8);
	contextMenu_.setFont(menuFont);
	QFont italicFont = font();
	italicFont.setItalic(true);

	// Top action is name of current pane (disabled)
	currentPaneAction_ = contextMenu_.addAction("Current Pane");
	currentPaneAction_->setFont(italicFont);
	currentPaneAction_->setEnabled(false);
	currentCollectionsMenu_ = contextMenu_.addMenu("Collections");

	QAction* action = contextMenu_.addAction("Import...");
	action->setData(UChromaBase::UChromaImportDialog);
	connect(action, SIGNAL(triggered(bool)), this, SLOT(showDialog(bool)));

	// View-wide functions
	contextMenu_.addSeparator();
	action = contextMenu_.addAction("Copy image");
	connect(action, SIGNAL(triggered(bool)), this, SLOT(copyViewToClipboard(bool)));
}

// Update dynamic aspects of context menu before display
void UChromaViewer::updateContextMenu()
{
	ViewPane* viewPane = uChromaBase_->currentViewPane();

	// Update name of current ViewPane
	currentPaneAction_->setText(viewPane ? viewPane->name() : "No Current Pane");

	// Update current Collections
	currentCollectionsMenu_->clear();
	for (Collection* collection = uChromaBase_->collections(); collection != NULL; collection = collection->next)
	{
		// Add an action for the Collection
		QAction* action = currentCollectionsMenu_->addAction(collection->name());
		action->setData(VariantPointer<Collection>(collection));
		action->setCheckable(true);
		action->setChecked(viewPane->collectionIsTarget(collection));
		connect(action, SIGNAL(triggered(bool)), this, SLOT(setCollectionDisplayed(bool)));
	}
}

/*
 * Private Slots
 */

// Show dialog
void UChromaViewer::showDialog(bool checked)
{
	// Get sender
	QAction* action = (QAction*) sender();
	if (!action) return;

	uChromaBase_->showDialog(action->data().toInt());
}

// Set display status of Collection target
void UChromaViewer::setCollectionDisplayed(bool checked)
{
	// Get collection from data attached to sender
	QAction* action = (QAction*) sender();
	if (!action) return;
	Collection* collection = VariantPointer<Collection>(action->data());

	// Add / remove the specified Collection as a target for the current view pane
	ViewPane* viewPane = uChromaBase_->currentViewPane();
	if (checked) viewPane->addCollectionTarget(collection);
	else viewPane->removeCollectionTarget(collection);

	postRedisplay();
}

// Copy current view image to clipboard
void UChromaViewer::copyViewToClipboard(bool checked)
{
	// Generate image of current view
	QPixmap pixmap = frameBuffer();

	// Copy image to clipboard
	QClipboard* clipboard = QApplication::clipboard();
	clipboard->setImage(pixmap.toImage());
}

/*
 * Public Slots
 */

// Show context menu
void UChromaViewer::showContextMenu(const QPoint& pos)
{
	// The clicked pane will become the current pane
	if (uChromaBase_->setCurrentViewPane(pos.x(), height()-pos.y())) postRedisplay();

	updateContextMenu();

	contextMenu_.exec(mapToGlobal(pos));
}
