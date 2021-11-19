// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "gui/helpers/comboboxcontroller.h"
#include "gui/helpers/mousewheeladjustmentguard.h"
#include "gui/keywordwidgets/nodeandinteger.h"

Q_DECLARE_METATYPE(const ProcedureNode *)

NodeAndIntegerKeywordWidget::NodeAndIntegerKeywordWidget(QWidget *parent, NodeAndIntegerKeywordBase *keyword,
                                                         const CoreData &coreData)
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
    ui_.IntegerSpin->installEventFilter(new MouseWheelWidgetAdjustmentGuard(ui_.IntegerSpin));

    refreshing_ = false;
}

/*
 * Widgets
 */

void NodeAndIntegerKeywordWidget::on_IntegerSpin_valueChanged(int value)
{
    if (refreshing_)
        return;

    keyword_->setIndex(value);
    keyword_->setAsModified();

    emit(keywordValueChanged(keyword_->optionMask()));
}

void NodeAndIntegerKeywordWidget::modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    if (refreshing_)
        return;

    keyword_->setNode(allowedNodes_[ui_.NodeCombo->currentIndex()]);
    keyword_->setAsModified();

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void NodeAndIntegerKeywordWidget::updateValue() {}
