// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/ui_modulevector.h"
#include "gui/models/moduleModel.h"
#include "keywords/modulevector.h"
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

    private slots:
    void modelDataChanged(const QModelIndex &, const QModelIndex &);

    signals:
    // Keyword data changed
    void keywordDataChanged(int flags);

    /*
     * Update
     */
    public:
    // Update value displayed in widget
    void updateValue() override;
    // Update widget values data based on keyword data
    void updateWidgetValues(const CoreData &coreData) override;
    // Update keyword data based on widget values
    void updateKeywordData() override;
    // Update summary text
    void updateSummaryText();
};
