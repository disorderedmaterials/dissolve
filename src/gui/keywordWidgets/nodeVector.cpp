// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "gui/keywordWidgets/nodeVector.h"
#include "classes/coreData.h"
#include "procedure/procedure.h"
#include "templates/algorithms.h"
#include <QLabel>

NodeVectorKeywordWidget::NodeVectorKeywordWidget(QWidget *parent, NodeVectorKeywordBase *keyword, const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());

    // Set up the model
    ui_.NodeList->setModel(&nodeModel_);
    allowedNodes_ = keyword_->allowedNodes();
    nodeModel_.setNodeSelectedFunction([&](ConstNodeRef node) { return keyword_->addNode(node); });
    nodeModel_.setNodeDeselectedFunction([&](ConstNodeRef node) { return keyword_->removeNode(node); });
    nodeModel_.setNodePresenceFunction([&](ConstNodeRef node) { return keyword_->isPresent(node); });
    resetModelData();

    // Connect signals / slots
    connect(&nodeModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));

    // Summary text on KeywordDropDown button
    setSummaryText("Edit...");
}

/*
 * Widgets
 */

void NodeVectorKeywordWidget::modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    if (refreshing_)
        return;

    updateSummaryText();

    Q_EMIT(keywordDataChanged(keyword_->editSignals()));
}

/*
 * Update
 */

// Reset model data
void NodeVectorKeywordWidget::resetModelData()
{
    refreshing_ = true;

    nodeModel_.setData(allowedNodes_);

    updateSummaryText();

    refreshing_ = false;
}

// Update value displayed in widget
void NodeVectorKeywordWidget::updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) { resetModelData(); }

// Update summary text
void NodeVectorKeywordWidget::updateSummaryText()
{
    auto nodes{keyword_->nodes()};
    if (nodes.empty())
        setSummaryText("<None>");
    else
        setSummaryText(QString::fromStdString(joinStrings(nodes, ", ", [](const auto &node) { return node->name(); })));
}
