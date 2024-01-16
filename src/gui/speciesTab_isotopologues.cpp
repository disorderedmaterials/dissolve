// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/gui.h"
#include "gui/speciesTab.h"

/*
 * Private Slots
 */

void SpeciesTab::isotopologuesSelectionChanged(const QItemSelection &, const QItemSelection &) { updateIsotopologuesTab(); }

void SpeciesTab::isotopologuesChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)
{
    updateIsotopologuesTab();

    dissolveWindow_->setModified(DissolveSignals::DataMutations::IsotopologuesMutated);
}

void SpeciesTab::on_IsotopologuesTree_customContextMenuRequested(const QPoint &pos)
{
    auto index = ui_.IsotopologuesTree->indexAt(pos);
    if (!index.isValid() || index.parent().isValid())
        return;
    auto iso = isos_.data(index, Qt::UserRole).value<Isotopologue *>();
    assert(iso);

    QMenu menu;
    menu.setFont(font());
    menu.setEnabled(!dissolveWindow_->dissolveIterating());

    // Construct the context menu
    auto *setHydrogensMenu = menu.addMenu("Set hydrogens to...");
    setHydrogensMenu->setFont(font());
    auto *setHNatural = setHydrogensMenu->addAction("Natural abundance");
    auto *setHProtiated = setHydrogensMenu->addAction("Protiated");
    auto *setHDeuteriated = setHydrogensMenu->addAction("Deuteriated");
    menu.addSeparator();
    auto *duplicate = menu.addAction("Duplicate");
    duplicate->setIcon(QIcon(":/general/icons/copy.svg"));

    auto *action = menu.exec(ui_.IsotopologuesTree->mapToGlobal(pos));
    if (action == setHNatural || action == setHProtiated || action == setHDeuteriated)
    {
        auto changesMade = false;
        auto targetTope = action == setHNatural ? Sears91::H_Natural : (action == setHProtiated ? Sears91::H_1 : Sears91::H_2);
        for (auto &[atomType, tope] : iso->isotopes())
            if (atomType->Z() == Elements::H && tope != targetTope)
            {
                tope = targetTope;
                changesMade = true;
            }

        if (changesMade)
        {
            updateIsotopologuesTab();

            dissolveWindow_->setModified(DissolveSignals::DataMutations::IsotopologuesMutated);
        }
    }
    else if (action == duplicate)
    {
        auto newIndex = isos_.addIso();
        ui_.IsotopologuesTree->expand(newIndex);

        // Set a unique name for the new isotopologue
        auto newIso = isos_.data(newIndex, Qt::UserRole).value<Isotopologue *>();
        isos_.setData(newIndex,
                      QString::fromStdString(DissolveSys::uniqueName(
                          iso->name(), species_->isotopologues(),
                          [newIso](const auto &oldIso) { return newIso == oldIso.get() ? std::string() : oldIso->name(); })));

        auto row = 0;
        for (const auto &[atomType, tope] : iso->isotopes())
            isos_.setData(isos_.index(row++, 2, newIndex), QVariant::fromValue<Sears91::Isotope>(tope));
    }
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
    dissolveWindow_->setModified(DissolveSignals::DataMutations::IsotopologuesMutated);

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
