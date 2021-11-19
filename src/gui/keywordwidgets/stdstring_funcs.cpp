// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/keywordwidgets/stdstring.hui"

StringKeywordWidget::StringKeywordWidget(QWidget *parent, StringKeyword *keyword, const CoreData &coreData)
    : QLineEdit(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    setText(QString::fromStdString(keyword_->data()));

    // Connect the currentTextChanged signal to our own slot
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(myTextChanged(QString)));
}

/*
 * Signals / Slots
 */

// Line edit text changed
void StringKeywordWidget::myTextChanged(const QString &text)
{
    if (refreshing_)
        return;

    keyword_->data() = qPrintable(text);
    keyword_->setAsModified();

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void StringKeywordWidget::updateValue()
{
    refreshing_ = true;

    setText(QString::fromStdString(keyword_->data()));

    refreshing_ = false;
}
