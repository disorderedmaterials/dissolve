// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/coreData.h"
#include "gui/keywordWidgets/dropDown.h"
#include "gui/keywordWidgets/moduleVector.h"
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

    // Set up the model
    ui_.ModuleList->setModel(&moduleModel_);
    moduleModel_.setCheckStateData(keyword_->data());
    resetModelData();

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

// Reset model data
void ModuleVectorKeywordWidget::resetModelData()
{
    refreshing_ = true;

    // Update allowed modules
    allowedModules_ = Module::allOfType(keyword_->moduleTypes());
    moduleModel_.setData(allowedModules_);

    updateSummaryText();

    refreshing_ = false;
}

// Update value displayed in widget
void ModuleVectorKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    if (mutationFlags.isSet(DissolveSignals::ModulesMutated))
        resetModelData();
}

// Update summary text
void ModuleVectorKeywordWidget::updateSummaryText()
{
    if (keyword_->data().empty())
        setSummaryText("<None>");
    else
        setSummaryText(
            QString::fromStdString(joinStrings(keyword_->data(), ", ", [](const auto &module) { return module->name(); })));
}
