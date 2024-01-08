// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/dropDown.h"
#include "gui/keywordWidgets/ui_nodeVector.h"
#include "gui/models/procedureNodeModel.h"
#include "keywords/nodeVector.h"
#include <QWidget>

class NodeVectorKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    NodeVectorKeywordWidget(QWidget *parent, NodeVectorKeywordBase *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    NodeVectorKeywordBase *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::NodeRefListWidget ui_;
    // Allowed nodes to display in the model
    std::vector<ConstNodeRef> allowedNodes_;
    // Model for the node list
    ProcedureNodeModel nodeModel_;

    private Q_SLOTS:
    void modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    Q_SIGNALS:
    // Keyword data changed
    void keywordDataChanged(int flags);

    /*
     * Update
     */
    private:
    // Reset model data
    void resetModelData();

    public:
    // Update value displayed in widget
    void updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) override;
    // Update summary text
    void updateSummaryText();
};
