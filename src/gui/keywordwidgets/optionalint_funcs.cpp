// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/keywordwidgets/optionalint.hui"

OptionalIntegerKeywordWidget::OptionalIntegerKeywordWidget(QWidget *parent, OptionalIntegerKeyword *keyword,
                                                           const CoreData &coreData)
    : ExponentialSpin(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Set minimum and maximum values and step size
    setDecimals(0);
    setMinimum(keyword_->validationMin());
    if (keyword_->validationMax())
        setMaximum(keyword_->validationMax().value());
    setSingleStep(keyword_->valueDelta());

    // Set null value text
    setSpecialValueText(QString::fromStdString(keyword_->textWhenNull()));

    // Set current value
    setValue(keyword_->data());

    // Connect signals
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(spinBoxValueChanged(int)));
    connect(this, SIGNAL(valueNullified()), this, SLOT(spinBoxValueNullified()));

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    installEventFilter(new MouseWheelWidgetAdjustmentGuard(this));
}

/*
 * Slots
 */

// Spin box value changed
void OptionalIntegerKeywordWidget::spinBoxValueChanged(int newValue)
{
    if (refreshing_)
        return;

    if (keyword_->setData(newValue))
        emit(keywordValueChanged(keyword_->optionMask()));
}

// Spin box value nullified
void OptionalIntegerKeywordWidget::spinBoxValueNullified()
{
    if (refreshing_ || !keyword_->data())
        return;

    keyword_->setData(std::nullopt);
    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void OptionalIntegerKeywordWidget::updateValue()
{
    refreshing_ = true;

    setValue(keyword_->data());

    refreshing_ = false;
}
