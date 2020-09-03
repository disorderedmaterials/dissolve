/*
    *** Keyword Widget - Node
    *** src/gui/keywordwidgets/node_funcs.cpp
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

#include "gui/helpers/comboboxupdater.h"
#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/keywordwidgets/node.h"

NodeKeywordWidget::NodeKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = true;

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<NodeKeywordBase *>(keyword);
    if (!keyword_)
        Messenger::error("Couldn't cast base keyword '{}' into NodeKeywordBase.\n", keyword->name());
    else
    {
        updateValue();
    }

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    ui_.NodeCombo->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.NodeCombo));

    refreshing_ = false;
}

/*
 * Widgets
 */

// Value changed
void NodeKeywordWidget::on_NodeCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    // Get data from the selected item
    ProcedureNode *node = VariantPointer<ProcedureNode>(ui_.NodeCombo->itemData(index, Qt::UserRole));
    keyword_->setNode(node);

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void NodeKeywordWidget::updateValue()
{
    refreshing_ = true;

    // Get the list of available nodes of the specified type
    RefList<ProcedureNode> availableNodes = keyword_->onlyInScope() ? keyword_->parentNode()->nodesInScope(keyword_->nodeType())
                                                                    : keyword_->parentNode()->nodes(keyword_->nodeType());
    ComboBoxUpdater<ProcedureNode> comboUpdater(ui_.NodeCombo, availableNodes, keyword_->procedureNode());

    refreshing_ = false;
}
