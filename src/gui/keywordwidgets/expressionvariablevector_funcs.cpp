// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
    ui_.VariablesTable->setItemDelegateForColumn(2, new ExponentialSpinDelegate(this));
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

    // Type
    if (createItem)
    {
        item = new QTableWidgetItem;
        item->setFlags(Qt::ItemIsSelectable);
        item->setData(Qt::UserRole, QVariant::fromValue(variable));
        ui_.VariablesTable->setItem(row, 1, item);
    }
    else
        item = ui_.VariablesTable->item(row, 1);
    item->setText(variable->value().type() == ExpressionValue::ValueType::Integer ? "int" : "double");

    // Value
    if (createItem)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, QVariant::fromValue(variable));
        ui_.VariablesTable->setItem(row, 2, item);
    }
    else
        item = ui_.VariablesTable->item(row, 2);
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
        case (2):
            // Determine the type of the new value so we can convert and set accordingly
            bool isFloatingPoint = false,
                 originalFloatingPoint = variable->value().type() == ExpressionValue::ValueType::Double;
            if (DissolveSys::isNumber(qPrintable(w->text()), isFloatingPoint))
            {
                if (isFloatingPoint)
                    variable->setValue(w->text().toDouble());
                else
                    variable->setValue(w->text().toInt());

                // Update the type?
                if (isFloatingPoint != originalFloatingPoint)
                {
                    auto *item = ui_.VariablesTable->item(w->row(), 1);
                    if (item)
                        item->setText(isFloatingPoint ? "double" : "int");
                }
            }
            else
                Messenger::error("Invalid input - not a number.\n");
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
    ConstTableWidgetUpdater<ExpressionVariableVectorKeywordWidget, ExpressionVariable, std::shared_ptr<ExpressionVariable>>
        tableUpdater(ui_.VariablesTable, keyword_->data(), this,
                     &ExpressionVariableVectorKeywordWidget::updateVariableTableRow);

    refreshing_ = false;
}
