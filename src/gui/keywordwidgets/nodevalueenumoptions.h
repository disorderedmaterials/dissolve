// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/ui_nodevalueenumoptions.h"
#include "keywords/nodevalueenumoptions.h"
#include <QWidget>

// Forward Declarations
/* none */

class NodeValueEnumOptionsKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    NodeValueEnumOptionsKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);

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
    // Keyword value changed
    void keywordValueChanged(int flags);

    /*
     * Update
     */
    public:
    // Update value displayed in widget
    void updateValue();
};
