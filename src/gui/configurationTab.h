// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/units.h"
#include "gui/mainTab.h"
#include "gui/models/enumOptionsModel.h"
#include "gui/models/externalPotentialModel.h"
#include "gui/models/globalPotentialFilterProxy.h"
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
    // Global potential model
    ExternalPotentialModel globalPotentialModel_;
    // Global potential filter proxy
    GlobalPotentialFilterProxy globalPotentialFilterProxy_;
    // Restraint potential model
    ExternalPotentialModel targetedPotentialModel_;

    public:
    // Return displayed Configuration
    Configuration *configuration() const;

    /*
     * Update
     */
    private:
    // Update density label
    void updateDensityLabel();
    // Update temperature label
    void updateTemperatureLabel();

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
    private Q_SLOTS:
    // Generate
    void on_GenerateButton_clicked(bool checked);
    // Density units changed
    void on_DensityUnitsCombo_currentIndexChanged(int index);
    // Temperature button pressed
    void on_TemperatureToolButton_clicked(bool checked);
    void buttonGroupToggled(QAbstractButton *button, bool checked);
};
