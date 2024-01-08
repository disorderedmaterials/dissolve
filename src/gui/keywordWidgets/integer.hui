// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "keywords/integer.h"
#include <QSpinBox>

class IntegerKeywordWidget : public QSpinBox, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    IntegerKeywordWidget(QWidget *parent, IntegerKeyword *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    IntegerKeyword *keyword_;

    /*
     * Signals / Slots
     */
    private Q_SLOTS:
    // Spin box value changed
    void spinBoxValueChanged(int newValue);

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
