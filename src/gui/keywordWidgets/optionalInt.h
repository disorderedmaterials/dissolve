// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/widgets/integerSpin.h"
#include "keywords/optionalInt.h"

class OptionalIntegerKeywordWidget : public IntegerSpin, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    OptionalIntegerKeywordWidget(QWidget *parent, OptionalIntegerKeyword *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    OptionalIntegerKeyword *keyword_;

    /*
     * Signals / Slots
     */
    private Q_SLOTS:
    // Spin box value changed
    void spinBoxValueChanged(int newValue);
    // Spin box value nullified
    void spinBoxValueNullified();

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
