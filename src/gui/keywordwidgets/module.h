// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/ui_module.h"
#include "keywords/module.h"
#include <QWidget>

// Forward Declarations
/* none */

class ModuleKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    ModuleKeywordWidget(QWidget *parent, KeywordBase *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    ModuleKeywordBase *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::ModuleWidget ui_;

    private slots:
    // Value changed
    void on_ModuleCombo_currentIndexChanged(int index);

    signals:
    // Keyword value changed
    void keywordValueChanged(int flags);

    /*
     * Update
     */
    private:
    // Return text (for ComboBox item) for supplied Module
    QString uniqueNameOfModule(const Module *module);

    public:
    // Update value displayed in widget
    void updateValue();
};
