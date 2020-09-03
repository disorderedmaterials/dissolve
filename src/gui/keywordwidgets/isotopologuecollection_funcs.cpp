/*
    *** Keyword Widget - IsotopologueCollection
    *** src/gui/keywordwidgets/isotopologuecollection_funcs.cpp
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

#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "genericitems/listhelper.h"
#include "gui/delegates/combolist.hui"
#include "gui/delegates/customcombodelegate.h"
#include "gui/delegates/exponentialspin.hui"
#include "gui/delegates/usedspeciescombo.hui"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/isotopologuecollection.h"
#include "module/module.h"

IsotopologueCollectionKeywordWidget::IsotopologueCollectionKeywordWidget(QWidget *parent, KeywordBase *keyword,
                                                                         const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData), isotopologueSetsItemManager_(this), isotopologuesItemManager_(this),
      isotopologueWeightItemManager_(this)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Set delegates for table
    ui_.IsotopologueTree->setItemDelegateForColumn(2,
                                                   new CustomComboDelegate<IsotopologueCollectionKeywordWidget>(
                                                       this, &IsotopologueCollectionKeywordWidget::availableIsotopologueNames));
    ui_.IsotopologueTree->setItemDelegateForColumn(3, new ExponentialSpinDelegate(this));

    // Connect signals / slots
    connect(ui_.AutoButton, SIGNAL(clicked(bool)), this, SLOT(autoButton_clicked(bool)));
    connect(ui_.AddButton, SIGNAL(clicked(bool)), this, SLOT(addButton_clicked(bool)));
    connect(ui_.RemoveButton, SIGNAL(clicked(bool)), this, SLOT(removeButton_clicked(bool)));
    connect(ui_.IsotopologueTree, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this,
            SLOT(isotopologueTree_itemChanged(QTreeWidgetItem *, int)));
    connect(ui_.IsotopologueTree, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this,
            SLOT(isotopologueTree_currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<IsotopologueCollectionKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into IsotopologueCollectionKeyword.\n", keyword->name());
    else
    {
        // Set current information
        updateWidgetValues(coreData_);
    }

    // Summary text on KeywordDropDown button
    setSummaryText("Edit...");
}

/*
 * Widgets
 */

// Return valid Isotopologue names for specified model index
std::vector<std::string> IsotopologueCollectionKeywordWidget::availableIsotopologueNames(const QModelIndex &index)
{
    // We are expecting to be given a QModelIndex which locates an item representing a IsotopologueWeight
    // The column of the provided child model index must therefore be '2'
    if (index.column() != 2)
    {
        // TODO Raise Exception
        Messenger::error("IsotopologueCollectionKeywordWidget::availableIsotopologueNames() - Not a suitable column (!= 2).\n");
        return std::vector<std::string>();
    }

    // Get the parent and grandparent model indices to get the positions (rows) of the Isopologues and IsotopologueSets
    const auto topesIndex = index.parent();
    const auto setIndex = topesIndex.parent();
    if (!topesIndex.isValid() || !setIndex.isValid())
    {
        // TODO Raise Exception
        Messenger::error("IsotopologueCollectionKeywordWidget::availableIsotopologueNames() - Parent (Isotopologues) and/or "
                         "grandparent (IsotopologueSet) not valid.\n");
        return std::vector<std::string>();
    }

    auto &isotopologueSet = keyword_->data().isotopologueSets()[setIndex.row()];
    auto &isotopologues = isotopologueSet.isotopologues()[topesIndex.row()];

    // Construct valid names list
    std::vector<std::string> validNames = {"Natural"};
    ListIterator<Isotopologue> topeIterator(isotopologues.species()->isotopologues());
    while (Isotopologue *tope = topeIterator.iterate())
        validNames.push_back(std::string(tope->name()));

    return validNames;
}

void IsotopologueCollectionKeywordWidget::autoButton_clicked(bool checked)
{
    // Complete the set
    keyword_->data().complete(keyword_->allowedConfigurations());

    // Manually flag that the keyword data has changed
    keyword_->hasBeenSet();

    updateWidgetValues(coreData_);

    emit(keywordValueChanged(keyword_->optionMask()));
}

void IsotopologueCollectionKeywordWidget::addButton_clicked(bool checked)
{
    QTreeWidgetItem *item = ui_.IsotopologueTree->currentItem();

    // Determine what kind of data is selected
    if (!item)
    {
        // No item selected - add next missing configuration
        for (Configuration *cfg : keyword_->allowedConfigurations())
        {
            if (keyword_->data().contains(cfg))
                continue;
            if (cfg->usedSpecies().nItems() == 0)
                continue;

            keyword_->data().add(cfg, cfg->usedSpecies().first()->species()->naturalIsotopologue(), 1.0);

            break;
        }
    }
    else if (isotopologueSetsItemManager_.isMapped(item))
    {
        // IsotopologueSet (Configuration), so add next Species
        auto data = isotopologueSetsItemManager_.reference(item);
        if (!data)
        {
            // TODO Raise Exception
            Messenger::error("Reference for IsotopologueSet not in map.\n");
            return;
        }
        IsotopologueSet &set = *data;

        ListIterator<SpeciesInfo> speciesIterator(set.configuration()->usedSpecies());
        while (SpeciesInfo *spInfo = speciesIterator.iterate())
        {
            if (!set.contains(spInfo->species()))
            {
                set.add(spInfo->species()->naturalIsotopologue(), 1.0);
                break;
            }
        }
    }
    else if (isotopologuesItemManager_.isMapped(item))
    {
        // Get IsotopologueSet reference
        auto setData = isotopologueSetsItemManager_.reference(item->parent());
        if (!setData)
        {
            // TODO Raise Exception
            Messenger::error("Reference for IsotopologueSet not in map.\n");
            return;
        }
        IsotopologueSet &set = *setData;

        // Get Isotopologues reference
        auto topesData = isotopologuesItemManager_.reference(item);
        if (!topesData)
        {
            // TODO Raise Exception
            Messenger::error("Reference for Isotopologues not in map.\n");
            return;
        }
        Isotopologues &topes = *topesData;

        auto *sp = topes.species();

        // Natural first
        if (!topes.contains(sp->naturalIsotopologue()))
            set.add(sp->naturalIsotopologue(), 1.0);
        else
        {
            ListIterator<Isotopologue> topeIterator(sp->isotopologues());
            while (auto *tope = topeIterator.iterate())
            {
                if (!topes.contains(tope))
                {
                    set.add(tope, 1.0);
                    break;
                }
            }
        }
    }
    else if (isotopologueWeightItemManager_.isMapped(item))
    {
        // Get toplevel item parent (two levels up) so we have access to the IsotopologueSet
        QTreeWidgetItem *topLevelItem = item->parent();
        while (topLevelItem->parent())
            topLevelItem = topLevelItem->parent();
        auto setData = isotopologueSetsItemManager_.reference(topLevelItem);
        if (!setData)
        {
            // TODO Raise Exception
            Messenger::error("Reference for IsotopologueSet not in map.\n");
            return;
        }
        IsotopologueSet &set = *setData;

        // Get IsotopologueWeight reference
        auto weightData = isotopologueWeightItemManager_.reference(item);
        if (!weightData)
        {
            // TODO Raise Exception
            Messenger::error("Reference for IsotopologueWeight not in map.\n");
            return;
        }
        IsotopologueWeight &topeWeight = *weightData;

        // Grab relevant Species
        auto *sp = topeWeight.isotopologue()->parent();

        // Get available Isotopologues
        auto data = set.getIsotopologues(sp);
        // TODO Raise exception if isotopologue set not found
        if (!data)
        {
            // TODO Raise Exception
            Messenger::error("IsotopologueSet does not contain Species.\n");
            return;
        }
        const Isotopologues &topes = *data;

        // Natural first
        if (!topes.contains(sp->naturalIsotopologue()))
            set.add(sp->naturalIsotopologue(), 1.0);
        else
        {
            ListIterator<Isotopologue> topeIterator(sp->isotopologues());
            while (Isotopologue *tope = topeIterator.iterate())
            {
                if (!topes.contains(tope))
                {
                    set.add(tope, 1.0);
                    break;
                }
            }
        }
    }

    // Manually flag that the keyword data has changed
    keyword_->hasBeenSet();

    updateWidgetValues(coreData_);

    emit(keywordValueChanged(keyword_->optionMask()));
}

void IsotopologueCollectionKeywordWidget::removeButton_clicked(bool checked)
{
    QTreeWidgetItem *item = ui_.IsotopologueTree->currentItem();
    if (!item)
        return;

    // Determine what kind of data is selected
    if (isotopologueSetsItemManager_.isMapped(item))
    {
        // Get IsotopologueSet reference
        auto setData = isotopologueSetsItemManager_.reference(item);
        if (!setData)
        {
            // TODO Raise Exception
            Messenger::error("Reference for IsotopologueSet not in map.\n");
            return;
        }
        IsotopologueSet &set = *setData;
        keyword_->data().remove(&set);
    }
    else if (isotopologuesItemManager_.isMapped(item))
    {
        // Get IsotopologueSet reference
        auto setData = isotopologueSetsItemManager_.reference(item->parent());
        if (!setData)
        {
            // TODO Raise Exception
            Messenger::error("Reference for IsotopologueSet not in map.\n");
            return;
        }
        IsotopologueSet &set = *setData;

        // Get Isotopologues reference
        auto topesData = isotopologuesItemManager_.reference(item);
        if (!topesData)
        {
            // TODO Raise Exception
            Messenger::error("Reference for Isotopologues not in map.\n");
            return;
        }
        Isotopologues &topes = *topesData;

        keyword_->data().remove(&set, topes.species());
    }
    else if (isotopologueWeightItemManager_.isMapped(item))
    {
        // Get toplevel item parent (two levels up) so we have access to the IsotopologueSet
        QTreeWidgetItem *topLevelItem = item->parent();
        while (topLevelItem->parent())
            topLevelItem = topLevelItem->parent();

        // Get IsotopologueSet reference
        auto setData = isotopologueSetsItemManager_.reference(topLevelItem);
        if (!setData)
        {
            // TODO Raise Exception
            Messenger::error("Reference for IsotopologueSet not in map.\n");
            return;
        }
        IsotopologueSet &set = *setData;

        // Get IsotopologueWeight reference
        auto weightData = isotopologueWeightItemManager_.reference(item);
        if (!weightData)
        {
            // TODO Raise Exception
            Messenger::error("Reference for IsotopologueWeight not in map.\n");
            return;
        }
        IsotopologueWeight &weight = *weightData;

        keyword_->data().remove(&set, &weight);
    }

    // Manually flag that the keyword data has changed
    keyword_->hasBeenSet();

    updateWidgetValues(coreData_);

    emit(keywordValueChanged(keyword_->optionMask()));
}

void IsotopologueCollectionKeywordWidget::isotopologueTree_itemChanged(QTreeWidgetItem *item, int column)
{
    if (refreshing_)
        return;

    // Only editable items are in the IsotopologueWeight, so the item must reflect one
    if (!isotopologueWeightItemManager_.isMapped(item))
    {
        Messenger::error("Item being changed doesn't appear to be an IsotopologueWeight.\n");
        return;
    }

    // Get IsotopologueWeight reference
    auto weightData = isotopologueWeightItemManager_.reference(item);
    if (!weightData)
    {
        // TODO Raise Exception
        Messenger::error("Reference for IsotopologueWeight not in map.\n");
        return;
    }
    IsotopologueWeight &weight = *weightData;

    // Column of passed item tells us the type of data we need to change
    if (column == 2)
    {
        // Editing the Isotopologue - need the parent item in order to validate it
        auto topesData = isotopologuesItemManager_.reference(item->parent());
        if (!topesData)
        {
            // TODO Raise Exception
            Messenger::error("Reference for Isotopologues not in map.\n");
            return;
        }
        Isotopologues &topes = *topesData;
        auto *iso = topes.species()->findIsotopologue(qPrintable(item->text(column)));
        if (!iso)
            return;

        // Set the new Isotopologue
        weight.setIsotopologue(iso);

        // Manually flag that the keyword data has changed
        keyword_->hasBeenSet();

        emit(keywordValueChanged(keyword_->optionMask()));
    }
    else if (column == 3)
    {
        weight.setWeight(item->text(column).toDouble());

        // Manually flag that the keyword data has changed
        keyword_->hasBeenSet();

        emit(keywordValueChanged(keyword_->optionMask()));
    }
    else
        Messenger::error("Don't know what to do with data from column {} of Isotopologue table.\n", column);
}

void IsotopologueCollectionKeywordWidget::isotopologueTree_currentItemChanged(QTreeWidgetItem *currentItem,
                                                                              QTreeWidgetItem *previousItem)
{
    ui_.RemoveButton->setEnabled(currentItem);
}

/*
 * Update
 */

// IsotopologueTree parent (IsotopologueSet) item update function
void IsotopologueCollectionKeywordWidget::updateIsotopologueTreeRootItem(QTreeWidgetItem *item, IsotopologueSet &topeSet,
                                                                         bool itemIsNew)
{
    if (itemIsNew)
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    // Set item data
    item->setText(0, QString::fromStdString(std::string(topeSet.configuration()->name())));

    // Update child (Isotopologues) items
    isotopologuesItemManager_.updateChildren(item, topeSet.isotopologues(),
                                             &IsotopologueCollectionKeywordWidget::updateIsotopologueTreeChildItem);
}

// IsotopologueTree child (Isotopologues) update function
void IsotopologueCollectionKeywordWidget::updateIsotopologueTreeChildItem(QTreeWidgetItem *item, Isotopologues &topes,
                                                                          bool itemIsNew)
{
    if (itemIsNew)
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    // Set item data
    item->setText(1, QString::fromStdString(std::string(topes.species()->name())));

    // Update child (IsotopologueWeight) items
    isotopologueWeightItemManager_.updateChildren(item, topes.mix(),
                                                  &IsotopologueCollectionKeywordWidget::updateIsotopologueTreeSubChildItem);
}

// IsotopologueTree sub-child (IsotopologueWeight) update function
void IsotopologueCollectionKeywordWidget::updateIsotopologueTreeSubChildItem(QTreeWidgetItem *item,
                                                                             IsotopologueWeight &isoWeight, bool itemIsNew)
{
    if (itemIsNew)
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);

    item->setText(2, QString::fromStdString(std::string(isoWeight.isotopologue()->name())));
    item->setText(3, QString::number(isoWeight.weight()));
}

// Update value displayed in widget
void IsotopologueCollectionKeywordWidget::updateValue() { updateWidgetValues(coreData_); }

// Update widget values data based on keyword data
void IsotopologueCollectionKeywordWidget::updateWidgetValues(const CoreData &coreData)
{
    refreshing_ = true;

    // Update the root items of the tree with the allowed Configurations list
    isotopologueSetsItemManager_.update(ui_.IsotopologueTree, keyword_->data().isotopologueSets(),
                                        &IsotopologueCollectionKeywordWidget::updateIsotopologueTreeRootItem);

    for (int n = 0; n < 4; ++n)
        ui_.IsotopologueTree->resizeColumnToContents(n);
    ui_.IsotopologueTree->expandAll();

    ui_.RemoveButton->setEnabled(ui_.IsotopologueTree->currentItem());

    refreshing_ = false;
}

// Update keyword data based on widget values
void IsotopologueCollectionKeywordWidget::updateKeywordData()
{
    // Not used
}
