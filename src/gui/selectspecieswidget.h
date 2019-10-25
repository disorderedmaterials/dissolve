/*
	*** Select Species Widget
	*** src/gui/selectspecieswidget.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_WIDGET_SELECTSPECIES_H
#define DISSOLVE_WIDGET_SELECTSPECIES_H

#include "gui/ui_selectspecieswidget.h"
#include "data/fflibrary.h"
#include "templates/list.h"
#include <QWidget>

// Forward Declarations
class Dissolve;
class Species;

// Select Species Widget
class SelectSpeciesWidget : public QWidget
{
	Q_OBJECT

	public:
	// Constructor
	SelectSpeciesWidget(QWidget* parent);
	// Destructor
	~SelectSpeciesWidget();


	/*
	 * UI
	 */
	private:
	// Main form declaration
	Ui::SelectSpeciesWidget ui_;
	// Whether the widget is refreshing
	bool refreshing_;


	/*
	 * Data
	 */
	private:
	// CoreData containing available Species
	const CoreData* coreData_;
	// Minimum number of Species in a valid selection
	int minimumSelectionSize_;
	// Maximum number of Species in a valid selection (-1 for no limit)
	int maximumSelectionSize_;

	public:
	// Set CoreData containing available Species
	void setCoreData(const CoreData* coreData);
	// Reset widget, applying specified min and max limits to selection
	void reset(int minSize, int maxSize);


	/*
	 * Update
	 */
	private:
	// Update the list of Species
	void updateSpeciesList();


	/*
	 * Signals / Slots
	 */
	private slots:
	void on_SelectNoneButton_clicked(bool checked);
	void on_SelectAllButton_clicked(bool checked);
	void on_SpeciesList_itemSelectionChanged();
	void on_SpeciesList_itemDoubleClicked(QListWidgetItem* item);

	signals:
	void speciesSelectionChanged(bool isValid);
	void speciesDoubleClicked();

	private:
	// Return whether number of selected items is valid
	bool isSelectionValid() const;

	public:
	// Return number of species currently selected
	int nSelected() const;
	// Return the currently-selected Species
	RefList<Species> currentSpecies() const;
};

#endif
