// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/keywordwidgets/integer.hui"

IntegerKeywordWidget::IntegerKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : QSpinBox(parent), KeywordWidgetBase(coreData)
{
    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<IntegerKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into IntegerKeyword.\n", keyword->name());
    else
    {
        // Set minimum and maximum values
        setRange(keyword_->hasValidationMin() ? keyword_->validationMin() : -1e6,
                 keyword_->hasValidationMax() ? keyword_->validationMax() : 1e6);

        // Set current value
        setValue(keyword_->asInt());
    }

    // Connect the
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(myValueChanged(int)));

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    installEventFilter(new MouseWheelWidgetAdjustmentGuard(this));
}

/*
 * Signals / Slots
 */

// Spin box value changed
void IntegerKeywordWidget::myValueChanged(int newValue)
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
void IntegerKeywordWidget::updateValue()
{
    refreshing_ = true;

    setValue(keyword_->asInt());

    refreshing_ = false;
}
