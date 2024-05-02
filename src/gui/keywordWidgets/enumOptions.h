// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "keywords/enumOptions.h"
#include <QComboBox>

class EnumOptionsKeywordWidget : public QComboBox, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    EnumOptionsKeywordWidget(QWidget *parent, EnumOptionsBaseKeyword *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    EnumOptionsBaseKeyword *keyword_;

    /*
     * Signals / Slots
     */
    private Q_SLOTS:
    // Combo box index changed
    void comboBoxIndexChanged(int index);

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
