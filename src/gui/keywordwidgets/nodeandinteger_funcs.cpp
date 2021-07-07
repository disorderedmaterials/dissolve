// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/helpers/comboboxcontroller.h"
#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/keywordwidgets/nodeandinteger.h"

Q_DECLARE_METATYPE(const ProcedureNode *)

NodeAndIntegerKeywordWidget::NodeAndIntegerKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData)
{
    // Setup our UI
    ui_.setupUi(this);

    refreshing_ = true;

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<NodeAndIntegerKeyword *>(keyword);
    if (!keyword_)
        throw(std::runtime_error(
            fmt::format("Couldn't cast base keyword '{}' into NodeAndIntegerKeywordBase.\n", keyword->name())));

    updateValue();

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    ui_.NodeCombo->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.NodeCombo));
    ui_.IntegerSpin->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.IntegerSpin));

    refreshing_ = false;
}

/*
 * Widgets
 */

// Value changed
void NodeAndIntegerKeywordWidget::on_NodeCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    // Get data from the selected item
    auto *node = ui_.NodeCombo->itemData(index, Qt::UserRole).value<const ProcedureNode *>();
    keyword_->setData({node, ui_.IntegerSpin->value()});

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void NodeAndIntegerKeywordWidget::updateValue()
{
    refreshing_ = true;

    // Get the list of available nodes of the specified type
    auto availableNodes = keyword_->onlyInScope() ? keyword_->parentNode()->nodesInScope(keyword_->nodeType())
                                                  : keyword_->parentNode()->nodes(keyword_->nodeType());
    combo_box_updater(ui_.NodeCombo, availableNodes.begin(), availableNodes.end(), [](auto item) { return item->name(); },
                      true);

    refreshing_ = false;
}
