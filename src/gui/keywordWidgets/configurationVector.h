// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/dropDown.h"
#include "gui/keywordWidgets/ui_configurationVector.h"
#include "gui/models/configurationModel.h"
#include "keywords/configurationVector.h"
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

    private Q_SLOTS:
    void modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);

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
