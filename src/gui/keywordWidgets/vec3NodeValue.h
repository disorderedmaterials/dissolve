// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/ui_vec3NodeValue.h"
#include "keywords/vec3NodeValue.h"
#include <QWidget>

class Vec3NodeValueKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    Vec3NodeValueKeywordWidget(QWidget *parent, Vec3NodeValueKeyword *keyword, const CoreData &coreData);

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

    private Q_SLOTS:
    // Values edited
    void on_ValueAEdit_editingFinished();
    void on_ValueAEdit_returnPressed();
    void on_ValueBEdit_editingFinished();
    void on_ValueBEdit_returnPressed();
    void on_ValueCEdit_editingFinished();
    void on_ValueCEdit_returnPressed();

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
