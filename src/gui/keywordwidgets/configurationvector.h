// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "gui/keywordwidgets/base.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/keywordwidgets/ui_configurationvector.h"
#include "gui/models/configurationModel.h"
#include "keywords/configurationvector.h"
#include <QWidget>

// Forward Declarations
class Configuration;

class ConfigurationVectorKeywordWidget : public KeywordDropDown, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    ConfigurationVectorKeywordWidget(QWidget *parent, ConfigurationVectorKeyword *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    ConfigurationVectorKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::ConfigurationVectorWidget ui_;
    // Model for Configuration
    ConfigurationModel configurationModel_;

    private slots:
    void modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

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
