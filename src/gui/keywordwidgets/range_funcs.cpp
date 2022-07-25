// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/keywordwidgets/range.h"
#include "vec3labels.h"

RangeKeywordWidget::RangeKeywordWidget(QWidget *parent, RangeKeyword *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = true;

    // Set minimum and maximum values
    if (keyword_->minimumLimit())
    {
        ui_.Spin1->setMinimum(keyword_->minimumLimit().value());
        ui_.Spin2->setMinimum(keyword_->minimumLimit().value());
    }
    if (keyword_->maximumLimit())
    {
        ui_.Spin1->setMaximum(keyword_->maximumLimit().value());
        ui_.Spin2->setMaximum(keyword_->maximumLimit().value());
    }

    // Set current values
    ui_.Spin1->setValue(keyword_->data().minimum());
    ui_.Spin2->setValue(keyword_->data().maximum());

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    ui_.Spin1->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.Spin1));
    ui_.Spin2->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.Spin2));

    // Set appropriate labels
    Vec3WidgetLabels::set(ui_.Label1, keyword_->labelType(), 0);
    Vec3WidgetLabels::set(ui_.Label2, keyword_->labelType(), 1);

    refreshing_ = false;
}

/*
 * Widgets
 */

// Spin box value changed
void RangeKeywordWidget::on_Spin1_valueChanged(double value)
{
    if (refreshing_)
        return;

    if (keyword_->setMinimum(value))
        emit(keywordDataChanged(keyword_->editSignals()));
    else
    {
        refreshing_ = true;
        ui_.Spin1->setValue(keyword_->data().minimum());
        refreshing_ = false;
    }
}

// Spin box value changed
void RangeKeywordWidget::on_Spin2_valueChanged(double value)
{
    if (refreshing_)
        return;

    if (keyword_->setMaximum(value))
        emit(keywordDataChanged(keyword_->editSignals()));
    else
    {
        refreshing_ = true;
        ui_.Spin2->setValue(keyword_->data().maximum());
        refreshing_ = false;
    }
}

/*
 * Update
 */

// Update value displayed in widget
void RangeKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    refreshing_ = true;

    ui_.Spin1->setValue(keyword_->data().minimum());
    ui_.Spin2->setValue(keyword_->data().maximum());

    refreshing_ = false;
}
