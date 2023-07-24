// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "gui/mainTab.h"
#include "gui/models/procedureModel.h"
#include "gui/ui_controlLayerTab.h"
class ControlLayerTab : public QWidget, public MainTab
{
    // All Qt declarations derived from QObject must include this macro
    Q_OBJECT

    public:
    ControlLayerTab(DissolveWindow *dissolveWindow, Dissolve &dissolve, MainTabsWidget *parent, QString title);
    ~ControlLayerTab() override = default;

    /*
     * UI
     */
    private:
    // Main form declaration
    Ui::ControlLayerTab ui_;

    /*
     * MainTab Reimplementations
     */
    public:
    // Return tab type
    MainTab::TabType type() const override;
    // Return whether the tab can be closed (after any necessary user querying, etc.)
    bool canClose() const override;

    private:
    ProcedureModel procedureModel_;
        
    public:
    // update controls in tab
    void updateControls() override;
    // prevent editing within tab
    void preventEditing() override;
    // allow editing within tab
    void allowEditing() override;

    /*
     * Signals / Slots
     */
    private slots:
    // Generate
    void on_RunButton_clicked(bool checked);
};
