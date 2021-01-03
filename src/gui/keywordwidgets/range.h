// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/ui_range.h"
#include "keywords/range.h"
#include <QWidget>

// Forward Declarations
/* none */

class RangeKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    RangeKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);

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

    private slots:
    void on_Spin1_valueChanged(double value);
    void on_Spin2_valueChanged(double value);

    signals:
    // Keyword value changed
    void keywordValueChanged(int flags);

    /*
     * Update
     */
    public:
    // Update value displayed in widget
    void updateValue();
};
