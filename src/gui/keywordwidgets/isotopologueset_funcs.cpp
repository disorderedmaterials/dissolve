// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/coredata.h"
#include "classes/species.h"
#include "gui/delegates/combolist.hui"
#include "gui/delegates/customcombodelegate.h"
#include "gui/delegates/exponentialspin.hui"
#include "gui/delegates/null.h"
#include "gui/delegates/usedspeciescombo.hui"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/isotopologueset.h"
#include "module/module.h"

IsotopologueSetKeywordWidget::IsotopologueSetKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData), isotopologuesItemManager_(this), isotopologueWeightItemManager_(this)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Set delegates for table
    ui_.IsotopologueTree->setItemDelegateForColumn(0, new NullDelegate(this));
    ui_.IsotopologueTree->setItemDelegateForColumn(1, new CustomComboDelegate<IsotopologueSetKeywordWidget>(
                                                          this, &IsotopologueSetKeywordWidget::availableIsotopologueNames));
    ui_.IsotopologueTree->setItemDelegateForColumn(2, new ExponentialSpinDelegate(this, 0.0));

    // Connect signals / slots
    connect(ui_.AddButton, SIGNAL(clicked(bool)), this, SLOT(addButton_clicked(bool)));
    connect(ui_.RemoveButton, SIGNAL(clicked(bool)), this, SLOT(removeButton_clicked(bool)));
    connect(ui_.IsotopologueTree, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this,
            SLOT(isotopologueTree_itemChanged(QTreeWidgetItem *, int)));
    connect(ui_.IsotopologueTree, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)), this,
            SLOT(isotopologueTree_currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<IsotopologueSetKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into IsotopologueSetKeyword.\n", keyword->name());
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
std::vector<std::string> IsotopologueSetKeywordWidget::availableIsotopologueNames(const QModelIndex &index)
{
    // We are expecting to be given a QModelIndex which locates an item representing a Isotopologue
    // The column of the provided child model index must therefore be 1
    if (index.column() != 1)
    {
        // TODO Raise Exception
        Messenger::error("IsotopologueSetKeywordWidget::availableIsotopologueNames() - Not a suitable column (!= 1).\n");
        return std::vector<std::string>();
    }

    // Get the parent and grandparent model indices to get the positions (rows) of the Isopologues and IsotopologueSets
    const auto topesIndex = index.parent();
    if (!topesIndex.isValid())
    {
        // TODO Raise Exception
        Messenger::error("IsotopologueSetKeywordWidget::availableIsotopologueNames() - Parent (Isotopologues) and/or "
                         "grandparent (IsotopologueSet) not valid.\n");
        return std::vector<std::string>();
    }

    auto &isotopologues = keyword_->data().isotopologues()[topesIndex.row()];

    // Construct valid names list
    std::vector<std::string> validNames = {"Natural"};
    ListIterator<Isotopologue> topeIterator(isotopologues.species()->isotopologues());
    while (Isotopologue *tope = topeIterator.iterate())
        validNames.push_back(std::string(tope->name()));

    return validNames;
}

void IsotopologueSetKeywordWidget::addButton_clicked(bool checked)
{
    QTreeWidgetItem *item = ui_.IsotopologueTree->currentItem();

    auto &set = keyword_->data();

    // Determine what kind of data is selected
    if (!item)
    {
        // No item selected - add next missing Species
        for (const auto &sp : coreData_.species())
        {
            if (!set.contains(sp.get()))
            {
                set.add(sp->naturalIsotopologue(), 1.0);
                break;
            }
        }
    }
    else if (isotopologuesItemManager_.isMapped(item))
    {
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

void IsotopologueSetKeywordWidget::removeButton_clicked(bool checked)
{
    QTreeWidgetItem *item = ui_.IsotopologueTree->currentItem();
    if (!item)
        return;

    auto &set = keyword_->data();

    // Determine what kind of data is selected
    if (isotopologuesItemManager_.isMapped(item))
    {
        // Get Isotopologues reference
        auto topesData = isotopologuesItemManager_.reference(item);
        if (!topesData)
        {
            // TODO Raise Exception
            Messenger::error("Reference for Isotopologues not in map.\n");
            return;
        }
        Isotopologues &topes = *topesData;

        set.remove(topes.species());
    }
    else if (isotopologueWeightItemManager_.isMapped(item))
    {
        // Get IsotopologueWeight reference
        auto weightData = isotopologueWeightItemManager_.reference(item);
        if (!weightData)
        {
            // TODO Raise Exception
            Messenger::error("Reference for IsotopologueWeight not in map.\n");
            return;
        }
        IsotopologueWeight &weight = *weightData;

        set.remove(&weight);
    }

    // Manually flag that the keyword data has changed
    keyword_->hasBeenSet();

    updateWidgetValues(coreData_);

    emit(keywordValueChanged(keyword_->optionMask()));
}

void IsotopologueSetKeywordWidget::isotopologueTree_itemChanged(QTreeWidgetItem *item, int column)
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
    if (column == 1)
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
    else if (column == 2)
    {
        weight.setWeight(item->text(column).toDouble());

        // Manually flag that the keyword data has changed
        keyword_->hasBeenSet();

        emit(keywordValueChanged(keyword_->optionMask()));
    }
    else
        Messenger::error("Don't know what to do with data from column {} of Isotopologue table.\n", column);
}

void IsotopologueSetKeywordWidget::isotopologueTree_currentItemChanged(QTreeWidgetItem *currentItem,
                                                                       QTreeWidgetItem *previousItem)
{
    ui_.RemoveButton->setEnabled(currentItem);
}

/*
 * Update
 */

// IsotopologueTree parent (IsotopologueSet) item update function
void IsotopologueSetKeywordWidget::updateIsotopologueTreeRootItem(QTreeWidgetItem *item, Isotopologues &topes, bool itemIsNew)
{
    if (itemIsNew)
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    // Set item data
    item->setText(0, QString::fromStdString(std::string(topes.species()->name())));

    // Update child (IsotopologueWeight) items
    isotopologueWeightItemManager_.updateChildren(item, topes.mix(),
                                                  &IsotopologueSetKeywordWidget::updateIsotopologueTreeChildItem);
}

// IsotopologueTree sub-child (IsotopologueWeight) update function
void IsotopologueSetKeywordWidget::updateIsotopologueTreeChildItem(QTreeWidgetItem *item, IsotopologueWeight &isoWeight,
                                                                   bool itemIsNew)
{
    if (itemIsNew)
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);

    item->setText(1, QString::fromStdString(std::string(isoWeight.isotopologue()->name())));
    item->setText(2, QString::number(isoWeight.weight()));
}

// Update value displayed in widget
void IsotopologueSetKeywordWidget::updateValue() { updateWidgetValues(coreData_); }

// Update widget values data based on keyword data
void IsotopologueSetKeywordWidget::updateWidgetValues(const CoreData &coreData)
{
    refreshing_ = true;

    // Update the root items of the tree with the allowed Configurations list
    isotopologuesItemManager_.update(ui_.IsotopologueTree, keyword_->data().isotopologues(),
                                     &IsotopologueSetKeywordWidget::updateIsotopologueTreeRootItem);

    for (auto n = 0; n < 3; ++n)
        ui_.IsotopologueTree->resizeColumnToContents(n);
    ui_.IsotopologueTree->expandAll();

    ui_.RemoveButton->setEnabled(ui_.IsotopologueTree->currentItem());

    refreshing_ = false;
}

// Update keyword data based on widget values
void IsotopologueSetKeywordWidget::updateKeywordData()
{
    // Not used
}
