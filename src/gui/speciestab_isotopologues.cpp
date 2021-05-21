// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
    // QTreeWidgetItem *item = ui_.IsotopologuesTree->currentItem();
    // if (!item)
    return nullptr;
    // if (item->parent())
    //     return VariantPointer<Isotopologue>(item->parent()->data(0, Qt::UserRole));
    // else
    //     return VariantPointer<Isotopologue>(item->data(0, Qt::UserRole));
}

/*
 * Private Slots
 */

void SpeciesTab::on_IsotopologueAddButton_clicked(bool checked)
{
    species_->addIsotopologue("NewIsotopologue");

    dissolveWindow_->setModified();

    updateControls();
}

void SpeciesTab::on_IsotopologueRemoveButton_clicked(bool checked)
{
    // Get current Isotopologue
    Isotopologue *iso = currentIsotopologue();
    if (!iso)
        return;

    // Notify all keywords that our Isotopologue is about to be removed
    KeywordBase::objectNoLongerValid<Isotopologue>(iso);

    // Finally, remove the Isotopologue from the Species
    species_->removeIsotopologue(iso);

    dissolveWindow_->setModified();

    updateControls();
}

void SpeciesTab::on_IsotopologueGenerateButton_clicked(bool checked)
{
    // TODO
}

void SpeciesTab::on_IsotopologueExpandAllButton_clicked(bool checked) { ui_.IsotopologuesTree->expandAll(); }

void SpeciesTab::on_IsotopologueCollapseAllButton_clicked(bool checked) { ui_.IsotopologuesTree->collapseAll(); }

// void SpeciesTab::on_IsotopologuesTree_itemChanged(QTreeWidgetItem *item, int column)
// {
//     if (refreshLock_.isLocked() || (!item))
//         return;

//     Isotopologue *isotopologue = currentIsotopologue();

//     // If a top-level item, then the only possibility is to edit the isotopologue name (column 0)
//     if (item->parent() == nullptr)
//     {
//         // Name of the isotopologue
//         if (column == 0)
//         {
//             // Name of the Isotopologue has been changed - make sure it doesn't exist in the Species already
//             isotopologue->setName(species_->uniqueIsotopologueName(qPrintable(item->text(0)), isotopologue));

//             // Update the item text (we may have modified the name to avoid a clash)
//             Locker refreshLocker(refreshLock_);
//             item->setText(0, QString::fromStdString(std::string(isotopologue->name())));
//             refreshLocker.unlock();

//             dissolveWindow_->setModified();
//         }
//     }
//     else if (column == 1)
//     {
//         // AtomType - no action to perform (not editable)
//     }
//     else if (column == 2)
//     {
//         // Set neutron isotope - need to get AtomType from column 1...
//         auto atomType = item->data(1, Qt::UserRole).value<std::shared_ptr<AtomType>>();
//         // ...and Isotope from column 2
//         auto isotope = item->data(2, Qt::UserRole).value<Sears91::Isotope>();
//         isotopologue->setAtomTypeIsotope(atomType, isotope);
//         dissolveWindow_->setModified();
//     }
// }

/*
 * Public Functions
 */

// Update Isotopologues tab
void SpeciesTab::updateIsotopologuesTab()
{
    Locker refreshLocker(refreshLock_);

    // Isotopologues Tree
    // if (!species_)
    //     ui_.IsotopologuesTree->clear();
    // else
    // {
    //     TreeWidgetUpdater<SpeciesTab, Isotopologue> isotopologueUpdater(ui_.IsotopologuesTree, species_->isotopologues(),
    //     this,
    //                                                                     &SpeciesTab::updateIsotopologuesTreeTopLevelItem);

    //     // If there is no current isotopologue selected, try to select the first
    //     if (!currentIsotopologue())
    //         ui_.IsotopologuesTree->setCurrentItem(ui_.IsotopologuesTree->topLevelItem(0));

    //     ui_.IsotopologuesTree->resizeColumnToContents(0);
    // }
    Isotopologue *isotopologue = currentIsotopologue();
    ui_.IsotopologueRemoveButton->setEnabled(isotopologue != nullptr);
}
