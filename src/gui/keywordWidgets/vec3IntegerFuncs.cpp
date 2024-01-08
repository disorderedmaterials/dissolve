// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/helpers/mouseWheelAdjustmentGuard.h"
#include "gui/keywordWidgets/vec3Integer.h"
#include "gui/keywordWidgets/vec3Labels.h"

Vec3IntegerKeywordWidget::Vec3IntegerKeywordWidget(QWidget *parent, Vec3IntegerKeyword *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = true;

    // Set minimum and maximum values for each component
    ui_.Spin1->setRange(keyword_->minimumLimit() ? keyword_->minimumLimit().value().x : -1e6,
                        keyword_->maximumLimit() ? keyword_->maximumLimit().value().x : 1e6);
    ui_.Spin2->setRange(keyword_->minimumLimit() ? keyword_->minimumLimit().value().y : -1e6,
                        keyword_->maximumLimit() ? keyword_->maximumLimit().value().y : 1e6);
    ui_.Spin3->setRange(keyword_->minimumLimit() ? keyword_->minimumLimit().value().z : -1e6,
                        keyword_->maximumLimit() ? keyword_->maximumLimit().value().z : 1e6);

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
void Vec3IntegerKeywordWidget::on_Spin1_valueChanged(int value)
{
    if (refreshing_)
        return;

    auto newVec = keyword_->data();
    newVec.x = value;
    keyword_->setData(newVec);

    Q_EMIT(keywordDataChanged(keyword_->editSignals()));
}

// Spin box value changed
void Vec3IntegerKeywordWidget::on_Spin2_valueChanged(int value)
{
    if (refreshing_)
        return;

    auto newVec = keyword_->data();
    newVec.y = value;
    keyword_->setData(newVec);

    Q_EMIT(keywordDataChanged(keyword_->editSignals()));
}

// Spin box value changed
void Vec3IntegerKeywordWidget::on_Spin3_valueChanged(int value)
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
void Vec3IntegerKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    refreshing_ = true;

    Vec3<int> v = keyword_->data();

    ui_.Spin1->setValue(v.x);
    ui_.Spin2->setValue(v.y);
    ui_.Spin3->setValue(v.z);

    refreshing_ = false;
}
