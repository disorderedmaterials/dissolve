// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/coredata.h"
#include "gui/helpers/tablewidgetupdater.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/modulegroups.h"
#include "module/group.h"
#include "module/groups.h"
#include "module/module.h"
#include "templates/variantpointer.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QString>

ModuleGroupsKeywordWidget::ModuleGroupsKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Connect signals / slots
    connect(ui_.SelectionTable, SIGNAL(itemChanged(QTableWidgetItem *)), this, SLOT(itemChanged(QTableWidgetItem *)));

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<ModuleGroupsKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword into ModuleGroupsKeyword.\n");
    else
    {
        // Set current information
        updateWidgetValues(coreData_);
    }
}

/*
 * Widgets
 */

// Selection table update function
void ModuleGroupsKeywordWidget::updateSelectionRow(int row, const Module *module, bool create)
{
    // Grab the target groups
    auto &groups = keyword_->data();

    QTableWidgetItem *item;

    // Module name
    if (create)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, VariantPointer<Module>(module));
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
        ui_.SelectionTable->setItem(row, 0, item);
    }
    else
        item = ui_.SelectionTable->item(row, 0);
    item->setText(QString::fromStdString(std::string(module->uniqueName())));
    item->setCheckState(groups.contains(module) ? Qt::Checked : Qt::Unchecked);

    // Module group
    if (create)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, VariantPointer<Module>(module));
        ui_.SelectionTable->setItem(row, 1, item);
    }
    else
        item = ui_.SelectionTable->item(row, 1);
    item->setText(QString::fromStdString(std::string(groups.groupName(module))));
}

// Table item changed
void ModuleGroupsKeywordWidget::itemChanged(QTableWidgetItem *item)
{
    if (refreshing_)
        return;

    // Get the Module pointer and check status from column 0 of the current row
    Module *module = VariantPointer<Module>(ui_.SelectionTable->item(item->row(), 0)->data(Qt::UserRole));
    if (!module)
        return;
    auto isSelected = (ui_.SelectionTable->item(item->row(), 0)->checkState() == Qt::Checked);

    // Get group text from column 1 of the current row
    QString groupName = ui_.SelectionTable->item(item->row(), 1)->text();

    // Check the column of the item
    switch (item->column())
    {
        // Module (checked if a target)
        case (0):
            if (isSelected)
                keyword_->data().addModule(module, qPrintable(groupName));
            else
                keyword_->data().removeModule(module);
            keyword_->hasBeenSet();
            break;
        // Group name
        case (1):
            // Take the new name and try to re-add the current Module to the (new) group (only if the Module is
            // selected...)
            if (isSelected)
            {
                keyword_->data().addModule(module, qPrintable(groupName));
                keyword_->hasBeenSet();
            }
            break;
    }

    updateSummaryText();

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void ModuleGroupsKeywordWidget::updateValue() { updateWidgetValues(coreData_); }

// Update widget values data based on keyword data
void ModuleGroupsKeywordWidget::updateWidgetValues(const CoreData &coreData)
{
    refreshing_ = true;

    // Get a RefList of current Modules that are of the correct type
    RefList<Module> availableModules = coreData.findModules(keyword_->data().allowedModuleTypes());

    // Update the list widget
    ConstTableWidgetUpdater<ModuleGroupsKeywordWidget, Module> tableUpdater(ui_.SelectionTable, availableModules, this,
                                                                            &ModuleGroupsKeywordWidget::updateSelectionRow);

    ui_.SelectionTable->resizeColumnToContents(0);

    updateSummaryText();

    refreshing_ = false;
}

// Update keyword data based on widget values
void ModuleGroupsKeywordWidget::updateKeywordData()
{
    // Not used
}

// Update summary text
void ModuleGroupsKeywordWidget::updateSummaryText()
{
    // Create summary text for the KeywordDropDown button
    auto &groups = keyword_->data();
    if (groups.modules().nItems() == 0)
        setSummaryText("<None>");
    else
    {
        QString summaryText;
        ListIterator<ModuleGroup> groupIterator(groups.groups());
        while (ModuleGroup *group = groupIterator.iterate())
        {
            if (!groupIterator.isFirst())
                summaryText += ", ";
            summaryText += QString("%1 (%2)").arg(group->nModules()).arg(QString::fromStdString(std::string(group->name())));
        }
        setSummaryText(summaryText);
    }
}
