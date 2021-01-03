// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/ui_nodevalue.h"
#include "keywords/nodevalue.h"
#include <QWidget>

// Forward Declarations
/* none */

class NodeValueKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    NodeValueKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);

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

    private slots:
    // Value edited
    void on_ValueEdit_editingFinished();
    void on_ValueEdit_returnPressed();

    signals:
    // Keyword value changed
    void keywordValueChanged(int flags);

    /*
     * Update
     */
    private:
    // Check validity of current value
    void checkValueValidity();

    public:
    // Update value displayed in widget
    void updateValue();
};
