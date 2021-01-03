// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/ui_broadeningfunction.h"
#include "keywords/broadeningfunction.h"
#include <QWidget>

// Forward Declarations
class QComboBox;

class BroadeningFunctionKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    BroadeningFunctionKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    BroadeningFunctionKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::BroadeningFunctionWidget ui_;

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
    void updateValue();
    // Update widget values data based on keyword data
    void updateWidgetValues(const CoreData &coreData);
    // Update keyword data based on widget values
    void updateKeywordData();
};
