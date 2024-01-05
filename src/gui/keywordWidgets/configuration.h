// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/keywordWidgets/base.h"
#include "gui/keywordWidgets/ui_configuration.h"
#include "gui/models/configurationModel.h"
#include "keywords/configuration.h"
#include <QWidget>

class ConfigurationKeywordWidget : public QWidget, public KeywordWidgetBase
{
    // All Qt declarations must include this macro
    Q_OBJECT

    public:
    ConfigurationKeywordWidget(QWidget *parent, ConfigurationKeyword *keyword, const CoreData &coreData);

    /*
     * Keyword
     */
    private:
    // Associated keyword
    ConfigurationKeyword *keyword_;

    /*
     * Widgets
     */
    private:
    // Main form declaration
    Ui::ConfigurationWidget ui_;
    // Model for combo box
    ConfigurationModel configurationModel_;

    private slots:
    void on_ConfigurationCombo_currentIndexChanged(int index);
    void on_ClearButton_clicked(bool checked);

    signals:
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
};
