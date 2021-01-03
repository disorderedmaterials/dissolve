// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/ui_nodeandinteger.h"
#include "keywords/nodeandinteger.h"
#include <QWidget>

// Forward Declarations
/* none */

class NodeAndIntegerKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    NodeAndIntegerKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);

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

    private slots:
    // Value changed
    void on_NodeCombo_currentIndexChanged(int index);

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
