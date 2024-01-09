// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/helpers/mouseWheelAdjustmentGuard.h"
#include "gui/keywordWidgets/vec3Double.h"
#include "gui/keywordWidgets/vec3Labels.h"

Vec3DoubleKeywordWidget::Vec3DoubleKeywordWidget(QWidget *parent, Vec3DoubleKeyword *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = true;

    // Set minimum and maximum values for each component
    if (keyword_->minimumLimit())
    {
        ui_.Spin1->setMinimum(keyword_->minimumLimit().value().x);
        ui_.Spin2->setMinimum(keyword_->minimumLimit().value().y);
        ui_.Spin3->setMinimum(keyword_->minimumLimit().value().z);
    }
    if (keyword_->maximumLimit())
    {
        ui_.Spin1->setMaximum(keyword_->maximumLimit().value().x);
        ui_.Spin2->setMaximum(keyword_->maximumLimit().value().y);
        ui_.Spin3->setMaximum(keyword_->maximumLimit().value().z);
    }

    // Set current values
    ui_.Spin1->setValue(keyword_->data().x);
    ui_.Spin2->setValue(keyword_->data().y);
    ui_.Spin3->setValue(keyword_->data().z);

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    ui_.Spin1->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.Spin1));
    ui_.Spin2->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.Spin2));
    ui_.Spin3->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.Spin3));

    // Set appropriate labels
    Vec3WidgetLabels::set(ui_.Label1, keyword_->labelType(), 0);
    Vec3WidgetLabels::set(ui_.Label2, keyword_->labelType(), 1);
    Vec3WidgetLabels::set(ui_.Label3, keyword_->labelType(), 2);

    refreshing_ = false;
}

/*
 * Widgets
 */

// Spin box value changed
void Vec3DoubleKeywordWidget::on_Spin1_valueChanged(double value)
{
    if (refreshing_)
        return;

    auto newVec = keyword_->data();
    newVec.x = value;
    keyword_->setData(newVec);

    Q_EMIT(keywordDataChanged(keyword_->editSignals()));
}

// Spin box value changed
void Vec3DoubleKeywordWidget::on_Spin2_valueChanged(double value)
{
    if (refreshing_)
        return;

    auto newVec = keyword_->data();
    newVec.y = value;
    keyword_->setData(newVec);

    Q_EMIT(keywordDataChanged(keyword_->editSignals()));
}

// Spin box value changed
void Vec3DoubleKeywordWidget::on_Spin3_valueChanged(double value)
{
    if (refreshing_)
        return;

    auto newVec = keyword_->data();
    newVec.z = value;
    keyword_->setData(newVec);

    Q_EMIT(keywordDataChanged(keyword_->editSignals()));
}

/*
 * Update
 */

// Update value displayed in widget
void Vec3DoubleKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    refreshing_ = true;

    const auto v = keyword_->data();

    ui_.Spin1->setValue(v.x);
    ui_.Spin2->setValue(v.y);
    ui_.Spin3->setValue(v.z);

    refreshing_ = false;
}
