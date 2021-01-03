// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/ui_nodereflist.h"
#include "keywords/nodereflist.h"
#include <QWidget>

// Forward Declarations
/* none */

class NodeRefListKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    NodeRefListKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    NodeRefListKeywordBase *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::NodeRefListWidget ui_;

    private slots:
    void nodeItemChanged(QListWidgetItem *item);

    signals:
    // Keyword value changed
    void keywordValueChanged(int flags);

    /*
     * Update
     */
    private:
    // List widget row update function
    void updateListRow(int row, ProcedureNode *node, bool createItem);

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
