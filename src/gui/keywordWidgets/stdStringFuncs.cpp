// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/keywordWidgets/stdString.hui"

StringKeywordWidget::StringKeywordWidget(QWidget *parent, StringKeyword *keyword, const CoreData &coreData)
    : QLineEdit(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    setText(QString::fromStdString(keyword_->data()));

    // Connect the currentTextChanged signal to our own slot
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(lineEditTextChanged(QString)));
}

/*
 * Signals / Slots
 */

// Line edit text changed
void StringKeywordWidget::lineEditTextChanged(const QString &text)
{
    if (refreshing_)
        return;

    keyword_->data() = qPrintable(text);

    Q_EMIT(keywordDataChanged(keyword_->editSignals()));
}

/*
 * Update
 */

// Update value displayed in widget
void StringKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    refreshing_ = true;

    setText(QString::fromStdString(keyword_->data()));

    refreshing_ = false;
}
