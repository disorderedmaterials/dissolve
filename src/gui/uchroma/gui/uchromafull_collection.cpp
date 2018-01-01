/*
	*** uChroma Full Widget - Collection 
	*** src/gui/uchroma/gui/uchromafull_collection.cpp
	Copyright T. Youngs 2013-2018

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

#include "gui/uchroma/gui/uchromafull.h"
#include "templates/reflist.h"
#include "templates/variantpointer.h"

void UChromaFullWindow::on_CollectionListButton_clicked(bool checked)
{
	if (refreshing_) return;
	ui.LeftWidgetsWidget->setVisible(checked);
}

void UChromaFullWindow::on_CollectionFocusNextButton_clicked(bool checked)
{
	focusNextCollection();
}

void UChromaFullWindow::on_CollectionFocusPreviousButton_clicked(bool checked)
{
	focusPreviousCollection();
}

void UChromaFullWindow::on_CollectionTree_currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	// Check current item
	if (current == NULL) return;

	// Get collection from item
	Collection* collection = VariantPointer<Collection>(current->data(0, Qt::UserRole));
	if (!collection) return;

	// Set new current collection
	setCurrentCollection(collection);

	// Update other parts of the GUI
	updateSubWindows();
	updateCollectionInfo();
	updateDisplay();
}

void UChromaFullWindow::on_CollectionTree_itemClicked(QTreeWidgetItem* item, int column)
{
	// Get collection from item
	Collection* collection = VariantPointer<Collection>(item->data(0, Qt::UserRole));
	if (!collection) return;

	// Set the checkstate of the collection
	collection->setVisible(item->checkState(0) == Qt::Checked);

	// Update other parts of the GUI
	updateSubWindows();
	updateDisplay();
}

void UChromaFullWindow::on_CollectionTree_itemDoubleClicked(QTreeWidgetItem* item, int column)
{
	// Get collection from item
	Collection* collection = VariantPointer<Collection>(item->data(0, Qt::UserRole));
	if (!collection) return;

	// TODO
}

void UChromaFullWindow::on_CollectionTree_itemChanged(QTreeWidgetItem* item, int column)
{
	// Check refreshing status
	if (refreshing_) return;

	// Get collection from item
	Collection* collection = VariantPointer<Collection>(item->data(0, Qt::UserRole));
	if (!collection) return;

	beginEditStateGroup("change collection title");

	// Set name of collection
	collection->setName(qPrintable(item->text(0)));

	endEditStateGroup();

	updateCollectionInfo();
}

void UChromaFullWindow::on_CollectionAddButton_clicked(bool checked)
{
	beginEditStateGroup("add new collection");

	Collection* collection = addCollection();

	endEditStateGroup();

	updateGUI();
}

void UChromaFullWindow::on_CollectionRemoveButton_clicked(bool checked)
{
	if (!currentCollection()) return;

	beginEditStateGroup("remove collection");

	removeCurrentCollection();

	// Add new empty collection if there is now no current collection
	if (currentCollection() == NULL) addCollection();

	endEditStateGroup();

	updateGUI();
}

// Update item in collections tree
void UChromaFullWindow::updateCollectionTreeItem(QTreeWidgetItem* item)
{
	if (item == NULL) return;

	// First, get Collection from item data
	Collection* collection = VariantPointer<Collection>(item->data(0, Qt::UserRole));
	if (collection == NULL) return;

	// Set basic information
	item->setText(0, collection->name());
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
	item->setCheckState(0, collection->visible() ? Qt::Checked : Qt::Unchecked);

	// Set icon
	item->setIcon(0, QIcon(collection->iconString(viewLayout().collectionUsed(collection, ViewPane::StandardRole))));

	// If this is the current collection, select it
	if (isCurrentCollection(collection)) item->setSelected(true);
}

// Add collection data to CollectionTree under specified item
void UChromaFullWindow::addCollectionsToTree(Collection* collection, QTreeWidgetItem* parent)
{
	// Add on collection item
	QTreeWidgetItem* item;
	if (parent) item = new QTreeWidgetItem(parent, 0);
	else item = new QTreeWidgetItem(ui.CollectionTree, 0);

	// Set item information
	item->setData(0, Qt::UserRole, VariantPointer<Collection>(collection));
	updateCollectionTreeItem(item);
	item->setExpanded(true);

	// Add any additional data
	for (Collection* fit = collection->fits(); fit != NULL; fit = fit->next) addCollectionsToTree(fit, item);
	for (Collection* extract = collection->slices(); extract != NULL; extract = extract->next) addCollectionsToTree(extract, item);
}

/*
 * Slots
 */

// Context menu requested for CollectionTree
void UChromaFullWindow::collectionTreeContextMenuRequested(const QPoint& point)
{
	// Build the context menu to display
	QMenu contextMenu;
	QAction* action;
	// -- Main 'edit' functions
	QAction* deleteAction = contextMenu.addAction("&Delete");
	QAction* duplicateAction = contextMenu.addAction("Du&plicate");
	// -- "Display..." pane menu
	contextMenu.addSeparator();
	QMenu paneMenu, removeFromPaneMenu, addToPaneMenu;
	RefList<ViewPane,bool> displayPanes = viewLayout().panes(ViewPane::StandardRole);
	RefList<ViewPane,bool> currentPanes = viewLayout().panes(currentCollection(), ViewPane::StandardRole);
	QList<QAction*> addToPaneActions, removeFromPaneActions;
	// -- ... Populate removeFromPaneMenu first...
	for (RefListItem<ViewPane,bool>* ri = currentPanes.first(); ri != NULL; ri = ri->next)
	{
		action = removeFromPaneMenu.addAction(ri->item->name());
		action->setData(VariantPointer<ViewPane>(ri->item));
		removeFromPaneActions << action;
	}
	// -- ... Now populate addToPaneMenu...
	for (RefListItem<ViewPane,bool>* ri = displayPanes.first(); ri != NULL; ri = ri->next)
	{
		if (currentPanes.contains(ri->item)) continue;
		action = addToPaneMenu.addAction(ri->item->name());
		action->setData(VariantPointer<ViewPane>(ri->item));
		addToPaneActions << action;
	}
	// -- Construct parent paneMenu for these submenus, and add them to it
	action = paneMenu.addMenu(&addToPaneMenu);
	action->setText("Add to pane...");
	action = paneMenu.addMenu(&removeFromPaneMenu);
	action->setText("Remove from pane...");
	action = contextMenu.addMenu(&paneMenu);
	action->setText("Display...");
	// -- Analysis
	contextMenu.addSeparator();
	QAction* fitAction = contextMenu.addAction("New &Fit Equation...");
	QAction* editFitAction = contextMenu.addAction("&Edit Fit E&quation...");
	QAction* updateFitAction = contextMenu.addAction("&Update/Continue Fit");

	// Show it
	QAction* menuResult = contextMenu.exec(QCursor::pos());

	// What was clicked?
	if (menuResult == deleteAction) on_actionCollectionDelete_triggered(false);
	else if (menuResult == duplicateAction) on_actionCollectionDuplicate_triggered(false);
	else if (removeFromPaneActions.contains(menuResult))
	{
		ViewPane* pane = VariantPointer<ViewPane>(menuResult->data());
		if (pane) pane->removeCollectionTarget(currentCollection());

		// Update the item
		updateCollectionTreeItem(ui.CollectionTree->currentItem());

		updateDisplay();
	}
	else if (addToPaneActions.contains(menuResult))
	{
		ViewPane* pane = VariantPointer<ViewPane>(menuResult->data());
		if (pane) pane->addCollectionTarget(currentCollection());

		// Update the item
		updateCollectionTreeItem(ui.CollectionTree->currentItem());

		updateDisplay();
	}
	else if (menuResult == fitAction) on_actionAnalyseNewFit_triggered(false);
	else if (menuResult == editFitAction) on_actionAnalyseEditFit_triggered(false);
	else if (menuResult == updateFitAction) on_actionAnalyseUpdateFit_triggered(false);
}

// Refresh collection list
void UChromaFullWindow::refreshCollections()
{
	refreshing_ = true;

	ui.CollectionTree->clear();
	for (Collection* collection = collections(); collection != NULL; collection = collection->next) addCollectionsToTree(collection, NULL);

	refreshing_ = false;

	// Update associated info
	updateCollectionInfo();
}

// Update displayed info for current collection
void UChromaFullWindow::updateCollectionInfo()
{
	// Update collection stats underneath main Collection list
	if (currentCollection())
	{
		ui.CollectionNSlicesLabel->setText(QString::number(currentCollection()->nDataSets()));
		ui.CollectionNPointsLabel->setText(QString::number(currentCollection()->nDataPoints()));
	}
	else
	{
		ui.CollectionNSlicesLabel->setText("0");
		ui.CollectionNPointsLabel->setText("0");
	}

	// Set number of collections in the current session
	ui.CollectionNCollectionsLabel->setText("("+QString::number(nCollections())+")");

	// Update collection info label
	if (currentCollection())
	{
		ui.InfoCurrentCollectionLabel->setText(currentCollection()->name());
		ui.InfoCurrentCollectionIconLabel->setPixmap(QPixmap(currentCollection()->iconString(viewLayout().collectionUsed(currentCollection(), ViewPane::StandardRole))));
	}
	else ui.InfoCurrentCollectionLabel->setText("<No Current Collection>");
}
