// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "gui/maintab.h"
#include "gui/models/moduleLayerModel.h"
#include "gui/models/modulePaletteModel.h"
#include "gui/ui_layertab.h"

// Forward Declarations
class ModuleLayer;

// Layer Tab
class LayerTab : public QWidget, public MainTab
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    LayerTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, const QString title,
             ModuleLayer *layer);
    ~LayerTab() = default;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::LayerTab ui_;
    // ModuleLayer data to display
    ModuleLayer *moduleLayer_;
    // Model for module layer
    ModuleLayerModel moduleLayerModel_;
    // Model for module palette
    ModulePaletteModel modulePaletteModel_;

    public:
    // Return displayed ModuleLayer
    ModuleLayer *moduleLayer() const;

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
     * Widgets
     */
    private slots:
    void on_ShowAvailableModulesButton_clicked(bool checked);
    void on_EnabledButton_clicked(bool checked);
    void on_FrequencySpin_valueChanged(int value);
    void moduleSelectionChanged(const QItemSelection &current, const QItemSelection &previous);
    void layerDataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &);
    void updateModuleList();

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
