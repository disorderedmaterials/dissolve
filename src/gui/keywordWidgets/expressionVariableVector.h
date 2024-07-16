// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/dataTableModelInterface.h"
#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/ui_expressionVariableVector.h"
#include "keywords/expressionVariableVector.h"

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
    DataTableModelInterface variableModel_;

    private Q_SLOTS:
    void variableDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void variableSelectionChanged(const QItemSelection &current, const QItemSelection &previous);
    void on_AddVariableButton_clicked(bool checked);
    void on_RemoveVariableButton_clicked(bool checked);

    Q_SIGNALS:
    // Keyword data changed
    void keywordDataChanged(int flags);

    /*
     * Update
     */
    public:
    // Update value displayed in widget
    void updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) override;
};
