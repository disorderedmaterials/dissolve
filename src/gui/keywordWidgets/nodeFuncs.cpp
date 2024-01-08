// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "gui/helpers/mouseWheelAdjustmentGuard.h"
#include "gui/keywordWidgets/node.h"

NodeKeywordWidget::NodeKeywordWidget(QWidget *parent, NodeKeywordBase *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Setup our UI
    ui_.setupUi(this);
    ui_.NodeCombo->setModel(&nodeModel_);

    refreshing_ = true;

    // Get allowed nodes, set model for combo box, and set current index
    allowedNodes_ = keyword_->allowedNodes();
    nodeModel_.setData(allowedNodes_);
    auto it = std::find_if(allowedNodes_.begin(), allowedNodes_.end(),
                           [&](const auto &node) { return node.get() == keyword_->baseNode().get(); });
    ui_.NodeCombo->setCurrentIndex(it == allowedNodes_.end() ? -1 : it - allowedNodes_.begin());

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    ui_.NodeCombo->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.NodeCombo));

    refreshing_ = false;
}

/*
 * Widgets
 */

void NodeKeywordWidget::on_NodeCombo_currentIndexChanged(int index)
{
    if (refreshing_)
        return;

    // Get data from the selected item
    if (index == -1)
        keyword_->setData(nullptr);
    else
        keyword_->setData(allowedNodes_[ui_.NodeCombo->currentIndex()]);

    Q_EMIT(keywordDataChanged(keyword_->editSignals()));
}

void NodeKeywordWidget::on_ClearButton_clicked(bool checked)
{
    if (ui_.NodeCombo->currentIndex() == -1 && !keyword_->baseNode())
        return;

    ui_.NodeCombo->setCurrentIndex(-1);
}

/*
 * Update
 */

// Update value displayed in widget
void NodeKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags)
{
    refreshing_ = true;

    auto it = std::find(allowedNodes_.begin(), allowedNodes_.end(), keyword_->baseNode());
    ui_.NodeCombo->setCurrentIndex(it == allowedNodes_.end() ? -1 : it - allowedNodes_.begin());

    refreshing_ = false;
}
