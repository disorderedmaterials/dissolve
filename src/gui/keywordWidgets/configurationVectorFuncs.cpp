// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/configuration.h"
#include "classes/coreData.h"
#include "gui/keywordWidgets/configurationVector.h"
#include "module/module.h"
#include "templates/algorithms.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QString>

ConfigurationVectorKeywordWidget::ConfigurationVectorKeywordWidget(QWidget *parent, ConfigurationVectorKeyword *keyword,
                                                                   const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Set up the model
    ui_.ConfigurationList->setModel(&configurationModel_);
    configurationModel_.setCheckStateData(keyword_->data());
    resetModelData();

    // Connect signals / slots
    connect(&configurationModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));
}

/*
 * Widgets
 */

void ConfigurationVectorKeywordWidget::modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
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
void ConfigurationVectorKeywordWidget::resetModelData()
{
    refreshing_ = true;

    configurationModel_.setData(coreData_.configurations());

    updateSummaryText();

    refreshing_ = false;
}

// Update value displayed in widget
void ConfigurationVectorKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    resetModelData();
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
