// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/ui_speciessite.h"
#include "keywords/speciessite.h"
#include <QWidget>

// Forward Declarations
class Species;

class SpeciesSiteKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    SpeciesSiteKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    SpeciesSiteKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::SpeciesSiteWidget ui_;

    private slots:
    void siteRadioButton_clicked(bool checked);

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
    // Update summary text
    void updateSummaryText();
};
