// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/coreData.h"
#include "gui/helpers/mouseWheelAdjustmentGuard.h"
#include "gui/keywordWidgets/configuration.h"

ConfigurationKeywordWidget::ConfigurationKeywordWidget(QWidget *parent, ConfigurationKeyword *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = false;

    ui_.ConfigurationCombo->setModel(&configurationModel_);
    resetModelData();

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    ui_.ConfigurationCombo->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.ConfigurationCombo));
}

/*
 * Widgets
 */

void ConfigurationKeywordWidget::on_ConfigurationCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    // Get data from the selected item
    if (index == -1)
        keyword_->data() = nullptr;
    else
        keyword_->data() = ui_.ConfigurationCombo->currentData(Qt::UserRole).value<Configuration *>();

    emit(keywordDataChanged(keyword_->editSignals()));
}

void ConfigurationKeywordWidget::on_ClearButton_clicked(bool checked)
{
    if (ui_.ConfigurationCombo->currentIndex() == -1 && !keyword_->data())
        return;

    ui_.ConfigurationCombo->setCurrentIndex(-1);
}

/*
 * Update
 */

// Reset model data
void ConfigurationKeywordWidget::resetModelData()
{
    refreshing_ = true;

    configurationModel_.setData(coreData_.configurations());

    // Set current index based on keyword data
    auto it = std::find_if(coreData_.configurations().begin(), coreData_.configurations().end(),
                           [&](const auto &cfg) { return cfg.get() == keyword_->data(); });
    ui_.ConfigurationCombo->setCurrentIndex(it == coreData_.configurations().end() ? -1
                                                                                   : it - coreData_.configurations().begin());

    refreshing_ = false;
}

// Update value displayed in widget
void ConfigurationKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    if (mutationFlags.isSet(DissolveSignals::ConfigurationsMutated))
        resetModelData();
}
