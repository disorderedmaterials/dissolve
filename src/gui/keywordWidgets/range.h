// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/ui_range.h"
#include "keywords/range.h"
#include <QWidget>

class RangeKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    RangeKeywordWidget(QWidget *parent, RangeKeyword *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    RangeKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::RangeWidget ui_;

    private Q_SLOTS:
    void on_Spin1_valueChanged(double value);
    void on_Spin2_valueChanged(double value);

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
