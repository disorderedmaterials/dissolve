// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/coreData.h"
#include "expression/variable.h"
#include "gui/delegates/exponentialSpin.hui"
#include "gui/keywordWidgets/expressionVariableVector.h"

ExpressionVariableVectorKeywordWidget::ExpressionVariableVectorKeywordWidget(QWidget *parent,
                                                                             ExpressionVariableVectorKeyword *keyword,
                                                                             const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Create and set up the UI for our widget
    ui_.setupUi(this);

    // Set up model
    variableModel_.setData(keyword->data(), keyword->parentNode());
    ui_.VariablesTable->setModel(&variableModel_);

    // Connect signals / slots
    connect(&variableModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)), this,
            SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));

    // Add suitable delegate to the table
    ui_.VariablesTable->setItemDelegateForColumn(2, new ExponentialSpinDelegate(this));
}

/*
 * Widgets
 */

// Variable data changed
void ExpressionVariableVectorKeywordWidget::modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    if (refreshing_)
        return;

    emit(keywordDataChanged(keyword_->editSignals()));
}

// Update value displayed in widget
void ExpressionVariableVectorKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) {}
