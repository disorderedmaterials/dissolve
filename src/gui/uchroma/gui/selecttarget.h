/*
	*** Select Target Dialog
	*** src/gui/uchroma/gui/selecttarget.h
	Copyright T. Youngs 2012-2018.

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

#ifndef DISSOLVE_UCHROMASELECTTARGET_H
#define DISSOLVE_UCHROMASELECTTARGET_H

#include "gui/uchroma/gui/ui_selecttarget.h"
#include <QDialog>
#include <QObject>
#include "templates/reflist.h"

// Forward Declarations
class Collection;
class ViewPane;

/*
 * Target Select Dialog
 */
class SelectTargetDialog : public QDialog
{
	Q_OBJECT

	public:
	// Constructor
	SelectTargetDialog(QWidget *parent);
	// Destructor
	~SelectTargetDialog();
	// Main form declaration
	Ui::SelectTargetDialog ui;


	/*
	 * Targets
	 */
	public:
	// Populate list with available targets
	void populateLists(ViewPane* currentPane, ViewPane* availablePanes, Collection* availableCollections);
	// Return selected panes
	RefList<ViewPane,bool> selectedPanes();
	// Return selected collections
	RefList<Collection,bool> selectedCollections();


	/*
	 * Slots
	 */
	public slots:
	void on_AvailablePanesList_itemDoubleClicked(QListWidgetItem* item);
	void on_AvailableCollectionsList_itemDoubleClicked(QListWidgetItem* item);
	void on_CancelButton_clicked(bool checked);
	void on_SelectButton_clicked(bool checked);
};

#endif
