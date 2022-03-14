// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/keywordwidgets/module.h"

ModuleKeywordWidget::ModuleKeywordWidget(QWidget *parent, ModuleKeywordBase *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = false;

    ui_.ModuleCombo->setModel(&moduleModel_);
    updateAllowedModules();

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    ui_.ModuleCombo->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.ModuleCombo));
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
    if (index == -1)
        keyword_->setData(nullptr);
    else
        keyword_->setData(ui_.ModuleCombo->currentData(Qt::UserRole).value<Module *>());

    emit(keywordDataChanged(keyword_->editSignals()));
}

/*
 * Update
 */

// Check / update allowed modules and displayed data
void ModuleKeywordWidget::updateAllowedModules()
{
    refreshing_ = true;

    // Update allowed modules
    allowedModules_ = Module::allOfType(keyword_->moduleType());
    moduleModel_.setData(allowedModules_);

    // Set current index based on keyword data
    auto it = std::find(allowedModules_.begin(), allowedModules_.end(), keyword_->module());
    ui_.ModuleCombo->setCurrentIndex(it == allowedModules_.end() ? -1 : it - allowedModules_.begin());

    refreshing_ = false;
}

// Update value displayed in widget
void ModuleKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    if (mutationFlags.isSet(DissolveSignals::ModulesMutated))
        updateAllowedModules();
}
