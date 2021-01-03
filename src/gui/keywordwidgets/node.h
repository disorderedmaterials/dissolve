// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/ui_node.h"
#include "keywords/node.h"
#include <QWidget>

// Forward Declarations
/* none */

class NodeKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    NodeKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);

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
