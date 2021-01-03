// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/ui_speciessitereflist.h"
#include "keywords/speciessitereflist.h"
#include <QWidget>

// Forward Declarations
class Species;

class SpeciesSiteRefListKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    SpeciesSiteRefListKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    SpeciesSiteRefListKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::SpeciesSiteRefListWidget ui_;

    private slots:
    void siteCheckBox_clicked(bool checked);

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
