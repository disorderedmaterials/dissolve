// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/coreData.h"
#include "gui/helpers/mouseWheelAdjustmentGuard.h"
#include "gui/keywordWidgets/species.h"

SpeciesKeywordWidget::SpeciesKeywordWidget(QWidget *parent, SpeciesKeyword *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = true;

    // Set up the model
    ui_.SpeciesCombo->setModel(&speciesModel_);
    resetModelData();

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    ui_.SpeciesCombo->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.SpeciesCombo));

    refreshing_ = false;
}

/*
 * Widgets
 */

void SpeciesKeywordWidget::on_SpeciesCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    // Get data from the selected item
    if (index == -1)
        keyword_->data() = nullptr;
    else
        keyword_->data() = ui_.SpeciesCombo->currentData(Qt::UserRole).value<const Species *>();

    Q_EMIT(keywordDataChanged(keyword_->editSignals()));
}

void SpeciesKeywordWidget::on_ClearButton_clicked(bool checked)
{
    if (ui_.SpeciesCombo->currentIndex() == -1 && !keyword_->data())
        return;

    ui_.SpeciesCombo->setCurrentIndex(-1);
}

/*
 * Update
 */

// Reset model data
void SpeciesKeywordWidget::resetModelData()
{
    refreshing_ = true;

    speciesModel_.setData(coreData_.species());

    auto it = std::find_if(coreData_.species().begin(), coreData_.species().end(),
                           [&](const auto &sp) { return sp.get() == keyword_->data(); });
    ui_.SpeciesCombo->setCurrentIndex(it == coreData_.species().end() ? -1 : it - coreData_.species().begin());

    refreshing_ = false;
}

// Update value displayed in widget
void SpeciesKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) {}
