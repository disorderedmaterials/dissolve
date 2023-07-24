// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/helpers/mouseWheelAdjustmentGuard.h"
#include "gui/keywordWidgets/layer.h"

LayerKeywordWidget::LayerKeywordWidget(QWidget *parent, LayerKeyword *keyword, const CoreData &coreData) : QWidget(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = false;

    ui_.LayerCombo->setModel(&moduleLayersModel_);
    updateLayersCombo();

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    ui_.LayerCombo->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.LayerCombo));
}

/*
 * Widgets
 */

void LayerKeywordWidget::on_LayerCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    // Get data from the selected item
    
    if (index == -1)
        keyword_->setData(nullptr);
    else
        keyword_->setData(ui_.LayerCombo->currentData(Qt::UserRole).value<ModuleLayer *>());

    emit(keywordDataChanged(keyword_->editSignals()));
}

void LayerKeywordWidget::on_ClearButton_clicked(bool checked)
{
    if (ui_.LayerCombo->currentIndex() == -1 && !keyword_->data())
        return;

    ui_.LayerCombo->setCurrentIndex(-1);
}

/*
 * Update
 */
void LayerKeywordWidget::updateLayersCombo()
{
    refreshing_ = true;
    
    moduleLayersModel_.setData(coreData_.processingLayers());

    refreshing_ = false;
}

// Update value displayed in widget
void LayerKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    if (mutationFlags.isSet(DissolveSignals::ModulesMutated))
        updateLayersCombo();
}
