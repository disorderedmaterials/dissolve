// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/coreData.h"
#include "gui/delegates/exponentialSpin.hui"
#include "gui/keywordWidgets/dropDown.h"
#include "gui/keywordWidgets/weightedModuleVector.h"
#include "module/module.h"
#include "templates/algorithms.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QString>

WeightedModuleVectorKeywordWidget::WeightedModuleVectorKeywordWidget(QWidget *parent, WeightedModuleVectorKeyword *keyword,
                                                                     const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Set up the model
    ui_.ModuleColumns->setModel(&weightedModuleModel_);
    weightedModuleModel_.setWeightedItems(keyword_->data());
    resetModelData();

    // Set delegates
    ui_.ModuleColumns->setItemDelegateForColumn(1, new ExponentialSpinDelegate(this));

    // Connect signals / slots
    connect(&weightedModuleModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));

    updateSummaryText();
}

/*
 * Widgets
 */

// List item changed
void WeightedModuleVectorKeywordWidget::modelDataChanged(const QModelIndex &, const QModelIndex &)
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
void WeightedModuleVectorKeywordWidget::resetModelData()
{
    refreshing_ = true;

    // Update allowed modules
    allowedModules_ = Module::allOfType(keyword_->moduleTypes());
    weightedModuleModel_.setData(allowedModules_);

    updateSummaryText();

    refreshing_ = false;
}

// Update value displayed in widget
void WeightedModuleVectorKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    if (mutationFlags.isSet(DissolveSignals::ModulesMutated))
        resetModelData();
}

// Update summary text
void WeightedModuleVectorKeywordWidget::updateSummaryText()
{
    if (keyword_->data().empty())
        setSummaryText("<None>");
    else
        setSummaryText(QString::fromStdString(joinStrings(
            keyword_->data(), ", ", [](const auto &item) { return fmt::format("{} ({})", item.first->name(), item.second); })));
}
