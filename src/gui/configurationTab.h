// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/units.h"
#include "gui/mainTab.h"
#include "gui/models/enumOptionsModel.h"
#include "gui/models/procedureModel.h"
#include "gui/ui_configurationTab.h"
#include <map>

// Forward Declarations
class Configuration;

// Configuration Tab
class ConfigurationTab : public QWidget, public MainTab
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    ConfigurationTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, QString title,
                     Configuration *cfg);
    ~ConfigurationTab() override = default;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::ConfigurationTab ui_;
    // Model for import file format
    EnumOptionsModel importEnumOptionsModel_;

    /*
     * MainTab Reimplementations
     */
    public:
    // Return tab type
    MainTab::TabType type() const override;
    // Raise suitable dialog for entering / checking new tab name
    QString getNewTitle(bool &ok) override;
    // Return whether the title of the tab can be changed
    bool canChangeTitle() const override;
    // Return whether the tab can be closed (after any necessary user querying, etc.)
    bool canClose() const override;

    /*
     * Configuration Target
     */
    private:
    // Configuration data to display
    Configuration *configuration_;
    // Procedure Model
    ProcedureModel procedureModel_;

    public:
    // Return displayed Configuration
    Configuration *configuration() const;

    /*
     * Update
     */
    private:
    // Update density label
    void updateDensityLabel();

    public:
    // Update controls in tab
    void updateControls() override;
    // Prevent editing within tab
    void preventEditing() override;
    // Allow editing within tab
    void allowEditing() override;

    /*
     * Signals / Slots
     */
    private slots:
    // Content
    void on_GenerateButton_clicked(bool checked);
    // Definition
    void on_TemperatureSpin_valueChanged(double value);
    // Current Box
    void on_DensityUnitsCombo_currentIndexChanged(int index);
    // Initial Coordinates
    void on_CoordinatesFileEdit_textChanged(QString text);
    void on_CoordinatesFileSelectButton_clicked(bool checked);
    // Size Factor Scaling
    void on_RequestedSizeFactorSpin_valueChanged(double value);
};
