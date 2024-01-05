// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/ui_nodeValueEnumOptions.h"
#include "keywords/nodeValueEnumOptions.h"
#include <QWidget>

class NodeValueEnumOptionsKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    NodeValueEnumOptionsKeywordWidget(QWidget *parent, NodeValueEnumOptionsBaseKeyword *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    NodeValueEnumOptionsBaseKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::NodeValueEnumOptionsWidget ui_;

    private slots:
    // Value edited
    void on_ValueEdit_editingFinished();
    void on_ValueEdit_returnPressed();
    void on_OptionsCombo_currentIndexChanged(int index);

    signals:
    // Keyword data changed
    void keywordDataChanged(int flags);

    /*
     * Update
     */
    public:
    // Check validity of current value
    void checkValueValidity();
    // Update value displayed in widget
    void updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) override;
};
