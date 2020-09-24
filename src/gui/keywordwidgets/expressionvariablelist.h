// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/ui_expressionvariablelist.h"
#include "keywords/expressionvariablelist.h"
#include "procedure/nodes/node.h"

Q_DECLARE_METATYPE(ExpressionNode *);

// Forward Declarations
class QWidget;

class ExpressionVariableListKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    ExpressionVariableListKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    ExpressionVariableListKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::ExpressionVariableListWidget ui_;

    private:
    // Variable table update function
    void updateVariableTableRow(int row, ExpressionNode *node, bool createItem);

    private slots:
    // Variables table item changed
    void on_VariablesTable_itemChanged(QTableWidgetItem *item);

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
