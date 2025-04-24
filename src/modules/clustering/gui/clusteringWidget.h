// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#pragma once

#include "gui/configurationViewer.h"
#include "gui/configurationViewerWidget.h"
#include "modules/clustering/gui/ui_clusteringWidget.h"
#include "modules/widget.h"
#include <QListWidget>

// Forward Declarations
class ClusteringModule;

// Module Widget
class ClusteringModuleWidget : public ModuleWidget
{
    Q_OBJECT

    private:
    // Associated Module
    ClusteringModule *module_;
    // Local pointer to the cluster configuration
    Configuration *clusterConfiguration_{nullptr};
    // Integers for which clusters to display
    int displaySize_{0}, displayID_{0};
    // Standard stuff
    bool refreshing_{false};
    // Prevents selection in ID list forcing size list update
    bool fromBuilder_{false};
    // Prevents unnecessary recalculation of clusters config
    bool getNewConfig_{false};

    public:
    ClusteringModuleWidget(QWidget *parent, ClusteringModule *module, Dissolve &dissolve);
    void updateControls(const Flags<ModuleWidget::UpdateFlags> &updateFlags = {}) override;

    private:
    void buildSizeList();
    void buildIDList(QListWidgetItem *item);
    void buildCNList();

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::ClusteringModuleWidget ui_;
    DataViewer *sizeDist_{nullptr}, *massDist_{nullptr};

    private Q_SLOTS:
    void on_clusterSizeList_itemClicked(QListWidgetItem *item);
    void on_clusterIDList_itemClicked(QListWidgetItem *item);
};
