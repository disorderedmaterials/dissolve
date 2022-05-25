// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/coredata.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/modulevector.h"
#include "module/module.h"
#include "templates/algorithms.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QString>

ModuleVectorKeywordWidget::ModuleVectorKeywordWidget(QWidget *parent, ModuleVectorKeyword *keyword, const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    moduleModel_.setCheckStateData(keyword_->data());
    ui_.ModuleList->setModel(&moduleModel_);
    updateAllowedModules();

    // Connect signals / slots
    connect(&moduleModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));

    updateSummaryText();
}

/*
 * Widgets
 */

// List item changed
void ModuleVectorKeywordWidget::modelDataChanged(const QModelIndex &, const QModelIndex &)
{
    if (refreshing_)
        return;

    updateSummaryText();

    emit(keywordDataChanged(keyword_->editSignals()));
}

/*
 * Update
 */

// Check / update allowed modules and displayed data
void ModuleVectorKeywordWidget::updateAllowedModules()
{
    refreshing_ = true;

    // Update allowed modules
    allowedModules_ = Module::allOfType(keyword_->moduleTypes());
    moduleModel_.setData(allowedModules_);

    refreshing_ = false;
}

// Update value displayed in widget
void ModuleVectorKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    if (mutationFlags.isSet(DissolveSignals::ModulesMutated))
    {
        updateAllowedModules();
        updateSummaryText();
    }
}

// Update widget values data based on keyword data
void ModuleVectorKeywordWidget::updateWidgetValues(const CoreData &coreData) {}

// Update keyword data based on widget values
void ModuleVectorKeywordWidget::updateKeywordData() {}

// Update summary text
void ModuleVectorKeywordWidget::updateSummaryText()
{
    if (keyword_->data().empty())
        setSummaryText("<None>");
    else
        setSummaryText(
            QString::fromStdString(joinStrings(keyword_->data(), ", ", [](const auto &module) { return module->name(); })));
}
