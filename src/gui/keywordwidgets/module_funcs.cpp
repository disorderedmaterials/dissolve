// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/helpers/comboboxupdater.h"
#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/keywordwidgets/module.h"

ModuleKeywordWidget::ModuleKeywordWidget(QWidget *parent, ModuleKeywordBase *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = true;

    updateValue();

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    ui_.ModuleCombo->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.ModuleCombo));

    refreshing_ = false;
}

/*
 * Widgets
 */

// Value changed
void ModuleKeywordWidget::on_ModuleCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    // Get data from the selected item
    Module *module = VariantPointer<Module>(ui_.ModuleCombo->itemData(index, Qt::UserRole));
    keyword_->setData(module);

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Return text (for ComboBox item) for supplied Module
QString ModuleKeywordWidget::uniqueNameOfModule(const Module *module)
{
    if (module)
        return QString::fromStdString(std::string(module->uniqueName()));
    else
        return "NULL";
}

// Update value displayed in widget
void ModuleKeywordWidget::updateValue()
{
    refreshing_ = true;

    // Get the list of available modules of the specified type
    RefList<Module> availableModules = coreData_.findModules(keyword_->moduleType());
    ComboBoxTextUpdater<ModuleKeywordWidget, Module> comboUpdater(ui_.ModuleCombo, availableModules, keyword_->module(), this,
                                                                  &ModuleKeywordWidget::uniqueNameOfModule);

    refreshing_ = false;
}
