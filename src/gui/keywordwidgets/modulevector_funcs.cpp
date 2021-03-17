// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/coredata.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/modulevector.h"
#include "module/module.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QString>

ModuleVectorKeywordWidget::ModuleVectorKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Connect signals / slots
    connect(ui_.SelectionList, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(itemChanged(QListWidgetItem *)));

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<ModuleVectorKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword into ModuleVectorKeyword.\n");
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
void ModuleVectorKeywordWidget::updateSelectionRow(int row, Module *module, bool createItem)
{
    // Grab the target reference list
    auto &selection = keyword_->data();

    QListWidgetItem *item;
    if (createItem)
    {
        item = new QListWidgetItem(QString::fromStdString(std::string(module->uniqueName())));
        item->setData(Qt::UserRole, VariantPointer<Module>(module));
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        ui_.SelectionList->insertItem(row, item);
    }
    else
        item = ui_.SelectionList->item(row);
    item->setCheckState(std::find(selection.begin(), selection.end(), module) != selection.end() ? Qt::Checked : Qt::Unchecked);
}

// List item changed
void ModuleVectorKeywordWidget::itemChanged(QListWidgetItem *item)
{
    if (refreshing_)
        return;

    updateKeywordData();

    updateSummaryText();

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void ModuleVectorKeywordWidget::updateValue() { updateWidgetValues(coreData_); }

// Update widget values data based on keyword data
void ModuleVectorKeywordWidget::updateWidgetValues(const CoreData &coreData)
{
    refreshing_ = true;

    // Get a list of current Modules that are of the correct type
    RefList<Module> availableModules = coreData.findModules(keyword_->moduleTypes());

    // Update the list widget
    ListWidgetUpdater<ModuleVectorKeywordWidget, Module> listUpdater(ui_.SelectionList, availableModules, this,
                                                                     &ModuleVectorKeywordWidget::updateSelectionRow);

    updateSummaryText();

    refreshing_ = false;
}

// Update keyword data based on widget values
void ModuleVectorKeywordWidget::updateKeywordData()
{
    // Loop over items in the QListWidget, adding the associated Modules for any that are checked
    std::vector<Module *> newSelection;
    for (auto n = 0; n < ui_.SelectionList->count(); ++n)
    {
        QListWidgetItem *item = ui_.SelectionList->item(n);
        if (item->checkState() == Qt::Checked)
            newSelection.emplace_back(VariantPointer<Module>(item->data(Qt::UserRole)));
    }

    // Update the data
    keyword_->setData(newSelection);
}

// Update summary text
void ModuleVectorKeywordWidget::updateSummaryText()
{
    // Create summary text for the KeywordDropDown button
    auto &selection = keyword_->data();
    if (selection.empty())
        setSummaryText("<None>");
    else
    {
        QString summaryText;
        auto first = true;
        for (Module *module : selection)
        {
            if (!first)
                summaryText += ", ";
            summaryText += QString::fromStdString(std::string(module->uniqueName()));
            first = false;
        }
        setSummaryText(summaryText);
    }
}
