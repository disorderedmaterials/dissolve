// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/coredata.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/keywordwidgets/nodevector.h"
#include "procedure/procedure.h"
#include "templates/algorithms.h"
#include <QLabel>

NodeVectorKeywordWidget::NodeVectorKeywordWidget(QWidget *parent, NodeVectorKeywordBase *keyword, const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData), keyword_(keyword)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());
    ui_.NodeList->setModel(&nodeModel_);

    // Connect signals / slots
    connect(&nodeModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));

    allowedNodes_ = keyword_->allowedNodes();
    nodeModel_.setData(allowedNodes_);
    nodeModel_.setNodeSelectedFunction([&](ConstNodeRef node) { return keyword_->addNode(node); });
    nodeModel_.setNodeDeselectedFunction([&](ConstNodeRef node) { return keyword_->removeNode(node); });
    nodeModel_.setNodePresenceFunction([&](ConstNodeRef node) { return keyword_->isPresent(node); });

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

    keyword_->setAsModified();

    updateSummaryText();

    emit(keywordValueChanged(keyword_->optionMask()));
}

/*
 * Update
 */

// Update value displayed in widget
void NodeVectorKeywordWidget::updateValue() { updateWidgetValues(coreData_); }

// Update widget values data based on keyword data
void NodeVectorKeywordWidget::updateWidgetValues(const CoreData &coreData)
{
    refreshing_ = true;

    updateSummaryText();

    refreshing_ = false;
}

// Update keyword data based on widget values
void NodeVectorKeywordWidget::updateKeywordData()
{
    // Not relevant - Handled via widget callbacks
}

// Update summary text
void NodeVectorKeywordWidget::updateSummaryText()
{
    auto nodes{keyword_->nodes()};
    if (nodes.empty())
        setSummaryText("<None>");
    else
        setSummaryText(QString::fromStdString(joinStrings(nodes, ", ", [](const auto &node) { return node->name(); })));
}
