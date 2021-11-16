// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/keywordwidgets/bool.hui"

BoolKeywordWidget::BoolKeywordWidget(QWidget *parent, BoolKeyword *keyword, const CoreData &coreData)
    : QCheckBox(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Set current state
    setChecked(keyword_->data());

    // Connect the
    connect(this, SIGNAL(clicked(bool)), this, SLOT(myClicked(bool)));
}

/*
 * Signals / Slots
 */

// Check box state changed
void BoolKeywordWidget::myClicked(bool checked)
{
    if (refreshing_)
        return;

    keyword_->setData(checked);

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void BoolKeywordWidget::updateValue()
{
    refreshing_ = true;

    setChecked(keyword_->data());

    refreshing_ = false;
}
