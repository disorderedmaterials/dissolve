/*
    *** Keyword Widget - Node RefList
    *** src/gui/keywordwidgets/nodereflist_funcs.cpp
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
#include "gui/helpers/listwidgetupdater.h"
#include "gui/keywordwidgets/nodereflist.h"
#include "templates/variantpointer.h"
#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpacerItem>

NodeRefListKeywordWidget::NodeRefListKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<NodeRefListKeywordBase *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into NodeKeyword.\n", keyword->name());
    else
    {
        // Set current information
        updateWidgetValues(coreData_);
    }

    // Connect signals / slots
    connect(ui_.NodeList, SIGNAL(itemChanged(QListWidgetItem *)), this, SLOT(nodeItemChanged(QListWidgetItem *)));

    // Summary text on KeywordDropDown button
    setSummaryText("Edit...");
}

/*
 * Widgets
 */

void NodeRefListKeywordWidget::nodeItemChanged(QListWidgetItem *item)
{
    if (refreshing_ || (!item))
        return;

    // Get the ProcedureNode from the item
    ProcedureNode *node = VariantPointer<ProcedureNode>(item->data(Qt::UserRole));
    if (!node)
        return;

    // If the box is checked, we need to add the site to the list. If not, remove it.
    if (item->checkState() == Qt::Checked)
        keyword_->addNode(node);
    else
        keyword_->removeNode(node);
}

/*
 * Update
 */

// List widget row update function
void NodeRefListKeywordWidget::updateListRow(int row, ProcedureNode *node, bool createItem)
{
    QListWidgetItem *item;
    if (createItem)
    {
        item = new QListWidgetItem;
        item->setData(Qt::UserRole, VariantPointer<ProcedureNode>(node));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsUserCheckable);
        ui_.NodeList->insertItem(row, item);
    }
    else
        item = ui_.NodeList->item(row);

    // Set item data
    item->setText(QString::fromStdString(std::string(node->name())));
    item->setCheckState(keyword_->hasNode(node) ? Qt::Checked : Qt::Unchecked);
}

// Update value displayed in widget
void NodeRefListKeywordWidget::updateValue() { updateWidgetValues(coreData_); }

// Update widget values data based on keyword data
void NodeRefListKeywordWidget::updateWidgetValues(const CoreData &coreData)
{
    refreshing_ = true;

    // Get list of available nodes of the correct type and in the relevant scope
    RefList<ProcedureNode> availableNodes = keyword_->onlyInScope() ? keyword_->parentNode()->nodesInScope(keyword_->nodeType())
                                                                    : keyword_->parentNode()->nodes(keyword_->nodeType());
    ListWidgetUpdater<NodeRefListKeywordWidget, ProcedureNode> listUpdater(ui_.NodeList, availableNodes, this,
                                                                           &NodeRefListKeywordWidget::updateListRow);

    updateSummaryText();

    refreshing_ = false;
}

// Update keyword data based on widget values
void NodeRefListKeywordWidget::updateKeywordData()
{
    // Not relevant - Handled via widget callbacks
}

// Update summary text
void NodeRefListKeywordWidget::updateSummaryText()
{
    // TODO
}
