// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/ui_nodeAndInteger.h"
#include "gui/models/procedureNodeModel.h"
#include "keywords/nodeAndInteger.h"
#include <QWidget>

class NodeAndIntegerKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    NodeAndIntegerKeywordWidget(QWidget *parent, NodeAndIntegerKeywordBase *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    NodeAndIntegerKeywordBase *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::NodeAndIntegerValueWidget ui_;
    // Allowed nodes to display in the model
    std::vector<ConstNodeRef> allowedNodes_;
    // Model for the node list
    ProcedureNodeModel nodeModel_;

    private slots:
    void on_IntegerSpin_valueChanged(int value);
    void modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

    signals:
    // Keyword data changed
    void keywordDataChanged(int flags);

    /*
     * Update
     */
    public:
    // Update value displayed in widget
    void updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) override;
};
