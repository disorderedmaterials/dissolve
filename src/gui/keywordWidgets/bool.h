// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "keywords/bool.h"
#include <QCheckBox>

class BoolKeywordWidget : public QCheckBox, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    BoolKeywordWidget(QWidget *parent, BoolKeyword *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    BoolKeyword *keyword_;

    /*
     * Signals / Slots
     */
    private Q_SLOTS:
    // Check box state changed
    void checkBoxClicked(bool checked);

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
