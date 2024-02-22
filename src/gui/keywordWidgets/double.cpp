// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/keywordWidgets/double.h"
#include "gui/helpers/mouseWheelAdjustmentGuard.h"

DoubleKeywordWidget::DoubleKeywordWidget(QWidget *parent, DoubleKeyword *keyword, const CoreData &coreData)
    : ExponentialSpin(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Set minimum and maximum values
    if (keyword_->minimumLimit())
        setMinimum(keyword_->minimumLimit().value());
    if (keyword_->maximumLimit())
        setMaximum(keyword_->maximumLimit().value());

    // Set current value
    setValue(keyword_->data());

    // Connect the valueChanged signal to our own slot
    connect(this, SIGNAL(valueChanged(double)), this, SLOT(spinBoxValueChanged(double)));

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    installEventFilter(new MouseWheelWidgetAdjustmentGuard(this));
}

/*
 * Slots
 */

// Spin box value changed
void DoubleKeywordWidget::spinBoxValueChanged(double newValue)
{
    if (refreshing_)
        return;

    if (keyword_->setData(newValue))
        Q_EMIT(keywordDataChanged(keyword_->editSignals()));
}

/*
 * Update
 */

// Update value displayed in widget
void DoubleKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    refreshing_ = true;

    setValue(keyword_->data());

    refreshing_ = false;
}
