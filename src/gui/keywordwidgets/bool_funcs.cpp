// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/keywordwidgets/bool.hui"

BoolKeywordWidget::BoolKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : QCheckBox(parent), KeywordWidgetBase(coreData)
{
    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<BoolKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into BoolKeyword.\n", keyword->name());
    else
    {
        // Set current value
        setChecked(keyword_->asBool());
    }

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

    setChecked(keyword_->asBool());

    refreshing_ = false;
}
