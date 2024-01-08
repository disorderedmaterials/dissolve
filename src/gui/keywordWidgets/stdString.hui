// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "keywords/stdString.h"
#include <QLineEdit>

class StringKeywordWidget : public QLineEdit, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    StringKeywordWidget(QWidget *parent, StringKeyword *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    StringKeyword *keyword_;

    /*
     * Signals / Slots
     */
    private Q_SLOTS:
    // Line edit text changed
    void lineEditTextChanged(const QString &text);

    Q_SIGNALS:
    // Keyword data changed
    void keywordDataChanged(int flags);

    /*
     * Update
     */
    public:
    // Update value displayed in widget
    void updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) override;
};
