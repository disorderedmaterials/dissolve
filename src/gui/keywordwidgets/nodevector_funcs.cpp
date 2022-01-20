// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/coredata.h"
#include "gui/helpers/listwidgetupdater.h"
#include "gui/keywordwidgets/nodevector.h"
#include "procedure/procedure.h"
#include "templates/variantpointer.h"
#include <QLabel>

NodeVectorKeywordWidget::NodeVectorKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData)
    : KeywordDropDown(this), KeywordWidgetBase(coreData)
{
    // Create and set up the UI for our widget in the drop-down's widget container
    ui_.setupUi(dropWidget());
    ui_.NodeList->setModel(&nodeModel_);

    // Connect signals / slots
    connect(&nodeModel_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)), this,
            SLOT(modelDataChanged(const QModelIndex &, const QModelIndex &)));

    // Cast the pointer up into the parent class type
    keyword_ = dynamic_cast<NodeVectorKeyword *>(keyword);
    if (!keyword_)
        throw(std::runtime_error(fmt::format("Couldn't cast base keyword '{}' into NodeKeyword.\n", keyword->name())));
    nodeModel_.setCheckStateData(keyword_->data());

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

    // Get list of available nodes of the correct type and in the relevant scope
    std::vector<const ProcedureNode *> nodes;
    if (keyword_->onlyInScope())
        nodes = keyword_->parentNode()->nodesInScope(keyword_->nodeType(), keyword_->nodeClass());
    else
    {
        auto *proc = keyword_->parentNode()->procedure();
        if (proc)
            nodes = proc->nodes(keyword_->nodeType(), keyword_->nodeClass());
    }
    nodeModel_.setData(nodes);
    nodeModel_.setCheckStateData(keyword_->data());

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
    if (keyword_->data().empty())
        setSummaryText("<None>");
    else
    {
        std::string text;
        for (auto *node : keyword_->data())
            text += fmt::format("{}{}", text.empty() ? "" : ", ", node->name());
        setSummaryText(QString::fromStdString(text));
    }
}
