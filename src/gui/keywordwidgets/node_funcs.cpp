// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/helpers/comboboxcontroller.h"
#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/keywordwidgets/node.h"

Q_DECLARE_METATYPE(const ProcedureNode *)

NodeKeywordWidget::NodeKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = true;

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<NodeKeyword *>(keyword);
    if (!keyword_)
        throw(std::runtime_error(fmt::format("Couldn't cast base keyword '{}' into NodeKeywordBase.\n", keyword->name())));

    updateValue();

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
    auto *node = ui_.NodeCombo->itemData(index, Qt::UserRole).value<const ProcedureNode *>();
    keyword_->setData(node);

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
    auto availableNodes = keyword_->onlyInScope()
                              ? keyword_->parentNode()->nodesInScope(keyword_->nodeType(), keyword_->nodeClass())
                              : keyword_->parentNode()->nodes(keyword_->nodeType(), keyword_->nodeClass());
    combo_box_updater(ui_.NodeCombo, availableNodes.begin(), availableNodes.end(), [](auto *item) { return item->name(); });

    refreshing_ = false;
}
