/*
    *** Select Species Widget
    *** src/gui/selectspecieswidget_funcs.cpp
    Copyright T. Youngs 2012-2020

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

#include "classes/coredata.h"
#include "classes/species.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/selectspecieswidget.h"
#include "templates/variantpointer.h"

SelectSpeciesWidget::SelectSpeciesWidget(QWidget *parent) : QWidget(parent)
{
    ui_.setupUi(this);

    coreData_ = nullptr;
    minimumSelectionSize_ = 1;
    maximumSelectionSize_ = 1;

    refreshing_ = false;
}

SelectSpeciesWidget::~SelectSpeciesWidget() {}

/*
 * Data
 */

// Set CoreData containing available Species
void SelectSpeciesWidget::setCoreData(const CoreData *coreData)
{
    coreData_ = coreData;

    updateSpeciesList();
}

// Reset widget, applying specified min and max limits to selection
void SelectSpeciesWidget::reset(int minSize, int maxSize)
{
    minimumSelectionSize_ = minSize;
    maximumSelectionSize_ = maxSize;

    ui_.SpeciesList->clearSelection();

    // Set the correct selection behaviour and enable/disable select all / none buttons as appropriate
    if (maxSize == 1)
        ui_.SpeciesList->setSelectionMode(QAbstractItemView::SingleSelection);
    else
        ui_.SpeciesList->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui_.SelectionControls->setVisible(maxSize != 1);
}

/*
 * Update
 */

// Update the list of Species
void SelectSpeciesWidget::updateSpeciesList()
{
    if (!coreData_)
    {
        ui_.SpeciesList->clear();
        return;
    }

    ListWidgetUpdater<SelectSpeciesWidget, Species> speciesUpdater(ui_.SpeciesList, coreData_->constSpecies());
}

void SelectSpeciesWidget::on_SelectNoneButton_clicked(bool checked) { ui_.SpeciesList->clearSelection(); }

void SelectSpeciesWidget::on_SelectAllButton_clicked(bool checked) { ui_.SpeciesList->selectAll(); }

void SelectSpeciesWidget::on_SpeciesList_itemSelectionChanged() { emit(speciesSelectionChanged(isSelectionValid())); }

void SelectSpeciesWidget::on_SpeciesList_itemDoubleClicked(QListWidgetItem *item)
{
    if (!item)
        return;

    emit(speciesDoubleClicked());
}

// Return whether number of selected items is valid
bool SelectSpeciesWidget::isSelectionValid() const
{
    auto count = nSelected();

    if (count < minimumSelectionSize_)
        return false;
    else if (maximumSelectionSize_ == -1)
        return true;
    else
        return (count <= maximumSelectionSize_);
}

// Return number of species currently selected
int SelectSpeciesWidget::nSelected() const
{
    auto count = 0;
    for (int n = 0; n < ui_.SpeciesList->count(); ++n)
    {
        QListWidgetItem *item = ui_.SpeciesList->item(n);

        if (item->isSelected())
            ++count;
    }

    return count;
}

// Return the currently-selected Species
RefList<Species> SelectSpeciesWidget::currentSpecies() const
{
    RefList<Species> selection;

    // Loop over items in the list and construct the selection RefList
    for (int n = 0; n < ui_.SpeciesList->count(); ++n)
    {
        QListWidgetItem *item = ui_.SpeciesList->item(n);

        if (item->isSelected())
            selection.append(VariantPointer<Species>(item->data(Qt::UserRole)));
    }

    return selection;
}
