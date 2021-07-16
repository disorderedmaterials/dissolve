// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/configuration.h"
#include "classes/coredata.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/keywordwidgets/configurationvector.h"
#include "module/module.h"
#include "templates/algorithms.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QString>

ConfigurationVectorKeywordWidget::ConfigurationVectorKeywordWidget(QWidget *parent, KeywordBase *keyword,
                                                                   const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Connect signals / slots
    connect(ui_.SelectionList, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(itemChanged(QListWidgetItem *)));

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<ConfigurationVectorKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '%s' into ConfigurationVectorKeyword.\n", keyword->name());
    else
    {
        // Set current information
        updateWidgetValues(coreData_);
    }
}

/*
 * Widgets
 */

// Selection list update function
void ConfigurationVectorKeywordWidget::updateSelectionRow(int row, Configuration *cfg, bool createItem)
{
    // Grab the target reference list
    auto &selection = keyword_->data();

    QListWidgetItem *item;
    if (createItem)
    {
        item = new QListWidgetItem(QString::fromStdString(std::string(cfg->name())));
        item->setData(Qt::UserRole, VariantPointer<Configuration>(cfg));
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        ui_.SelectionList->insertItem(row, item);
    }
    else
        item = ui_.SelectionList->item(row);
    bool contains = std::find(selection.begin(), selection.end(), cfg) != selection.end();
    item->setCheckState(contains ? Qt::Checked : Qt::Unchecked);
}

// List item changed
void ConfigurationVectorKeywordWidget::itemChanged(QListWidgetItem *item)
{
    if (refreshing_)
        return;

    refreshing_ = true;

    // Ensure that we obey any limit on maximum number of selected items
    auto nChecked = 0;
    for (auto n = 0; n < ui_.SelectionList->count(); ++n)
        if (ui_.SelectionList->item(n)->checkState() == Qt::Checked)
            ++nChecked;
    switch (keyword_->maxListSize())
    {
        case (Module::ZeroTargets):
            if (nChecked != 0)
                for (auto n = 0; n < ui_.SelectionList->count(); ++n)
                    ui_.SelectionList->item(n)->setCheckState(Qt::Unchecked);
            break;
        case (Module::ExactlyOneTarget):
            if (nChecked > 1)
            {
                for (auto n = 0; n < ui_.SelectionList->count(); ++n)
                    if ((ui_.SelectionList->item(n)->checkState() == Qt::Checked) && (ui_.SelectionList->item(n) != item))
                        ui_.SelectionList->item(n)->setCheckState(Qt::Unchecked);
            }
            break;
        case (Module::OneOrMoreTargets):
            // Nothing to do
            break;
        default:
            if (nChecked > keyword_->maxListSize())
            {
                for (auto n = 0; n < ui_.SelectionList->count(); ++n)
                {
                    if ((ui_.SelectionList->item(n)->checkState() == Qt::Checked) && (ui_.SelectionList->item(n) != item))
                    {
                        ui_.SelectionList->item(n)->setCheckState(Qt::Unchecked);
                        --nChecked;
                        if (nChecked == keyword_->maxListSize())
                            break;
                    }
                }
            }
            break;
    }

    updateKeywordData();

    updateSummaryText();

    emit(keywordValueChanged(keyword_->optionMask()));

    refreshing_ = false;
}

/*
 * Update
 */

// Update value displayed in widget
void ConfigurationVectorKeywordWidget::updateValue() { updateWidgetValues(coreData_); }

// Update widget values data based on keyword data
void ConfigurationVectorKeywordWidget::updateWidgetValues(const CoreData &coreData)
{
    refreshing_ = true;

    // Update the list against the global Configuration list
    ListWidgetUpdater<ConfigurationVectorKeywordWidget, Configuration> listUpdater(
        ui_.SelectionList, coreData_.configurations(), this, &ConfigurationVectorKeywordWidget::updateSelectionRow);

    updateSummaryText();

    refreshing_ = false;
}

// Update keyword data based on widget values
void ConfigurationVectorKeywordWidget::updateKeywordData()
{
    // Loop over items in the QListWidget, adding the associated Configurations for any that are checked
    std::vector<Configuration *> newSelection;
    for (auto n = 0; n < ui_.SelectionList->count(); ++n)
    {
        QListWidgetItem *item = ui_.SelectionList->item(n);
        if (item->checkState() == Qt::Checked)
            newSelection.push_back(VariantPointer<Configuration>(item->data(Qt::UserRole)));
    }
    keyword_->setData(newSelection);
}

// Update summary text
void ConfigurationVectorKeywordWidget::updateSummaryText()
{
    if (keyword_->data().empty())
        setSummaryText("<None>");
    else
        setSummaryText(
            QString::fromStdString(joinStrings(keyword_->data(), ", ", [](const auto &cfg) { return cfg->name(); })));
}
