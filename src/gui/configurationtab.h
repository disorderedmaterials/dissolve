/*
    *** Configuration Tab
    *** src/gui/configurationtab.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "base/units.h"
#include "gui/maintab.h"
#include "gui/ui_configurationtab.h"

// Forward Declarations
class Configuration;
class SpeciesInfo;

// Configuration Tab
class ConfigurationTab : public QWidget, public ListItem<ConfigurationTab>, public MainTab
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    ConfigurationTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title,
                     Configuration *cfg);
    ~ConfigurationTab();

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
    MainTab::TabType type() const;
    // Raise suitable dialog for entering / checking new tab name
    QString getNewTitle(bool &ok);
    // Return whether the title of the tab can be changed
    bool canChangeTitle() const;
    // Return whether the tab can be closed (after any necessary user querying, etc.)
    bool canClose() const;

    /*
     * Configuration Target
     */
    private:
    // Configuration data to display
    Configuration *configuration_;

    public:
    // Return displayed Configuration
    Configuration *configuration() const;

    /*
     * Update
     */
    private:
    // Row update function for BondsTable
    void updateSpeciesInfoTableRow(int row, SpeciesInfo *speciesInfo, bool createItems);
    // Update density label
    void updateDensityLabel();

    protected:
    // Update controls in tab
    void updateControls();
    // Disable sensitive controls within tab
    void disableSensitiveControls();
    // Enable sensitive controls within tab
    void enableSensitiveControls();

    /*
     * Signals / Slots
     */
    private slots:
    // Content
    void on_GeneratorRegenerateButton_clicked(bool checked);
    // Definition
    void on_TemperatureSpin_valueChanged(double value);
    // Current Box
    void on_DensityUnitsCombo_currentIndexChanged(int index);
    // Initial Coordinates
    void on_CoordinatesFileEdit_textChanged(QString text);
    void on_CoordinatesFileSelectButton_clicked(bool checked);
    // Size Factor Scaling
    void on_RequestedSizeFactorSpin_valueChanged(double value);

    /*
     * State
     */
    public:
    // Read widget state through specified LineParser
    bool readState(LineParser &parser, const CoreData &coreData);
    // Write widget state through specified LineParser
    bool writeState(LineParser &parser) const;
};
