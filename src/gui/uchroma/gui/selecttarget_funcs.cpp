/*
	*** Select Target Functions
	*** src/gui/uchroma/gui/selecttarget_funcs.cpp
	Copyright T. Youngs 2012-2015

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

#include "gui/uchroma/gui/selecttarget.h"
#include "gui/uchroma/classes/collection.h"
#include "gui/uchroma/classes/viewpane.h"
#include "templates/variantpointer.h"

// Constructor
SelectTargetDialog::SelectTargetDialog(QWidget* parent) : QDialog(parent)
{
	ui.setupUi(this);
}

// Destructor
SelectTargetDialog::~SelectTargetDialog()
{
}

/*
 * Target
 */

// Populate list with available targets
void SelectTargetDialog::populateLists(ViewPane* currentPane, ViewPane* availablePanes, Collection* availableCollections)
{
	// Populate the panes list
	QListWidgetItem* item;
	for (ViewPane* pane = availablePanes; pane != NULL; pane = pane->next)
	{
		// Don't add this pane if it is already in the current pane's list of targets, or is the current pane itself
		if (pane == currentPane) continue;
		if (currentPane->paneIsTarget(pane)) continue;
		item = new QListWidgetItem(ui.AvailablePanesList);
		item->setText(pane->name());
		item->setData(Qt::UserRole, VariantPointer<ViewPane>(pane));
	}

	// Populate the collections list
	for (Collection* collection = availableCollections; collection != NULL; collection = collection->next)
	{
		// Don't add this collection if it is already in the current pane's list of targets
		if (currentPane->collectionIsTarget(collection)) continue;
		item = new QListWidgetItem(ui.AvailableCollectionsList);
		item->setText(collection->name());
		item->setData(Qt::UserRole, VariantPointer<Collection>(collection));
	}
}

// Return selected panes
RefList<ViewPane,bool> SelectTargetDialog::selectedPanes()
{
	RefList<ViewPane,bool> result;
	QList<QListWidgetItem*> items = ui.AvailablePanesList->selectedItems();
	for (int n=0; n<items.count(); ++n) result.add(VariantPointer<ViewPane>(items[n]->data(Qt::UserRole)));
	return result;
}

// Return selected collections
RefList<Collection,bool> SelectTargetDialog::selectedCollections()
{
	RefList<Collection,bool> result;
	QList<QListWidgetItem*> items = ui.AvailableCollectionsList->selectedItems();
	for (int n=0; n<items.count(); ++n) result.add(VariantPointer<Collection>(items[n]->data(Qt::UserRole)));
	return result;
}

/*
 * Slots
 */

void SelectTargetDialog::on_AvailablePanesList_itemDoubleClicked(QListWidgetItem* item)
{
	accept();
}

void SelectTargetDialog::on_AvailableCollectionsList_itemDoubleClicked(QListWidgetItem* item)
{
	accept();
}

void SelectTargetDialog::on_CancelButton_clicked(bool checked)
{
	reject();
}

void SelectTargetDialog::on_SelectButton_clicked(bool checked)
{
	accept();
}
