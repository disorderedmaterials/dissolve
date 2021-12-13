// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/ui_expressionvariablevector.h"
#include "gui/models/expressionVariableVectorModel.h"
#include "keywords/expressionvariablevector.h"

// Forward Declarations
class QWidget;

class ExpressionVariableVectorKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    ExpressionVariableVectorKeywordWidget(QWidget *parent, ExpressionVariableVectorKeyword *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    ExpressionVariableVectorKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::ExpressionVariableVectorWidget ui_;
    // Model for table
    ExpressionVariableVectorModel variableModel_;

    private slots:
    void modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    signals:
    // Keyword value changed
    void keywordValueChanged(int flags);

    /*
     * Update
     */
    public:
    // Update value displayed in widget
    void updateValue() override;
};
