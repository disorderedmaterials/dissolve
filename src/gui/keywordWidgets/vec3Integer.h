// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/ui_vec3Integer.h"
#include "keywords/vec3Integer.h"
#include <QWidget>

class Vec3IntegerKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    Vec3IntegerKeywordWidget(QWidget *parent, Vec3IntegerKeyword *keyword, const CoreData &coreData);

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

    private Q_SLOTS:
    // Spin box value changed
    void on_Spin1_valueChanged(int value);
    void on_Spin2_valueChanged(int value);
    void on_Spin3_valueChanged(int value);

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
