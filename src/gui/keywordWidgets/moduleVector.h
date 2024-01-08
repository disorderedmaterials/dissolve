// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/dropDown.h"
#include "gui/keywordWidgets/ui_moduleVector.h"
#include "gui/models/moduleModel.h"
#include "keywords/moduleVector.h"
#include <QWidget>

// Forward Declarations
class Module;

class ModuleVectorKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    ModuleVectorKeywordWidget(QWidget *parent, ModuleVectorKeyword *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    ModuleVectorKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::ModuleVectorWidget ui_;
    // Vector of allowed module targets
    std::vector<Module *> allowedModules_;
    // Model for combo box
    ModuleModel moduleModel_;

    private Q_SLOTS:
    void modelDataChanged(const QModelIndex &, const QModelIndex &);

    Q_SIGNALS:
    // Keyword data changed
    void keywordDataChanged(int flags);

    /*
     * Update
     */
    private:
    // Reset model data
    void resetModelData();

    public:
    // Update value displayed in widget
    void updateValue(const Flags<DissolveSignals::DataMutations> &mutationFlags) override;
    // Update summary text
    void updateSummaryText();
};
