/*
    *** SpeciesTab Functions - Isotopologues
    *** src/gui/speciestab_isotopologues.cpp
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

#include "classes/atomtype.h"
#include "gui/delegates/isotopecombo.hui"
#include "gui/gui.h"
#include "gui/helpers/treewidgetupdater.h"
#include "gui/speciestab.h"
#include <memory>

Q_DECLARE_METATYPE(std::shared_ptr<AtomType>)

/*
 * Private Functions
 */

// IsotopologuesTree top-level update function
void SpeciesTab::updateIsotopologuesTreeTopLevelItem(QTreeWidget *treeWidget, int topLevelItemIndex, Isotopologue *data,
                                                     bool createItem)
{
    QTreeWidgetItem *item;
    if (createItem)
    {
        item = new QTreeWidgetItem;
        item->setData(0, Qt::UserRole, VariantPointer<Isotopologue>(data));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
        treeWidget->insertTopLevelItem(topLevelItemIndex, item);
    }
    else
        item = treeWidget->topLevelItem(topLevelItemIndex);

    // Set item data
    item->setText(0, QString::fromStdString(std::string(data->name())));

    // Update child items
    TreeWidgetRefDataListUpdater<SpeciesTab, std::shared_ptr<AtomType>, Isotope *> isotopeUpdater(
        item, data->isotopes(), this, &SpeciesTab::updateIsotopologuesTreeChildItem);
}

// IsotopologuesTree item update function
void SpeciesTab::updateIsotopologuesTreeChildItem(QTreeWidgetItem *parentItem, int childIndex,
                                                  std::shared_ptr<AtomType> atomType, Isotope *isotope, bool createItem)
{
    QTreeWidgetItem *item;

    // AtomType
    if (createItem)
    {
        item = new QTreeWidgetItem;
        item->setData(1, Qt::UserRole, QVariant::fromValue(atomType));
        item->setData(2, Qt::UserRole, VariantPointer<Isotope>(isotope));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
        parentItem->insertChild(childIndex, item);
    }
    else
        item = parentItem->child(childIndex);
    item->setText(1, QString::fromStdString(std::string(atomType->name())));
    item->setText(2, IsotopeComboDelegate::textForIsotope(isotope));
}

// Return currently-selected Isotopologue
Isotopologue *SpeciesTab::currentIsotopologue()
{
    // Get current item from tree, and check the parent item
    QTreeWidgetItem *item = ui_.IsotopologuesTree->currentItem();
    if (!item)
        return nullptr;
    if (item->parent())
        return VariantPointer<Isotopologue>(item->parent()->data(0, Qt::UserRole));
    else
        return VariantPointer<Isotopologue>(item->data(0, Qt::UserRole));
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

void SpeciesTab::on_IsotopologuesTree_itemChanged(QTreeWidgetItem *item, int column)
{
    if (refreshLock_.isLocked() || (!item))
        return;

    Isotopologue *isotopologue = currentIsotopologue();

    // If a top-level item, then the only possibility is to edit the isotopologue name (column 0)
    if (item->parent() == nullptr)
    {
        // Name of the isotopologue
        if (column == 0)
        {
            // Name of the Isotopologue has been changed - make sure it doesn't exist in the Species already
            isotopologue->setName(species_->uniqueIsotopologueName(qPrintable(item->text(0)), isotopologue));

            // Update the item text (we may have modified the name to avoid a clash)
            Locker refreshLocker(refreshLock_);
            item->setText(0, QString::fromStdString(std::string(isotopologue->name())));
            refreshLocker.unlock();

            dissolveWindow_->setModified();
        }
    }
    else if (column == 1)
    {
        // AtomType - no action to perform (not editable)
    }
    else if (column == 2)
    {
        // Set neutron isotope - need to get AtomType from column 1...
        std::shared_ptr<AtomType> atomType = item->data(1, Qt::UserRole).value<std::shared_ptr<AtomType>>();
        Isotope *isotope = VariantPointer<Isotope>(item->data(2, Qt::UserRole));
        if (isotope)
            isotopologue->setAtomTypeIsotope(atomType, isotope);
        dissolveWindow_->setModified();
    }
}

/*
 * Public Functions
 */

// Update Isotopologues tab
void SpeciesTab::updateIsotopologuesTab()
{
    Locker refreshLocker(refreshLock_);

    // Isotopologues Tree
    if (!species_)
        ui_.IsotopologuesTree->clear();
    else
    {
        TreeWidgetUpdater<SpeciesTab, Isotopologue> isotopologueUpdater(ui_.IsotopologuesTree, species_->isotopologues(), this,
                                                                        &SpeciesTab::updateIsotopologuesTreeTopLevelItem);

        // If there is no current isotopologue selected, try to select the first
        if (!currentIsotopologue())
            ui_.IsotopologuesTree->setCurrentItem(ui_.IsotopologuesTree->topLevelItem(0));

        ui_.IsotopologuesTree->resizeColumnToContents(0);
    }
    Isotopologue *isotopologue = currentIsotopologue();
    ui_.IsotopologueRemoveButton->setEnabled(isotopologue != nullptr);
}
