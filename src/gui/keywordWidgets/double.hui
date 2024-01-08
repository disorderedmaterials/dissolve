// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/widgets/exponentialSpin.hui"
#include "keywords/double.h"

class DoubleKeywordWidget : public ExponentialSpin, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    DoubleKeywordWidget(QWidget *parent, DoubleKeyword *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    DoubleKeyword *keyword_;

    /*
     * Signals / Slots
     */
    private Q_SLOTS:
    // Spin box value changed
    void spinBoxValueChanged(double newValue);

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
