// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "gui/delegates/isotopecombo.hui"
#include "gui/gui.h"
#include "gui/helpers/treewidgetupdater.h"
#include "gui/speciestab.h"
#include <memory>

Q_DECLARE_METATYPE(std::shared_ptr<AtomType>)
Q_DECLARE_METATYPE(Sears91::Isotope)

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
    if (item.parent().isValid())
        return VariantPointer<Isotopologue>(isos_.data(item.parent(), Qt::UserRole));
    else
        return VariantPointer<Isotopologue>(isos_.data(item, Qt::UserRole));
}

/*
 * Private Slots
 */

void SpeciesTab::on_IsotopologueRemoveButton_clicked(bool checked)
{
    // Get current Isotopologue
    Isotopologue *iso = currentIsotopologue();
    if (!iso)
        return;

    // Notify all keywords that our Isotopologue is about to be removed
    KeywordStore::objectNoLongerValid<Isotopologue>(iso);

    // Finally, remove the Isotopologue from the Species
    isos_.removeIso(iso);

    updateControls();
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
    Isotopologue *isotopologue = currentIsotopologue();
    ui_.IsotopologueRemoveButton->setEnabled(isotopologue != nullptr);
}
