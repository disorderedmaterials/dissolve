/*
    *** Keyword Widget - ExpressionVariable List
    *** src/gui/keywordwidgets/expressionvariablelist.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

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
