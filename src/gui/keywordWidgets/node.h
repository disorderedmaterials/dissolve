// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/ui_node.h"
#include "gui/models/procedureNodeModel.h"
#include "keywords/node.h"
#include <QWidget>

class NodeKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    NodeKeywordWidget(QWidget *parent, NodeKeywordBase *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    NodeKeywordBase *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::NodeWidget ui_;
    // Allowed nodes to display in the model
    std::vector<ConstNodeRef> allowedNodes_;
    // Model for the node list
    ProcedureNodeModel nodeModel_;

    private Q_SLOTS:
    void on_NodeCombo_currentIndexChanged(int index);
    void on_ClearButton_clicked(bool checked);

    Q_SIGNALS:
    // Keyword data changed
    void keywordDataChanged(int flags);

    /*
     * Update
     */
    public:
    // Update value displayed in widget
    void updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) override;
};
