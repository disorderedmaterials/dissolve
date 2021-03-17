// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/keywordwidgets/double.hui"

DoubleKeywordWidget::DoubleKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : ExponentialSpin(parent), KeywordWidgetBase(coreData)
{
    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<DoubleKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into DoubleKeyword.\n", keyword->name());
    else
    {
        // Set minimum and maximum values
        if (keyword_->hasValidationMin())
            setMinimumLimit(keyword_->validationMin());
        if (keyword_->hasValidationMax())
            setMaximumLimit(keyword_->validationMax());

        // Set current value
        setValue(keyword_->asDouble());
    }

    // Connect the valueChanged signal to our own slot
    connect(this, SIGNAL(valueChanged(double)), this, SLOT(myValueChanged(double)));

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    installEventFilter(new MouseWheelWidgetAdjustmentGuard(this));
}

/*
 * Slots
 */

// Spin box value changed
void DoubleKeywordWidget::myValueChanged(double newValue)
{
    if (refreshing_)
        return;

    keyword_->setData(newValue);

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void DoubleKeywordWidget::updateValue()
{
    refreshing_ = true;

    setValue(keyword_->asDouble());

    refreshing_ = false;
}
