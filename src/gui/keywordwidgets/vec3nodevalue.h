// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/ui_vec3nodevalue.h"
#include "keywords/vec3nodevalue.h"
#include <QWidget>

// Forward Declarations
/* none */

class Vec3NodeValueKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    Vec3NodeValueKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    Vec3NodeValueKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::Vec3NodeValueWidget ui_;

    private slots:
    // Values edited
    void on_ValueAEdit_editingFinished();
    void on_ValueAEdit_returnPressed();
    void on_ValueBEdit_editingFinished();
    void on_ValueBEdit_returnPressed();
    void on_ValueCEdit_editingFinished();
    void on_ValueCEdit_returnPressed();

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
