/*
    *** Keyword Widget - ExpressionVariable List
    *** src/gui/keywordwidgets/expressionvariablelist_funcs.cpp
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

#include "classes/coredata.h"
#include "expression/variable.h"
#include "gui/delegates/exponentialspin.hui"
#include "gui/delegates/integerspin.hui"
#include "gui/helpers/tablewidgetupdater.h"
#include "gui/keywordwidgets/expressionvariablelist.h"
#include "procedure/nodes/node.h"
#include "templates/variantpointer.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QString>

ExpressionVariableListKeywordWidget::ExpressionVariableListKeywordWidget(QWidget *parent, KeywordBase *keyword,
                                                                         const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData)
{
    // Create and set up the UI for our widget
    ui_.setupUi(this);

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<ExpressionVariableListKeyword *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into ExpressionVariableListKeyword.\n", keyword->name());
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
void ExpressionVariableListKeywordWidget::updateVariableTableRow(int row, ExpressionNode *node, bool createItem)
{
    // Case the node up into an ExpressionVariable
    auto *var = dynamic_cast<ExpressionVariable *>(node);
    if (!var)
        return;

    QTableWidgetItem *item;

    // Name
    if (createItem)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, VariantPointer<ExpressionVariable>(var));
        ui_.VariablesTable->setItem(row, 0, item);
    }
    else
        item = ui_.VariablesTable->item(row, 0);
    item->setText(QString::fromStdString(std::string(var->name())));

    // Value
    if (createItem)
    {
        item = new QTableWidgetItem;
        item->setData(Qt::UserRole, VariantPointer<ExpressionVariable>(var));
        ui_.VariablesTable->setItem(row, 1, item);
    }
    else
        item = ui_.VariablesTable->item(row, 1);
    item->setText(QString::fromStdString(var->value().asString()));
}

// Variable data changed
void ExpressionVariableListKeywordWidget::on_VariablesTable_itemChanged(QTableWidgetItem *w)
{
    if (refreshing_)
        return;

    // Get the widget's data (our ExpressionVariable)
    ExpressionVariable *var = VariantPointer<ExpressionVariable>(w->data(Qt::UserRole));
    if (!var)
        return;

    // Check column of the item to see what we need to do
    switch (w->column())
    {
        // Variable name
        case (0):
            // Check that the name is not currently in use anywhere in the Procedure
            if (keyword_->parentNode()->parameterExists(qPrintable(w->text()), var))
            {
                Messenger::error("A Node with name '{}' already exists elsewhere in the Procedure.\n", qPrintable(w->text()));
                w->setText(QString::fromStdString(std::string(var->name())));
                return;
            }
            else
                var->setName(qPrintable(w->text()));
            break;
        // Variable value
        case (1):
            // Set the new value
            if (keyword_->variableType() == ExpressionValue::IntegerType)
                var->set(w->text().toInt());
            else if (keyword_->variableType() == ExpressionValue::DoubleType)
                var->set(w->text().toDouble());
            break;
    }

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void ExpressionVariableListKeywordWidget::updateValue()
{
    refreshing_ = true;

    // Update the variables list against that contained in the keyword's data
    TableWidgetUpdater<ExpressionVariableListKeywordWidget, ExpressionNode> tableUpdater(
        ui_.VariablesTable, keyword_->data(), this, &ExpressionVariableListKeywordWidget::updateVariableTableRow);

    refreshing_ = false;
}
