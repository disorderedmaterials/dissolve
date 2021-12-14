// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/keywordwidgets/range.h"
#include "vec3labels.h"

RangeKeywordWidget::RangeKeywordWidget(QWidget *parent, RangeKeyword *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = true;

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

    keyword_->data().setMinimum(value);

    emit(keywordDataChanged(keyword_->signalMask()));
}

// Spin box value changed
void RangeKeywordWidget::on_Spin2_valueChanged(double value)
{
    if (refreshing_)
        return;

    keyword_->data().setMaximum(value);

    emit(keywordDataChanged(keyword_->signalMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void RangeKeywordWidget::updateValue()
{
    refreshing_ = true;

    ui_.Spin1->setValue(keyword_->data().minimum());
    ui_.Spin2->setValue(keyword_->data().maximum());

    refreshing_ = false;
}
