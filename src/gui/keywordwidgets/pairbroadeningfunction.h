// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/ui_pairbroadeningfunction.h"
#include "keywords/pairbroadeningfunction.h"
#include <QWidget>

// Forward Declarations
class QComboBox;

class PairBroadeningFunctionKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    PairBroadeningFunctionKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    PairBroadeningFunctionKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::PairBroadeningFunctionWidget ui_;

    private slots:
    // Function type radio changed
    void functionRadioChanged(bool checked);
    // Function parameter value changed
    void functionParameterChanged(double value);

    signals:
    // Keyword value changed
    void keywordValueChanged(int flags);

    /*
     * Update
     */
    public:
    // Update value displayed in widget
    void updateValue();
    // Update widget values data based on keyword data
    void updateWidgetValues(const CoreData &coreData);
    // Update keyword data based on widget values
    void updateKeywordData();
};
