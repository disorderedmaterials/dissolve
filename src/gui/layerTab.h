// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "gui/mainTab.h"
#include "gui/models/moduleLayerModel.h"
#include "gui/models/modulePaletteModel.h"
#include "gui/ui_layerTab.h"

// Forward Declarations
class ModuleControlWidget;
class ModuleLayer;

// Layer Tab
class LayerTab : public QWidget, public MainTab
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    LayerTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, QString title, ModuleLayer *layer);
    ~LayerTab() override = default;

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
    MainTab::TabType type() const override;
    // Raise suitable dialog for entering / checking new tab name
    QString getNewTitle(bool &ok) override;
    // Return whether the title of the tab can be changed
    bool canChangeTitle() const override;
    // Return whether the tab can be closed (after any necessary user querying, etc.)
    bool canClose() const override;

    /*
     * Widgets
     */
    private:
    // Return ModuleControlWidget for the specified Module (if it exists)
    ModuleControlWidget *getControlWidget(const Module *module, bool setAsCurrent = false);
    // Remove ModuleControlWidget for the specified Module (if it exists)
    void removeControlWidget(const Module *module);

    private Q_SLOTS:
    void on_ShowAvailableModulesButton_clicked(bool checked);
    void on_LayerEnabledButton_clicked(bool checked);
    void on_LayerFrequencySpin_valueChanged(int value);
    void on_RunControlEnergyStabilityCheck_clicked(bool checked);
    void on_RunControlSizeFactorsCheck_clicked(bool checked);
    void moduleSelectionChanged(const QItemSelection &current, const QItemSelection &previous);
    void moduleNameChanged(const QModelIndex &, const QString &oldName, const QString &newName);
    void layerDataChanged(const QModelIndex &, const QModelIndex &, const QList<int> &);
    void updateModuleList();
    void on_ModulesTable_customContextMenuRequested(const QPoint &pos);
    void on_AvailableModulesTree_doubleClicked(const QModelIndex &index);

    public:
    // Remove all module control widgets
    void removeModuleControlWidgets();

    /*
     * Update
     */
    public:
    // Update controls in tab
    void updateControls() override;
    // Prevent editing within tab
    void preventEditing() override;
    // Allow editing within tab
    void allowEditing() override;
};
