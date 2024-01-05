// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/dropDown.h"
#include "gui/keywordWidgets/ui_function1D.h"
#include "keywords/function1D.h"
#include <QWidget>

class Function1DKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    Function1DKeywordWidget(QWidget *parent, Function1DKeyword *keyword, const CoreData &coreData);

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
    // Keyword data changed
    void keywordDataChanged(int flags);

    /*
     * Update
     */
    private:
    // Update widgets, including visibility of spins and labels
    void updateWidgetsFromData();

    public:
    // Update value displayed in widget
    void updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) override;
    // Update keyword data based on widget values
    void updateKeywordData() override;
    // Update summary text
    void updateSummaryText();
};
