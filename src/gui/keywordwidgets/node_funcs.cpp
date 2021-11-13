// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/helpers/comboboxcontroller.h"
#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/keywordwidgets/node.h"

NodeKeywordWidget::NodeKeywordWidget(QWidget *parent, NodeKeywordBase *keyword, const CoreData &coreData)
    : QWidget(parent), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Setup our UI
    ui_.setupUi(this);
    ui_.NodeCombo->setModel(&nodeModel_);

    refreshing_ = true;

    // Connect signals / slots
    connect(&nodeModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));

    // Get allowed nodes, set model for combo box, and set current index
    allowedNodes_ = keyword_->allowedNodes();
    nodeModel_.setData(allowedNodes_);
    auto it = std::find(allowedNodes_.begin(), allowedNodes_.end(), keyword_->baseNode());
    if (it != allowedNodes_.end())
        ui_.NodeCombo->setCurrentIndex(it - allowedNodes_.begin());

    // Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a
    // QScrollArea)
    ui_.NodeCombo->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.NodeCombo));

    refreshing_ = false;
}

/*
 * Widgets
 */

void NodeKeywordWidget::modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    if (refreshing_)
        return;

    keyword_->setData(allowedNodes_[ui_.NodeCombo->currentIndex()]);
    keyword_->setAsModified();

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void NodeKeywordWidget::updateValue() {}
