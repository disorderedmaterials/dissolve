// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/ui_module.h"
#include "gui/models/moduleModel.h"
#include "keywords/module.h"
#include <QWidget>

class ModuleKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    ModuleKeywordWidget(QWidget *parent, ModuleKeywordBase *keyword, const CoreData &coreData);

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
    // Vector of allowed module targets
    std::vector<Module *> allowedModules_;
    // Model for combo box
    ModuleModel moduleModel_;

    private slots:
    // Value changed
    void on_ModuleCombo_currentIndexChanged(int index);

    signals:
    // Keyword data changed
    void keywordDataChanged(int flags);

    /*
     * Update
     */
    public:
    // Update value displayed in widget
    void updateValue() override;
};
