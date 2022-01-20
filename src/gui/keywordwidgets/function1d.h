// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/ui_function1d.h"
#include "keywords/function1d.h"
#include <QWidget>

class Function1DKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    Function1DKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    Function1DKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::Function1DWidget ui_;
    std::vector<ExponentialSpin *> spins_;
    std::vector<QLabel *> labels_;

    private slots:
    // Function type combo changed
    void functionCombo_currentIndexChanged(int index);
    // Parameter value changed
    void parameterSpin_valueChanged(double value);

    signals:
    // Keyword value changed
    void keywordValueChanged(int flags);

    /*
     * Update
     */
    public:
    // Update value displayed in widget
    void updateValue() override;
    // Update widget values data based on keyword data
    void updateWidgetValues(const CoreData &coreData) override;
    // Update keyword data based on widget values
    void updateKeywordData() override;
    // Update summary text
    void updateSummaryText();
};
