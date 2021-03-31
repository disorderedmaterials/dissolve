// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
};
