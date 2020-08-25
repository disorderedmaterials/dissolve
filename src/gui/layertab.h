/*
    *** Layer Tab
    *** src/gui/layertab.h
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

#include "gui/maintab.h"
#include "gui/ui_layertab.h"

// Forward Declarations
class ModuleChart;
class ModulePalette;

// Layer Tab
class LayerTab : public QWidget, public ListItem<LayerTab>, public MainTab
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    LayerTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title,
             ModuleLayer *layer);
    ~LayerTab();

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::LayerTab ui_;

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
     * ModuleLayer Target
     */
    private:
    // ModuleLayer data to display
    ModuleLayer *moduleLayer_;

    public:
    // Return displayed ModuleLayer
    ModuleLayer *moduleLayer() const;

    /*
     * Widgets
     */
    public slots:
    void on_ShowPaletteButton_clicked(bool checked);
    void on_EnabledButton_clicked(bool checked);
    void on_FrequencySpin_valueChanged(int value);

    /*
     * Update
     */
    protected:
    // Update controls in tab
    void updateControls();
    // Disable sensitive controls within tab
    void disableSensitiveControls();
    // Enable sensitive controls within tab
    void enableSensitiveControls();

    /*
     * State
     */
    public:
    // Read widget state through specified LineParser
    bool readState(LineParser &parser, const CoreData &coreData);
    // Write widget state through specified LineParser
    bool writeState(LineParser &parser) const;
};
