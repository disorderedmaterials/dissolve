// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/ui_nodeValue.h"
#include "keywords/nodeValue.h"
#include <QWidget>

class NodeValueKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    NodeValueKeywordWidget(QWidget *parent, NodeValueKeyword *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    NodeValueKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::NodeValueWidget ui_;

    private Q_SLOTS:
    // Value edited
    void on_ValueEdit_editingFinished();
    void on_ValueEdit_returnPressed();

    Q_SIGNALS:
    // Keyword data changed
    void keywordDataChanged(int flags);

    /*
     * Update
     */
    private:
    // Check validity of current value
    void checkValueValidity();

    public:
    // Update value displayed in widget
    void updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) override;
};
