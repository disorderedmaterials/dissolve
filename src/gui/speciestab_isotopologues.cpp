// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "gui/gui.h"
#include "gui/speciestab.h"

/*
 * Private Functions
 */

// Return currently-selected Isotopologue
Isotopologue *SpeciesTab::currentIsotopologue()
{
    // Get current item from tree, and check the parent item
    auto model = ui_.IsotopologuesTree->selectionModel();
    if (!model->hasSelection())
        return nullptr;
    auto item = model->currentIndex();
    return isos_.data(item.parent().isValid() ? item.parent() : item, Qt::UserRole).value<Isotopologue *>();
}

/*
 * Private Slots
 */

void SpeciesTab::isotopologuesSelectionChanged(const QItemSelection &, const QItemSelection &) { updateIsotopologuesTab(); }

void SpeciesTab::isotopologuesChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)
{
    updateIsotopologuesTab();

    dissolveWindow_->setModified();
}

void SpeciesTab::on_IsotopologueAddButton_clicked(bool checked)
{
    auto newIndex = isos_.addIso();

    ui_.IsotopologuesTree->expand(newIndex);
}

void SpeciesTab::on_IsotopologueRemoveButton_clicked(bool checked)
{
    // Get current Isotopologue
    auto model = ui_.IsotopologuesTree->selectionModel();
    if (!model->hasSelection())
        return;
    auto item = model->currentIndex();
    auto *iso = isos_.data(item.parent().isValid() ? item.parent() : item, Qt::UserRole).value<Isotopologue *>();
    if (!iso)
        return;

    // Notify all keywords that our Isotopologue is about to be removed
    KeywordStore::objectNoLongerValid<Isotopologue>(iso);

    // Finally, remove the Isotopologue from the Species
    isos_.removeIso(item);

    dissolveWindow_->fullUpdate();
}

void SpeciesTab::on_IsotopologueGenerateButton_clicked(bool checked)
{
    // TODO
}

void SpeciesTab::on_IsotopologueExpandAllButton_clicked(bool checked) { ui_.IsotopologuesTree->expandAll(); }

void SpeciesTab::on_IsotopologueCollapseAllButton_clicked(bool checked) { ui_.IsotopologuesTree->collapseAll(); }

/*
 * Public Functions
 */

// Update Isotopologues tab
void SpeciesTab::updateIsotopologuesTab()
{
    ui_.IsotopologuesTree->resizeColumnToContents(0);
    ui_.IsotopologueRemoveButton->setEnabled(ui_.IsotopologuesTree->selectionModel()->hasSelection());
}
