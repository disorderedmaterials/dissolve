// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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

    ListWidgetUpdater<SelectSpeciesWidget, Species> speciesUpdater(ui_.SpeciesList, coreData_->species());
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
    for (auto n = 0; n < ui_.SpeciesList->count(); ++n)
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
    for (auto n = 0; n < ui_.SpeciesList->count(); ++n)
    {
        QListWidgetItem *item = ui_.SpeciesList->item(n);

        if (item->isSelected())
            selection.append(VariantPointer<Species>(item->data(Qt::UserRole)));
    }

    return selection;
}
