// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/ui_vec3integer.h"
#include "keywords/vec3integer.h"
#include <QWidget>

// Forward Declarations
/* none */

class Vec3IntegerKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    Vec3IntegerKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    Vec3IntegerKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::Vec3IntegerWidget ui_;

    private slots:
    // Spin box value changed
    void on_Spin1_valueChanged(int value);
    void on_Spin2_valueChanged(int value);
    void on_Spin3_valueChanged(int value);

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
