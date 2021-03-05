// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/keywordwidgets/stdstring.hui"

StringKeywordWidget::StringKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : QLineEdit(parent), KeywordWidgetBase(coreData)
{
    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<StringKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into StringKeyword.\n", keyword->name());
    else
        setText(QString::fromStdString(keyword_->asString()));

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

    keyword_->setData(qPrintable(text));

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void StringKeywordWidget::updateValue()
{
    refreshing_ = true;

    setText(QString::fromStdString(keyword_->asString()));

    refreshing_ = false;
}
