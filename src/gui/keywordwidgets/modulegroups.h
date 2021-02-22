// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/ui_modulegroups.h"
#include "keywords/modulegroups.h"
#include "module/module.h"
#include <QWidget>

// Forward Declarations
class ModuleGroup;

class ModuleGroupsKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    ModuleGroupsKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    ModuleGroupsKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::ModuleGroupsWidget ui_;

    private:
    // Selection table update function
    void updateSelectionRow(int row, const Module *module, bool create);

    private slots:
    // Table item changed
    void itemChanged(QTableWidgetItem *item);

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

Q_DECLARE_METATYPE(const Module *);
