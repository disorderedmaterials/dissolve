// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/keywordWidgets/expressionVariableVector.h"
#include "classes/coreData.h"
#include "expression/variable.h"
#include "gui/delegates/exponentialSpin.h"

ExpressionVariableVectorKeywordWidget::ExpressionVariableVectorKeywordWidget(QWidget *parent,
                                                                             ExpressionVariableVectorKeyword *keyword,
                                                                             const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData), keyword_(keyword), variableModel_(keyword->dataModel())
{
    // Create and set up the UI for our widget
    ui_.setupUi(this);

    // Set model
    ui_.VariablesTable->setModel(&variableModel_);

    // Connect signals / slots
    connect(&variableModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), this,
            SLOT(variableDataChanged(const QModelIndex &, const QModelIndex &)));
    connect(ui_.VariablesTable->selectionModel(), SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(variableSelectionChanged(const QItemSelection &, const QItemSelection &)));

    // Add suitable delegate to the table
    ui_.VariablesTable->setItemDelegateForColumn(2, new ExponentialSpinDelegate(this));
}

/*
 * Widgets
 */

// Variable data changed
void ExpressionVariableVectorKeywordWidget::variableDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    if (refreshing_)
        return;

    Q_EMIT(keywordDataChanged(keyword_->editSignals()));
}

void ExpressionVariableVectorKeywordWidget::variableSelectionChanged(const QItemSelection &current,
                                                                     const QItemSelection &previous)
{
    ui_.RemoveVariableButton->setEnabled(current.empty());
}

void ExpressionVariableVectorKeywordWidget::ui_AddVariableButton_clicked(bool checked) {}

void ExpressionVariableVectorKeywordWidget::ui_RemoveVariableButton_clicked(bool checked) {}

// Update value displayed in widget
void ExpressionVariableVectorKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) {}
