// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "classes/coredata.h"
#include "expression/variable.h"
#include "gui/delegates/exponentialspin.hui"
#include "gui/delegates/integerspin.hui"
#include "gui/helpers/tablewidgetupdater.h"
#include "gui/keywordwidgets/expressionvariablevector.h"
#include "procedure/nodes/node.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QString>

Q_DECLARE_SMART_POINTER_METATYPE(std::shared_ptr)
Q_DECLARE_METATYPE(std::shared_ptr<ExpressionVariable>)

ExpressionVariableVectorKeywordWidget::ExpressionVariableVectorKeywordWidget(QWidget *parent, KeywordBase *keyword,
                                                                             const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData)
{
    // Create and set up the UI for our widget
    ui_.setupUi(this);

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<ExpressionVariableVectorKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into ExpressionVariableVectorKeyword.\n", keyword->name());
    else
    {
        // Set current information
        updateValue();
    }

    // Add suitable delegate to the table
    if (keyword_->variableType() == ExpressionValue::IntegerType)
        ui_.VariablesTable->setItemDelegateForColumn(1, new IntegerSpinDelegate(this));
    else if (keyword_->variableType() == ExpressionValue::DoubleType)
        ui_.VariablesTable->setItemDelegateForColumn(1, new ExponentialSpinDelegate(this));
}

/*
 * Widgets
 */

// Variable table update function
void ExpressionVariableVectorKeywordWidget::updateVariableTableRow(int row, std::shared_ptr<ExpressionVariable> variable,
                                                                   bool createItem)
{
    QTableWidgetItem *item;

    // Name
    if (createItem)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, QVariant::fromValue(variable));
        ui_.VariablesTable->setItem(row, 0, item);
    }
    else
        item = ui_.VariablesTable->item(row, 0);
    item->setText(QString::fromStdString(std::string(variable->name())));

    // Value
    if (createItem)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, QVariant::fromValue(variable));
        ui_.VariablesTable->setItem(row, 1, item);
    }
    else
        item = ui_.VariablesTable->item(row, 1);
    item->setText(QString::fromStdString(variable->value().asString()));
}

// Variable data changed
void ExpressionVariableVectorKeywordWidget::on_VariablesTable_itemChanged(QTableWidgetItem *w)
{
    if (refreshing_)
        return;

    // Get the widget's data (our ExpressionVariable)
    std::shared_ptr<ExpressionVariable> variable = w->data(Qt::UserRole).value<std::shared_ptr<ExpressionVariable>>();
    if (!variable)
        return;

    // Check column of the item to see what we need to do
    switch (w->column())
    {
        // Variable name
        case (0):
            // Check that the name is not currently in use anywhere in the Procedure
            if (keyword_->parentNode()->parameterExists(qPrintable(w->text()), variable))
            {
                Messenger::error("A Node with name '{}' already exists elsewhere in the Procedure.\n", qPrintable(w->text()));
                w->setText(QString::fromStdString(std::string(variable->name())));
                return;
            }
            else
                variable->setName(qPrintable(w->text()));
            break;
        // Variable value
        case (1):
            // Set the new value
            if (keyword_->variableType() == ExpressionValue::IntegerType)
                variable->setValue(w->text().toInt());
            else if (keyword_->variableType() == ExpressionValue::DoubleType)
                variable->setValue(w->text().toDouble());
            break;
    }

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void ExpressionVariableVectorKeywordWidget::updateValue()
{
    refreshing_ = true;

    // Update the variables list against that contained in the keyword's data
    TableWidgetUpdater<ExpressionVariableVectorKeywordWidget, ExpressionVariable, std::shared_ptr<ExpressionVariable>>
        tableUpdater(ui_.VariablesTable, keyword_->data(), this,
                     &ExpressionVariableVectorKeywordWidget::updateVariableTableRow);

    refreshing_ = false;
}
