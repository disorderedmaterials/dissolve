// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/ui_module.h"
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

    private Q_SLOTS:
    void on_ModuleCombo_currentIndexChanged(int index);
    void on_ClearButton_clicked(bool checked);

    Q_SIGNALS:
    // Keyword data changed
    void keywordDataChanged(int flags);

    /*
     * Update
     */
    private:
    // Check / update allowed modules and displayed data
    void updateAllowedModules();

    public:
    // Update value displayed in widget
    void updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) override;
};
