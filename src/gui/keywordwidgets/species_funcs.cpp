// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/coredata.h"
#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/keywordwidgets/species.h"

SpeciesKeywordWidget::SpeciesKeywordWidget(QWidget *parent, SpeciesKeyword *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = true;
    speciesModel_.setData(coreData.species());
    ui_.SpeciesCombo->setModel(&speciesModel_);
    auto it = std::find_if(coreData.species().begin(), coreData.species().end(),
                           [keyword](const auto &sp) { return sp.get() == keyword->data(); });
    ui_.SpeciesCombo->setCurrentIndex(it == coreData.species().end() ? -1 : it - coreData.species().begin());

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    ui_.SpeciesCombo->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.SpeciesCombo));

    refreshing_ = false;
}

/*
 * Widgets
 */

// Value changed
void SpeciesKeywordWidget::on_SpeciesCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    // Get data from the selected item
    if (index == -1)
        keyword_->data() = nullptr;
    else
        keyword_->data() = ui_.SpeciesCombo->currentData(Qt::UserRole).value<Species *>();

    emit(keywordDataChanged(keyword_->editSignals()));
}

/*
 * Update
 */

// Update value displayed in widget
void SpeciesKeywordWidget::updateValue() {}
